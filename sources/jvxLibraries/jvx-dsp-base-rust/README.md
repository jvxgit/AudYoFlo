# Known issues
The FFT module is not thread-safe. To run the tests use `cargo test --features=testing -- --test-threads=1`.
Also make sure `AYF_SDK_PATH` is set.
The `testing` feature is only used to be able to detect if a test binary is being built in `build.rs`.
