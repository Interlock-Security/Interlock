package keeper_test

import (
	"time"

	cryptocodec "github.com/cosmos/cosmos-sdk/crypto/codec"
	"github.com/cosmos/cosmos-sdk/crypto/keys/ed25519"
	clienttypes "github.com/cosmos/ibc-go/modules/core/02-client/types"
	channeltypes "github.com/cosmos/ibc-go/modules/core/04-channel/types"
	"github.com/cosmos/interchain-security/app"
	childtypes "github.com/cosmos/interchain-security/x/ccv/child/types"
	parenttypes "github.com/cosmos/interchain-security/x/ccv/parent/types"
	"github.com/cosmos/interchain-security/x/ccv/types"
	ccv "github.com/cosmos/interchain-security/x/ccv/types"
	abci "github.com/tendermint/tendermint/abci/types"
)

func (suite *KeeperTestSuite) TestOnRecvPacket() {
	// setup CCV channel
	suite.SetupCCVChannel()

	pk1, err := cryptocodec.ToTmProtoPublicKey(ed25519.GenPrivKey().PubKey())
	suite.Require().NoError(err)
	pk2, err := cryptocodec.ToTmProtoPublicKey(ed25519.GenPrivKey().PubKey())
	suite.Require().NoError(err)

	pd := types.NewValidatorSetChangePacketData(
		[]abci.ValidatorUpdate{
			{
				PubKey: pk1,
				Power:  30,
			},
			{
				PubKey: pk2,
				Power:  20,
			},
		},
	)

	packet := channeltypes.NewPacket(pd.GetBytes(), 1, parenttypes.PortID, suite.path.EndpointB.ChannelID, childtypes.PortID, suite.path.EndpointA.ChannelID,
		clienttypes.NewHeight(1, 0), 0)

	testCases := []struct {
		name           string
		malleatePacket func()
		expErrorAck    bool
	}{
		{
			"success on first packet",
			func() {},
			false,
		},
		{
			"success on subsequent packet",
			func() {
				packet.Sequence = 2
			},
			false,
		},
		{
			"invalid packet: different destination channel than parent channel",
			func() {
				packet.Sequence = 1
				// change destination channel to different channelID than parent channel
				packet.DestinationChannel = "invalidChannel"
			},
			true,
		},
	}

	for _, tc := range testCases {
		// malleate packet for each case
		tc.malleatePacket()

		ack := suite.childChain.App.(*app.App).ChildKeeper.OnRecvPacket(suite.ctx, packet, pd)

		if tc.expErrorAck {
			suite.Require().NotNil(ack, "invalid test case: %s did not return ack", tc.name)
			suite.Require().False(ack.Success(), "invalid test case: %s did not return an Error Acknowledgment")
		} else {
			suite.Require().Nil(ack, "successful packet must send ack asynchronously. case: %s", tc.name)
			suite.Require().Equal(ccv.VALIDATING, suite.childChain.App.(*app.App).ChildKeeper.GetChannelStatus(suite.ctx, suite.path.EndpointA.ChannelID),
				"channel status is not valdidating after receive packet for valid test case: %s", tc.name)
			parentChannel, ok := suite.childChain.App.(*app.App).ChildKeeper.GetParentChannel(suite.ctx)
			suite.Require().True(ok)
			suite.Require().Equal(packet.DestinationChannel, parentChannel,
				"parent channel is not destination channel on successful receive for valid test case: %s", tc.name)
			actualPd, ok := suite.childChain.App.(*app.App).ChildKeeper.GetPendingChanges(suite.ctx)
			suite.Require().True(ok)
			suite.Require().Equal(&pd, actualPd, "pending changes not equal to packet data after successful packet receive. case: %s", tc.name)
			expectedTime := uint64(suite.ctx.BlockTime().Add(childtypes.UnbondingTime).UnixNano())
			unbondingTime := suite.childChain.App.(*app.App).ChildKeeper.GetUnbondingTime(suite.ctx, packet.Sequence)
			suite.Require().Equal(expectedTime, unbondingTime, "unbonding time has unexpected value for case: %s", tc.name)
			unbondingPacket, err := suite.childChain.App.(*app.App).ChildKeeper.GetUnbondingPacket(suite.ctx, packet.Sequence)
			suite.Require().NoError(err)
			suite.Require().Equal(&packet, unbondingPacket, "packet is not added to unbonding queue after successful receive. case: %s", tc.name)
		}
	}
}

