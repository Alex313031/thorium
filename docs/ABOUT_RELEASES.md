# Explainer for Thorium releases

This document was created to explain __SSE__ and __AVX__, and to guide users on which 
version of Thorium is appropriate to download for their machine.

Many users have been confused and posted issues across my repos on what option they should choose when downloading a release. 
AVX what? SSE huh? What's the difference? Why does one work on my machine but the other doesn't ?

### About SIMD

[Since 1997](https://en.wikipedia.org/wiki/MMX_(instruction_set)), x86 CPUs have been updated with new instructions that they can execute. 
Many of these increase performance because they are Single Instruction, Multiple Data ([SIMD](https://en.wikipedia.org/wiki/Single_instruction,_multiple_data)) instructions. 
The first was called MMX (Multi-Media EXtensions). Then came SSE, SSE2, SSE3, SSE4.1, SSE4.2, AVX, AVX2, and AVX-512, in that order.

More info on SIMD and how the optimizations work in Thorium can be found on the site > https://thorium.rocks/optimizations

However, they have to be built in (compiled in) to a program, and they are backwards, but not forwards compatible. Furthermore, your CPU *must* support a given SIMD 
level or else the browser will crash.

For example, a CPU that is capable of AVX is capable of all the SSE instructions, and so could run either an SSE3 or AVX release, but would get better 
performance from the AVX release. However, this same CPU would *not* be able to run an AVX2 release.

Chromium/Chrome, and projects based on it, have required SSE3 as a minimum [since 2020](https://docs.google.com/document/d/1QUzL4MGNqX4wiLvukUwBf6FdCL35kCDoEJTm2wMkahw/edit#heading=h.7nki9mck5t64), 
however, I try to make tailored/optimized releases, including:

32 bit SSE2 (restored!)  
32 bit SSE3

64 bit SSE3  
64 bit SSE4.1 (sometimes)  
64 bit AVX  
64 bit AVX2

(I stopped making SSE4.2 and AVX-512 builds)

 - CPUs since 2001 including Pentium 4/Athlon and up can run the SSE2 releases.

 - CPUs since 2005 including Pentium D/Opteron and up can run the SSE3 releases.

 - CPUs since 2007 including Core 2 Duo/Phenom and up can run the SSE3 releases.

 - CPUs since 2011 including 2nd Gen Core/Bulldozer and up can run the AVX releases.

 - CPUs since 2016 including 6th Gen Core/Ryzen and up can run the AVX2 releases.

 - Caveat: some lower end CPU's like Intel Atom/Celeron/Pentium or AMD Geode might not have AVX/AVX2.

### So which do I download ?

If you want to find out for sure what your CPU supports, you can use:

Windows > [CPU-Z](https://www.cpuid.com/softwares/cpu-z.html)  
Linux > Use [my script](https://github.com/Alex313031/thorium/blob/main/check_simd.sh) from the repo  
MacOS > Unneeded. Current Macs support AVX.
