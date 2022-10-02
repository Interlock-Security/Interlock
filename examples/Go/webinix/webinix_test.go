package webinix

import "testing"

func TestWebui(t *testing.T) {
	want := 0
	if got := webinix_test(); got != want {
		t.Errorf("webinix_test() = %q, want %q", got, want)
	}
}