func (suite *KeeperTestSuite) TestUnbondMaturePackets() {
	// setup CCV channel
	suite.SetupCCVChannel()

	// send 3 packets to child chain at different times
	pk1, err := cryptocodec.ToTmProtoPublicKey(ed25519.GenPrivKey().PubKey())
	suite.Require().NoError(err)
	pk2, err := cryptocodec.ToTmProtoPublicKey(ed25519.GenPrivKey().PubKey())
	suite.Require().NoError(err)

	pd := types.NewValidatorSetChangePacketData(
		[]abci.ValidatorUpdate{
			{
				PubKey: pk1,
				Power:  30,
			},
			{
				PubKey: pk2,
				Power:  20,
			},
		},
	)

	origTime := suite.ctx.BlockTime()

	// send first packet
	packet := channeltypes.NewPacket(pd.GetBytes(), 1, parenttypes.PortID, suite.path.EndpointB.ChannelID, childtypes.PortID, suite.path.EndpointA.ChannelID,
		clienttypes.NewHeight(1, 0), 0)
	ack := suite.childChain.App.(*app.App).ChildKeeper.OnRecvPacket(suite.ctx, packet, pd)
	suite.Require().Nil(ack)

	// update time and send second packet
	suite.ctx = suite.ctx.WithBlockTime(suite.ctx.BlockTime().Add(time.Hour))
	pd.ValidatorUpdates[0].Power = 15
	packet.Data = pd.GetBytes()
	packet.Sequence = 2
	ack = suite.childChain.App.(*app.App).ChildKeeper.OnRecvPacket(suite.ctx, packet, pd)
	suite.Require().Nil(ack)

	// update time and send third packet
	suite.ctx = suite.ctx.WithBlockTime(suite.ctx.BlockTime().Add(24 * time.Hour))
	pd.ValidatorUpdates[1].Power = 40
	packet.Data = pd.GetBytes()
	packet.Sequence = 3
	ack = suite.childChain.App.(*app.App).ChildKeeper.OnRecvPacket(suite.ctx, packet, pd)
	suite.Require().Nil(ack)

	// move ctx time forward such that first two packets are unbonded but third is not.
	suite.ctx = suite.ctx.WithBlockTime(origTime.Add(childtypes.UnbondingTime).Add(3 * time.Hour))

	suite.childChain.App.(*app.App).ChildKeeper.UnbondMaturePackets(suite.ctx)

	// ensure first two packets are unbonded and acknowledgement is written
	// unbonded time is deleted
	time1 := suite.childChain.App.(*app.App).ChildKeeper.GetUnbondingTime(suite.ctx, 1)
	time2 := suite.childChain.App.(*app.App).ChildKeeper.GetUnbondingTime(suite.ctx, 2)
	suite.Require().Equal(uint64(0), time1, "unbonding time not deleted for mature packet 1")
	suite.Require().Equal(uint64(0), time2, "unbonding time not deleted for mature packet 2")

	// unbonded packets are deleted
	_, err = suite.childChain.App.(*app.App).ChildKeeper.GetUnbondingPacket(suite.ctx, 1)
	suite.Require().Error(err, "retrieved unbonding packet for matured packet 1")
	_, err = suite.childChain.App.(*app.App).ChildKeeper.GetUnbondingPacket(suite.ctx, 2)
	suite.Require().Error(err, "retrieved unbonding packet for matured packet 1")

	expectedWriteAckBytes := channeltypes.CommitAcknowledgement(channeltypes.NewResultAcknowledgement([]byte{byte(1)}).Acknowledgement())

	// successful acknowledgements are written
	ackBytes1, ok := suite.childChain.App.GetIBCKeeper().ChannelKeeper.GetPacketAcknowledgement(suite.ctx, childtypes.PortID, suite.path.EndpointA.ChannelID, 1)
	suite.Require().True(ok)
	suite.Require().Equal(expectedWriteAckBytes, ackBytes1, "did not write successful ack for matue packet 1")
	ackBytes2, ok := suite.childChain.App.GetIBCKeeper().ChannelKeeper.GetPacketAcknowledgement(suite.ctx, childtypes.PortID, suite.path.EndpointA.ChannelID, 2)
	suite.Require().True(ok)
	suite.Require().Equal(expectedWriteAckBytes, ackBytes2, "did not write successful ack for matue packet 1")

	// ensure that third packet did not get ack written and is still in store
	time3 := suite.childChain.App.(*app.App).ChildKeeper.GetUnbondingTime(suite.ctx, 3)
	suite.Require().True(time3 > uint64(suite.ctx.BlockTime().UnixNano()), "Unbonding time for packet 3 is not after current time")
	packet3, err := suite.childChain.App.(*app.App).ChildKeeper.GetUnbondingPacket(suite.ctx, 3)
	suite.Require().NoError(err, "retrieving unbonding packet 3 returned error")
	suite.Require().Equal(&packet, packet3, "unbonding packet 3 has unexpected value")

	// ensure acknowledgement has not been written for unbonding packet
	ackBytes3, ok := suite.childChain.App.GetIBCKeeper().ChannelKeeper.GetPacketAcknowledgement(suite.ctx, childtypes.PortID, suite.path.EndpointA.ChannelID, 3)
	suite.Require().False(ok)
	suite.Require().Nil(ackBytes3, "acknowledgement written for unbonding packet 3")

}
