## Compiler configs for SSE2, SSE3, SSE4.1, and AVX2

For Polly, run `build_polly.sh` from //infra first, and then use the appropriate *BUILD.gn* for your build type from the Polly folder.

For Android arm64 builds, use the Android folder, including its *android_args.gn* for your args.gn, and the `build_android.sh` from the root of the repo for building the main apk, content shell apk, and system webview apk.
