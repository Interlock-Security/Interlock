

// Run this script by:
// deno run --allow-all --unstable deno_server.ts

// Import Webinix module
import * as webinix from "../module/webinix.ts";

// Import Deno Server Module
import { serve } from "https://deno.land/std@0.158.0/http/server.ts";

// Optional - Set a custom library path:
//  const lib_full_path = '../../../../build/Windows/MSVC/webinix-2-x64.dll';
//  console.log("Optional: Looking for the Webinix dynamic library at: " + lib_full_path);
//  webinix.set_lib_path(lib_full_path);

// Deno Server Listener
const port = 8080;
const url = "http://localhost:" + 8080;
const handler = (request: Request): Response => {
  const body = `This is a Deno-Web-Server example. Your user-agent is:\n\n${
    request.headers.get("user-agent") ?? "Unknown"
  }`;
  return new Response(body, { status: 200 });
};
serve(handler, { port });

// Create new window
const my_window = webinix.new_window();

// Show the window
if(!webinix.open(my_window, url, webinix.browser.chrome))
	webinix.open(my_window, url, webinix.browser.any);

// Wait until all windows get closed
await webinix.wait();

// The window is closed.
console.log('Thank you.');
Deno.exit(0);
