'use-strict' // Force strict mode for transpiled

/*
  Webinix Bridge
  
  http://webinix.me
  https://github.com/webinix-dev/webinix
  Copyright (c) 2020-2023 Hassan Draga.
  Licensed under MIT License.
  All rights reserved.
  Canada.

  Converted from JavaScript to TypeScript
  By Oculi Julien. Copyright (c) 2023.
*/

//@ts-ignore use *.ts import real extension
import { AsyncFunction, addRefreshableEventListener } from './utils.ts'

type B64string = string
type JSONValue =
	| string
	| number
	| boolean
	| { [x: string]: JSONValue | undefined }
	| JSONValue[]
	| Uint8Array

class WebuiBridge {
	// Webinix settings
	#port: number
	#winNum: number
	#bindList: unknown[] = []
	#log: boolean
	#winX: number
	#winY: number
	#winW: number
	#winH: number

	// Internals
	#ws: WebSocket
	#wsStatus = false
	#wsStatusOnce = false
	#closeReason = 0
	#closeValue: string
	#hasEvents = false
	#fnId = 1
	#fnPromiseResolve: (((data: string) => unknown) | undefined)[] = []

	// Webinix const
	#HEADER_SIGNATURE = 221
	#HEADER_JS = 254
	#HEADER_JS_QUICK = 253
	#HEADER_CLICK = 252
	#HEADER_SWITCH = 251
	#HEADER_CLOSE = 250
	#HEADER_CALL_FUNC = 249
	#HEADER_SEND_RAW = 248
	#HEADER_NEW_ID = 247

