

fn main() {

    // Path to prebuilt Webinix static library
    println!("cargo:rustc-link-search=../../../build/Windows/MSVC/");

    // Path to Webinix include folder (Optional)
    println!("cargo:rustc-link-search=../../../include/");   
    
    // Webinix static library name
    println!("cargo:rustc-link-lib=webinix-2-static-x64");
    println!("cargo:rustc-link-lib=user32");
}
