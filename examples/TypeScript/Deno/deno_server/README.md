### Deno Server

This example shows how to create a web server using Deno and run a simple system command to run a basic Webinix application to open a window.

 1. Download and [Install Deno](https://github.com/denoland/deno/releases) (*Or just copy deno binary file into this folder*)
 2. Build Webinix Library (*[instructions](https://github.com/alifcommunity/webinix/tree/main/build)*)
 3. Build `window.c` using any C compiler (*[instructions](https://github.com/alifcommunity/webinix/tree/main/examples/C)*)
 3. Run `deno run --allow-all server.ts`

Folder structure example (*Windows*)

    [My Folder]
	    * deno.exe (if not installed)
	    * window.exe
	    * server.ts
	    * file1.js
	    * file2.ts
	    * ...
