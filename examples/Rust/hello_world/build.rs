/*
    Webinix Library 2.0.2
    
    http://webinix.me
    https://github.com/alifcommunity/webinix

    Licensed under GNU General Public License v3.0.
    Copyright (C)2022 Hassan DRAGA <https://github.com/hassandraga>.
*/

fn main() {

    // Path to prebuilt Webinix static library
    println!("cargo:rustc-link-search=../../../build/Windows/MSVC/");

    // Path to Webinix include folder (Optional)
    println!("cargo:rustc-link-search=../../../include/");   
    
    // Webinix static library name
    println!("cargo:rustc-link-lib=webinix-2-static-x64");
}
