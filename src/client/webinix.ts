type B64string = string

class WebUiClient {
	//webinix settings
	//@ts-ignore injected by webinix.c
	#port: number = _webinix_port
	//@ts-ignore injected by webinix.c
	#winNum: number = _webinix_win_num
	//@ts-ignore injected by webinix.c
	#log = _webinix_log ?? false //If webinix.c define _webinix_log then use it, instead set it to false
	
    #ws: WebSocket
	#wsStatus = false
	#wsStatusOnce = false
	#closeReason = 0
	#closeValue: string
	#hasEvents = false
	#fnId = 1
	#fnPromiseResolve: (((data: string) => unknown) | undefined)[] = []

	#bindList: unknown[] = []

	//webinix const
	#HEADER_SIGNATURE = 221
	#HEADER_JS = 254
	#HEADER_JS_QUICK = 253
	#HEADER_CLICK = 252
	#HEADER_SWITCH = 251
	#HEADER_CLOSE = 250
	#HEADER_CALL_FUNC = 249

	constructor() {
		if (!('WebSocket' in window)) {
			alert('Sorry. WebSocket not supported by your Browser.')
			if (!this.#log) globalThis.close()
		}

		this.#start()

		if ('navigation' in globalThis) {
			globalThis.navigation.addEventListener('navigate', (event) => {
				const url = new URL(event.destination.url)
				this.#sendEventNavigation(url.href)
			})
		} else {
			console.error(
				'navigation API not supported, some features may be missing'
			)
		}

		// -- DOM ---------------------------
		document.addEventListener('keydown', (event) => {
			// Disable F5
			if (this.#log) return
			if (event.key === 'F5') event.preventDefault()
		})
		document.addEventListener('click', (event) => {
			const anchor = (event.target as HTMLElement).closest('a')
			if (anchor) {
				const link = anchor.href
				if (this.#isExternalLink(link)) {
					event.preventDefault()
					//TODO fic webinix.close declaration
					this.#close(this.#HEADER_SWITCH) //, link)
				}
			}
		})

		// -- Links -------------------------
		onbeforeunload = () => {
			this.#close()
		}
		setTimeout(() => {
			if (!this.#wsStatusOnce) {
				this.#freezeUi()
				alert(
					'Webinix failed to connect to the background application. Please try again.'
				)
				if (!webinix.log) globalThis.close()
			}
		}, 1500)

		addEventListener('load', () => {
			document.body.addEventListener('contextmenu', function (event) {
				event.preventDefault()
			})
		})
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

		this.#ws.onmessage = (event) => {
			const buffer8 = new Uint8Array(event.data)
			if (buffer8.length < 4) return
			if (buffer8[0] !== this.#HEADER_SIGNATURE) return
			const data8 =
				buffer8[buffer8.length - 1] === 0
					? buffer8.slice(3, -1)
					: buffer8.slice(3) // Null byte (0x00) can break eval()
			const data8utf8 = new TextDecoder().decode(data8)

			// Process Command
			switch (buffer8[1]) {
				case this.#HEADER_CALL_FUNC:
					{
						const callId = buffer8[2]
						if (this.#log) {
							console.log(`Webinix -> Func Reponse [${data8utf8}]`)
						}
						if (this.#fnPromiseResolve[callId]) {
							if (this.#log) {
								console.log(
									`Webinix -> Resolving reponse #${callId}...`
								)
							}
							this.#fnPromiseResolve[callId]?.(data8utf8)
							this.#fnPromiseResolve[callId] = undefined
						}
					}
					break
				case this.#HEADER_SWITCH:
					this.#close(this.#HEADER_SWITCH, data8utf8)
					break
				case this.#HEADER_CLOSE:
					globalThis.close()
					break
				case this.#HEADER_JS_QUICK:
				case this.#HEADER_JS:
					{
						const data8utf8sanitize = data8utf8.replace(
							/(?:\r\n|\r|\n)/g,
							'\n'
						)
						if (this.#log)
							console.log(`Webinix -> JS [${data8utf8sanitize}]`)

						// Get callback result
						let FunReturn = 'undefined'
						let FunError = false
						try {
							FunReturn = eval(`(() => {${data8utf8sanitize}})()`)
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

						// Format ws response
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
			const packet =
				elem !== ''
					? Uint8Array.of(
							this.#HEADER_SIGNATURE,
							this.#HEADER_CLICK,
							0,
							...new TextEncoder().encode(elem)
					  )
					: Uint8Array.of(
							this.#HEADER_SIGNATURE,
							this.#HEADER_CLICK,
							0,
							0
					  )
			this.#ws.send(packet.buffer)
			if (this.#log) console.log(`Webinix -> Click [${elem}]`)
		}
	}
	#sendEventNavigation(url: string) {
		if (this.#hasEvents && this.#wsStatus && url !== '') {
			const packet = Uint8Array.of(
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
	#fnPromise(fn: string, value: string) {
		if (this.#log) console.log(`Webinix -> Func [${fn}](${value})`)
		const callId = this.#fnId++

		const packet = Uint8Array.of(
			this.#HEADER_SIGNATURE,
			this.#HEADER_CALL_FUNC,
			callId,
			...new TextEncoder().encode(fn),
			0,
			...new TextEncoder().encode(value),
			0
		)

		return new Promise((resolve) => {
			this.#fnPromiseResolve[callId] = resolve
			this.#ws.send(packet.buffer)
		})
	}

	// -- APIs --------------------------

	/**
	 * Call a backend function from the frontend.
	 * @param fn - Backend bind name.
	 * @param value - Payload to send.
	 * @return - Response of the backend callback.
	 * @example
	 * ```c
	 * //Backend (C)
	 * webinix_bind(window, "get_cwd", get_current_working_directory);
	 * ```
	 * ```js
	 * //Frontend (JS)
	 * const cwd = await webinix_fn("get_cwd");
	 * ```
	 * @example
	 * ```c
	 * //Backend (C)
	 * webinix_bind(window, "write_file", write_file);
	 * ```
	 * ```js
	 * //Frontend (JS)
	 * webinix_fn("write_file", "content to write")
	 *  .then(() => console.log("file writed"))
	 *  .catch(() => console.error("can't write the file"))
	 * ```
	 */
	fn(fn: string, value?: string): Promise<string | void> {
		if (!fn || !this.#wsStatus) return Promise.resolve()
		if (typeof value === 'undefined') value = ''
		if (
			!this.#hasEvents &&
			!this.#bindList.includes(this.#winNum + '/' + fn)
		)
			return Promise.resolve()
		return this.#fnPromise(fn, value) as Promise<string | void>
	}

	/**
	 * Active or deactivate webinix debug logging.
	 * @param status - log status to set.
	 */
	log(status: boolean) {
		if (status) {
			console.log('Webinix -> Log Enabled.')
			this.#log = true
		} else {
			console.log('Webinix -> Log Disabled.')
			this.#log = false
		}
	}

	/**
	 * Encode a string into base64.
	 * @param str - string to encode.
	 */
	encode(str: string): B64string {
		return btoa(str)
	}

	/**
	 * Decode a base64 string.
	 * @param str - base64 string to decode.
	 */
	decode(str: B64string): string {
		return atob(str)
	}
}

export const webinix = new WebUiClient()
//@ts-ignore globally expose webinix APIs
globalThis.webinix = webinix

const inputs = document.getElementsByTagName('input')

for (let i = 0; i < inputs.length; i++) {
	inputs[i].addEventListener('contextmenu', function (event) {
		event.stopPropagation()
	})
}
