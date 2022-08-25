## Compiler configs for AVX2 and SSE4

To build for AVX2, copy the `build` directory in the AVX2 folder over `//chromium/src/build`.

For SSE4, do the same except from the SSE4 folder.

For Polly, run `build_polly.sh` first, and then use the appropriate *BUILD.gn* for your build type from the Polly folder.

For Android arm64 builds, use the Android folder, including its `android_args.gn` for your args.gn
