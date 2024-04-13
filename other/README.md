## Compiler configs for SSE2, SSE3, SSE4.1, and AVX2

This directory contains compiler configurations files and args.gn files for compiling Thorium with:

__SSE2__ > See the [the Wiki page](https://en.wikipedia.org/wiki/SSE2) about it. (For very old processors)  
__SSE3__ > See the [the Wiki page](https://en.wikipedia.org/wiki/SSE3) about it. (This is the normal Chromium baseline default)  
__SSE4.1__ > See the [the Wiki page](https://en.wikipedia.org/wiki/SSE4#SSE4.1) about it. (SSE4.2 is not used, as only 1st Gen Core supports 4.2 but not AVX)  
 and  
__AVX2__ > See the [the Wiki page](https://en.wikipedia.org/wiki/Advanced_Vector_Extensions#Advanced_Vector_Extensions_2) about it. (For modern CPUs)

It also contains config files for MacOS and ChromiumOS/ThoriumOS.

([AVX](https://en.wikipedia.org/wiki/Advanced_Vector_Extensions) is the normal Thorium baseline default)

### Other info

For [Polly](https://polly.llvm.org/) builds, run `build_polly.sh` from [//infra](../infra) first, and then uncomment the `use_polly = true` arg in your args.gn.

For Android or Raspberry Pi builds, see the [//arm](../arm) directory.
