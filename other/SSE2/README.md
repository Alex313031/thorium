# Thorium SSE2

This directory contains build config files for compiling 32 bit Thorium/Chromium with [SSE2](https://en.wikipedia.org/wiki/SSE2).

Chromium doesn't officially support 32 bit Linux anymore, and "requires" SSE3. With some workarounds though, it can be made to work.  
Using the `-msse2` compiler flag is not enough, one must also add the `-mmmx` and `-mfxsr` flags.