	constructor({
		port,
		winNum,
		bindList,
		log = false,
		winX,
		winY,
		winW,
		winH,
	}: {
		port: number
		winNum: number
		bindList: unknown[]
		log?: boolean
		winX: number
		winY: number
		winW: number
		winH: number
	}) {
		// Constructor arguments are injected by webinix.c
		this.#port = port
		this.#winNum = winNum
		this.#bindList = bindList
		this.#log = log
		this.#winX = winX
		this.#winY = winY
		this.#winW = winW
		this.#winH = winH

		// Instance
		if ('webinix' in globalThis) {
			throw new Error(
				'Sorry. Webinix is already defined, only one instance is allowed.'
			)
		}

		// Positioning the current window
		if (this.#winX !== undefined && this.#winY !== undefined) {
			window.moveTo(this.#winX, this.#winY)
		}

		// Resize the current window
		if (this.#winW !== undefined && this.#winH !== undefined) {
			window.resizeTo(this.#winW, this.#winH)
		}

		// WebSocket
		if (!('WebSocket' in window)) {
			alert('Sorry. WebSocket is not supported by your web browser.')
			if (!this.#log) globalThis.close()
		}

		// Connect to the backend application
		this.#start()

		// Handle navigation server side
		if ('navigation' in globalThis) {
			globalThis.navigation.addEventListener('navigate', (event) => {
				const url = new URL(event.destination.url)
				this.#sendEventNavigation(url.href)
			})
		} else {
			// Handle all link click to prevent natural navigation
			// Rebind listener if user inject new html
			addRefreshableEventListener(
				document.body,
				'a',
				'click',
				(event) => {
					event.preventDefault()
					const { href } = event.target as HTMLAnchorElement
					if (this.#isExternalLink(href)) {
						this.#close(this.#HEADER_SWITCH, href)
					} else {
						this.#sendEventNavigation(href)
					}
				}
			)
		}

		// Prevent F5 refresh
		document.addEventListener('keydown', (event) => {
			// Disable F5
			if (this.#log) return
			if (event.key === 'F5') event.preventDefault()
		})

		onbeforeunload = () => {
			this.#close()
		}

		setTimeout(() => {
			if (!this.#wsStatusOnce) {
				this.#freezeUi()
				alert(
					'Sorry. Webinix failed to connect to the backend application. Please try again.'
				)
				if (!this.#log) globalThis.close()
			}
		}, 1500)
	}

	#close(reason = 0, value = '') {
		if (reason === this.#HEADER_SWITCH) this.#sendEventNavigation(value)
		this.#wsStatus = false
		this.#closeReason = reason
		this.#closeValue = value
		this.#ws.close()
	}

	#freezeUi() {
		document.body.style.filter = 'contrast(1%)'
	}

	#isTextBasedCommand(cmd: number): Boolean {
		if(cmd !== this.#HEADER_SEND_RAW)
			return true;
		return false;
	}

	#getDataStrFromPacket(buffer: Uint8Array, startIndex: number): string {
		let stringBytes: number[] = [];
		for (let i = startIndex; i < buffer.length; i++) {
			if (buffer[i] === 0) { // Check for null byte
				break;
			}
			stringBytes.push(buffer[i]);
		}
	
		// Convert the array of bytes to a string
		const stringText = new TextDecoder().decode(new Uint8Array(stringBytes));
		return stringText;
	}

	#start() {
		if (this.#bindList.includes(this.#winNum + '/')) {
			this.#hasEvents = true
		}

		this.#ws = new WebSocket(
			`ws://localhost:${this.#port}/_webinix_ws_connect`
		)
		this.#ws.binaryType = 'arraybuffer'

		this.#ws.onopen = () => {
			this.#wsStatus = true
			this.#wsStatusOnce = true
			this.#fnId = 1
			if (this.#log) console.log('Webinix -> Connected')
			this.#clicksListener()
		}

		this.#ws.onerror = () => {
			if (this.#log) console.log('Webinix -> Connection Failed')
			this.#freezeUi()
		}

		this.#ws.onclose = (event) => {
			this.#wsStatus = false
			if (this.#closeReason === this.#HEADER_SWITCH) {
				if (this.#log) {
					console.log(
						`Webinix -> Connection lost -> Navigation to [${
							this.#closeValue
						}]`
					)
				}
				globalThis.location.replace(this.#closeValue)
			} else {
				if (this.#log) {
					console.log(`Webinix -> Connection lost (${event.code})`)
					this.#freezeUi()
				} else {
					this.#closeWindowTimer()
				}
			}
		}

		this.#ws.onmessage = async (event) => {
			const buffer8 = new Uint8Array(event.data)
			if (buffer8.length < 2) return
			if (buffer8[0] !== this.#HEADER_SIGNATURE) return

			if(this.#isTextBasedCommand(buffer8[1])) {
				// UTF8 Text based commands

				// Process Command
				switch (buffer8[1]) {
					case this.#HEADER_CALL_FUNC:
						{
							// Command Packet
							// 0: [Signature]
							// 1: [CMD]
							// 2: [Call ID]
							// 3: [Call Response]

							const callResponse = this.#getDataStrFromPacket(buffer8, 3)
							const callId = buffer8[2]

							if (this.#log) {
								console.log(`Webinix -> Call Response [${callResponse}]`)
							}
							if (this.#fnPromiseResolve[callId]) {
								if (this.#log) {
									console.log(
										`Webinix -> Resolving Response #${callId}...`
									)
								}
								this.#fnPromiseResolve[callId]?.(callResponse)
								this.#fnPromiseResolve[callId] = undefined
							}
						}
						break
					case this.#HEADER_SWITCH:
						// Command Packet
						// 0: [Signature]
						// 1: [CMD]
						// 2: [URL]

						const url = this.#getDataStrFromPacket(buffer8, 2)
						console.log(`Webinix -> Switch [${url}]`)
						this.#close(this.#HEADER_SWITCH, url)
						break
					case this.#HEADER_NEW_ID:
						// Command Packet
						// 0: [Signature]
						// 1: [CMD]
						// 2: [New Element]

						const newElement = this.#getDataStrFromPacket(buffer8, 2)
						console.log(`Webinix -> New Bind ID [${newElement}]`)
						if(!this.#bindList.includes(newElement))
							this.#bindList.push(newElement)
						break
					case this.#HEADER_JS_QUICK:
					case this.#HEADER_JS:
						{
							// Command Packet
							// 0: [Signature]
							// 1: [CMD]
							// 2: [ID]
							// 3: [Script]

							const script = this.#getDataStrFromPacket(buffer8, 3)
							const scriptSanitize = script.replace(
								/(?:\r\n|\r|\n)/g,
								'\n'
							)

							if (this.#log)
								console.log(`Webinix -> JS [${scriptSanitize}]`)

							// Get callback result
							let FunReturn = 'undefined'
							let FunError = false
							try {
								FunReturn = await AsyncFunction(scriptSanitize)()
							} catch (e) {
								FunError = true
								FunReturn = e.message
							}
							if (buffer8[1] === this.#HEADER_JS_QUICK) return
							if (FunReturn === undefined) {
								FunReturn = 'undefined'
							}

							// Logging
							if (this.#log && !FunError)
								console.log(`Webinix -> JS -> Return [${FunReturn}]`)
							if (this.#log && FunError)
								console.log(`Webinix -> JS -> Error [${FunReturn}]`)

							// Response Packet
							// 0: [Signature]
							// 1: [CMD]
							// 2: [ID]
							// 3: [Error]
							// 4: [Script Response]
							const Return8 = Uint8Array.of(
								this.#HEADER_SIGNATURE,
								this.#HEADER_JS,
								buffer8[2],
								FunError ? 0 : 1,
								...new TextEncoder().encode(FunReturn)
							)

							if (this.#wsStatus) this.#ws.send(Return8.buffer)
						}
						break
					case this.#HEADER_CLOSE:
						// Command Packet
						// 0: [Signature]
						// 1: [CMD]

						if (!this.#log)
							globalThis.close()
						else {
							console.log(`Webinix -> Close`)
							this.#ws.close()
						}
						break
				}
			}
			else {
				// Raw binary commands

				// Process Command
				switch (buffer8[1]) {
					case this.#HEADER_SEND_RAW:
						{
							// Command Packet
							// 0: [Signature]
							// 1: [CMD]
							// 2: [Function]
							// 3: [Null]
							// 4: [Raw Data]

							// Get function name
							const functionName: string = this.#getDataStrFromPacket(buffer8, 2)
							
							// Get the raw data
							const rawDataIndex: number = 2 + functionName.length + 1
							const userRawData = buffer8.subarray(rawDataIndex);
							
							// Call the user function, and pass the raw data
							if (typeof window[functionName] === 'function')
								window[functionName](userRawData);
							else
								await AsyncFunction(functionName + '(userRawData);')()
						}
						break
				}
			}
		}
	}

	#clicksListener() {
		Object.keys(window).forEach((key) => {
			if (/^on(click)/.test(key)) {
				globalThis.addEventListener(key.slice(2), (event) => {
					if (!(event.target instanceof HTMLElement)) return
					if (
						this.#hasEvents ||
						(event.target.id !== '' &&
							this.#bindList.includes(
								this.#winNum + '/' + event.target?.id
							))
					) {
						this.#sendClick(event.target.id)
					}
				})
			}
		})
	}

	#sendClick(elem: string) {
		if (this.#wsStatus) {
			// Response Packet
			// 0: [Signature]
			// 1: [CMD]
			// 2: [Element]
			const packet =
				elem !== ''
					? Uint8Array.of(
							this.#HEADER_SIGNATURE,
							this.#HEADER_CLICK,
							...new TextEncoder().encode(elem)
					  )
					: Uint8Array.of(
							this.#HEADER_SIGNATURE,
							this.#HEADER_CLICK,
							0
					  )
			this.#ws.send(packet.buffer)
			if (this.#log) console.log(`Webinix -> Click [${elem}]`)
		}
	}

	#sendEventNavigation(url: string) {
		if (this.#hasEvents && this.#wsStatus && url !== '') {
			const packet = Uint8Array.of(
				// Response Packet
				// 0: [Signature]
				// 1: [CMD]
				// 2: [URL]
				this.#HEADER_SIGNATURE,
				this.#HEADER_SWITCH,
				...new TextEncoder().encode(url)
			)
			this.#ws.send(packet.buffer)
			if (this.#log) console.log(`Webinix -> Navigation [${url}]`)
		}
	}

	#isExternalLink(url: string) {
		return new URL(url).host === globalThis.location.host
	}
	#closeWindowTimer() {
		setTimeout(function () {
			globalThis.close()
		}, 1000)
	}
	#fnPromise(fn: string, value: any) {
		if (this.#log) console.log(`Webinix -> Call [${fn}](${value})`)
		const callId = this.#fnId++

		// Response Packet
		// 0: [Signature]
        // 1: [CMD]
        // 2: [Call ID]
        // 3: [Element ID, Null, Len, Null, Data, Null]

		const packet = Uint8Array.of(
			this.#HEADER_SIGNATURE,
			this.#HEADER_CALL_FUNC,
			callId,
			...new TextEncoder().encode(fn),
			0,
			...new TextEncoder().encode(String(value.length)),
			0,
			...typeof value === 'object' ? value : new TextEncoder().encode(value),
			0
		)

		return new Promise((resolve) => {
			this.#fnPromiseResolve[callId] = resolve
			this.#ws.send(packet.buffer)
		})
	}

	// -- APIs --------------------------

	/**
	 * Call a backend binding from the frontend.
	 * @param bindingName - Backend bind name.
	 * @param payload - Payload to send to the binding, accept any JSON valid data
	 * (string, number, array, object, boolean, undefined).
	 * @return - Response of the backend callback as JSON compatible value.
	 * @example
	 * __Backend (C)__
	 * ```c
	 * webinix_bind(window, "get_cwd", get_current_working_directory);
	 * ```
	 * __Frontend (JS)__
	 * ```js
	 * const cwd = await webinix.call("get_cwd");
	 * ```
	 * @example
	 * __Backend (C)__
	 * ```c
	 * webinix_bind(window, "write_file", write_file);
	 * ```
	 * __Frontend (JS)__
	 * ```js
	 * webinix.call("write_file", "content to write")
	 *  .then(() => console.log("Success"))
	 *  .catch(() => console.error("Can't write the file"))
	 * ```
	 * @example
	 * __Backend (C)__
	 * ```c
	 * //complex_datas() returns the following JSON string
	 * //'{ "count": 1, "list": [ 1, 2, 3 ], "isGood": true }'
	 * webinix_bind(window, "complex_datas", complex_datas);
	 * ```
	 * __Frontend (JS)__
	 * ```js
	 * type ComplexDatas = {
	 *  count: number;
	 *  list: number[];
	 *  isGood: boolean;
	 * }
	 *
	 * const { count, list, isGood } = await webinix
	 *  .call<ComplexDatas>("complex_datas");
	 *
	 * //count = 1;
	 * //list = [ 1, 2, 3 ];
	 * //isGood = true;
	 * ```
	 */
	async call<
		Response extends JSONValue = string,
		Payload extends JSONValue = JSONValue
	>(bindingName: string, payload?: Payload): Promise<Response | void> {
		if (!bindingName)
			return Promise.reject(new SyntaxError('No binding name is provided'))

		if (!this.#wsStatus)
			return Promise.reject(new Error('WebSocket is not connected'))
		// Check binding list
		if (
			!this.#hasEvents &&
			!this.#bindList.includes(`${this.#winNum}/${bindingName}`)
		)
			return Promise.reject(
				new ReferenceError(`No binding was found for "${bindingName}"`)
			)

		// Get the binding response
		const response = (await this.#fnPromise(
			bindingName,
			payload === undefined
				? ''
				: typeof payload === 'string'	// String()
				? payload
				: typeof payload === 'object'	// Uint8Array()
				? payload
				: JSON.stringify(payload)		// JSON String
		)) as string | void

		// Handle response type (void, string or JSON value)
		if (response === undefined) return

		try {
			return JSON.parse(response)
		} catch {
			//@ts-ignore string is a valid JSON value
			return response
		}
	}

	/**
	 * Active or deactivate webinix debug logging.
	 * @param status - log status to set.
	 */
	setLogging(status: boolean) {
		if (status) {
			console.log('Webinix -> Log Enabled.')
			this.#log = true
		} else {
			console.log('Webinix -> Log Disabled.')
			this.#log = false
		}
	}

	/**
	 * Encode datas into base64 string.
	 * @param datas - string or JSON value.
	 */
	encode(datas: JSONValue): B64string {
		if (typeof datas === 'string') return btoa(datas)
		return btoa(JSON.stringify(datas))
	}

	/**
	 * Decode a base64 string into any JSON valid format
	 * (string, number, array, object, boolean).
	 * @param b64 - base64 string to decode.
	 */
	decode<T extends JSONValue>(b64: B64string): T {
		try {
			return JSON.parse(atob(b64))
		} catch {
			//@ts-ignore string a valid JSON value
			return atob(b64)
		}
	}
}

type webinix = WebuiBridge
export default webinix
export type { WebuiBridge }

// Wait for the html to be parsed
addEventListener('load', () => {
	document.body.addEventListener('contextmenu', (event) =>
		event.preventDefault()
	)
	addRefreshableEventListener(
		document.body,
		'input',
		'contextmenu',
		(event) => event.stopPropagation()
	)
})
