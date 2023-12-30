# Explainer for Thorium releases

 &ndash; This document was created to explain SSE and AVX, and to guide users on which 
   version of Thorium is appropriate to download for your machine.

Many users have been confused and posted issues across my repos on what option they should choose when downloading a release. 
AVX what? SSE huh? Whats the difference? Why does one work on my machine but the other doesn't?

### About SIMD

Since 1997, x86 based CPUs have gotten new instructions that they can execute. Many of these
increase performance because they are Single Instruction, Multiple Data instructions. The first was called 
MMX (Multi-Media EXtensions). Then came SSE, SSE2, SSE3, SSE4.1, SSE4.2, AVX, AVX2, and AVX-512, in that order.

However, they have to be built in (compiled in), and they are backwards, but not forwards compatible, and your CPU *must* support a given SIMD 
level or else the browser will crash.

For example, a CPU that is capable of AVX is capable of all the SSE instrucitons, and so can run either the SSE3 or AVX release, but will get better 
performance from the AVX release. However, this same CPU would *not* be able to run an AVX2 release.

Chromium/Chrome, and projects based on it, have required SSE3 as a minimum since 2020, however, I try to make tailored releases, including:

32 bit SSE2 (restored!)
32 bit SSE3
64 bit SSE3
64 bit SSE4.1 (sometimes)
64 bit AVX
64 bit AVX2

CPUs since 2001 including Pentium 4/Athlon and up can run the SSE2 releases.

CPUs since 2005 including Pentium D/Opteron and up can run the SSE3 releases.

CPUs since 2007 including Core 2 Duo/Phenom and up can run the SSE3 releases.

CPUs since 2011 including 2nd Gen Core/Bulldozer and up can run the AVX releases.

CPUs since 2016 including 6th Gen Core/Ryzen and up can run the AVX2 releases.

 - Caveat, some lower end CPU's like Intel Atom/Celeron/Pentium or AMD Geode might not have AVX/AVX2.

### So which do I download ?

If you want to find out for sure what your CPU supports, you can use:

Windows > CPU-Z
Linux > 

