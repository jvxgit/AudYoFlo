fn main() {
    if std::env::var("CARGO_FEATURE_TESTING").is_ok() {
        // Link the static library
        println!("cargo:rustc-link-lib=static=jvx-dsp-base_static");
        println!("cargo:rustc-link-lib=static=jvx-dsp_static");
        println!("cargo:rustc-link-lib=dylib=fftw3");
        println!("cargo:rustc-link-lib=dylib=fftw3f");

        let ayf_sdk_path = std::env::var("AYF_SDK_PATH").expect("AYF_SDK_PATH is not set");
        println!("cargo:rustc-link-search=native={}/lib/", ayf_sdk_path);
    }
}
