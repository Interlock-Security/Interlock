export type B64string = string;

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
export function webinix_fn(fn: string, value?: string): Promise<string | undefined>; 

/**
 * Active or deactivate webinix debug logging.
 * @param status - log status to set.
 */
export function webinix_log(status: boolean): void;

/**
 * Encode a string into base64.
 * @param str - string to encode.
 */
export function webinix_encode(str: string): B64string;

/**
 * Decode a base64 string.
 * @param str - base64 string to decode.
 */
export function webinix_decode(str: B64string): string;
