## Thorium FAQ <img src="https://github.com/Alex313031/thorium/blob/main/logos/STAGING/faq.png" width="48">

 - Commonly asked questions and answers to common GitHub issues that keep getting filed.

__<a id="1"></a>1. Q: Why is the Android version not letting me sign in to Google Sync?__  
 __A:__ Even though the Android versions are built with API Keys needed to sign-in, Google has placed extra restrictions on Android Chromium
 forks that prevent sign in unless the browser is on the Google Play Store and has been whitelisted by them (which costs money). The keys
 are still included to allow location services, but there is no workaround for Google Sync at this time. This is also the reason why on some
 devices, opening Thorium will cause a notification saying "Thorium won't run without Google Play Services, which are not supported by your
 device". This is baloney, and it will run just fine.

__<a id="2"></a>2. Q: Why are Netflix, Spotify, Twitch, etc. not playing?__  
 __A:__ Thorium includes Widevine, which is a proprietary component used to play encrypted media streams on sites like these. It is used to prevent
 piracy by preventing downloading these videos in any format that would be readable by an external application. However, there are various "security
 levels" for Widevine. There is L3 "software secure", which is the least secure. It is used on Linux. Then there is L2 "VMP secure", which is what
 Windows and MacOS uses. Then there is L1 "hardware secure", which is only used on specific Windows, MacOS, and ChromeOS devices that are very new
 and have a hardware decoding chip for H.264 or H.265 (HEVC) videos that supports integration with Widevine to prevent tampering or circumvention
 at the hardware level. Anyway, VMP Secure means that the application has been signed by widevine.org. On Windows with Google Chrome or Firefox, this
 can be seen by the fact that next to widevine.dll, there is a widevine.sig, and next to the main program .exes, there is a corresponding .sig file.
 For example firefox.sig or chrome.sig. While Widevine includes the widevine.sig file always, and is publicly available, this only contains the public
 key of the widevine binary. Signing with Google's VMP service sends a hash of the main executable to their servers, it is signed with their private key
 and then the results are encrypted and sent back as a .sig file with the name matching the executable that was sent to them. Using this service costs
 a prohibitive amount of money for a FOSS project (1,000's of dollars). On Linux, Widevine detects that the platform is Linux, and sends a response header
 to sites which then allows playback even though it is only software secure. But usually, sites will limit the resolution to 480p (i.e. SD), including Netflix.
 This has been a long standing problem/complaint in the Linux community. For Windows and MacOS, things are more complicated. While Widevine will work, since
 it is not VMP signed, it will only send an L3 header. Some sites will accept this on Windows (like Hulu), but some sites require VMP when it detects that it
 is on Windows or MacOS. Netflix used to not restrict this, but as of 2023, it will refuse to play without VMP. There is no way to spoof this by changing the
 User Agent to report Linux. Widevine detects the OS and sends it as part of the header to the website. As a side note, L1 is used to allow 4K or 8K playback
 on some sites that support it (like Netflix). Many streaming devices have a hardware chip like what was described above, and hence why they can play this
 high resolution content. So, the end result is that Thorium will play Widevine encrypted content on Linux, but at lower quality, and on Windows or MacOS
 it is highly dependent on the specific site. Your mileage may vary, and unless someone wants to send $7,000 on top of a yearly fee to be part of widevine.org's
 registered applications, Thorium will be restricted in this way (as are most FOSS Chromium and Firefox forks).

__<a id="3"></a>3. Q: Why is Thorium always behind Chromium/Chrome as far as the major release number?__  
 __A:__ Chromium/Chrome used to be on a 6 week release schedule, but then they moved to a 4 week release schedule. Rebasing Thorium is a long, hard, convoluted process
 that requires attention to detail and manual merging of files. I sometimes make mistakes that require correction before a release can be made. Very often, rebasing
 takes 8+ hours of focused, non-stop work. I do this for free (well I do get donations, that usually amount to ~$30 each month, but I would do it even without the
 donations. Donations are always welcome and appreciated), and it is not something I like doing more than necessary. Then comes actually building for all the platforms.
 Building Thorium takes alot of resources. The Chromium repository is 100+ GB in size, and compiling requires 8GB of free RAM or more. Compiling Thorium for Linux on
 my overclocked Ryzen 7 3800XT with 32GB RAM takes 5 hours. @gz83 helps with the Windows builds, and @midzer does the MacOS builds on his arm64 M1 Mac. Remember that
 we must build a total of 13 times to cover all the platforms and architectures that we support. This is a 3 person team, with me doing most of the coding work, feature
 additions, and rebasing. When Chromium makes a new major version, security fixes and bug fixes are backported from this new version into the previous version. This is why,
 for example, 122.0.6261.171 was released after 123.0.6312.133. This means that I always rebase against the previous major version, but use the latest minor version point
 releases. The result is a browser that is one major version behind, but very stable and usually as secure as the current major version. People keep getting super
 worried that because they are using a browser that is one major version behind, that it is super insecure and they are immediately going to become a magnet attracting
 all sorts of malware and viruses. This simply isn't the case. The amount of people who post issues, discussions, and personal emails at me is getting annoying. It is like
 a child in the back seat asking "Are we there yet?" over and over. Asking a billion times is not going to speed up the process at all, and just leads to frustration on my part
 and a cluttering of the GitHub issues and discussions that are better filled with actual bugs. Of course, if there is ever a pertinent security flaw (like the WebP vulnerability
 or something privately relayed to me as per the [SECURITY.md](https://github.com/Alex313031/thorium/blob/main/SECURITY.md) file), I try my best to fix/update it and get
 releases out as soon as possible. Another thing that people keep
 suggesting is to use some kind of CI build service. This wouldn't help with the rebasing/development time, and because of the amount of resources needed to compile Thorium,
 there are no services that I am aware of that have a free tier with hardware capable of doing the compilation. Again, this comes down to money and is prohibitively expensive.

__<a id="4"></a>4. Q: Why isn't Thorium UnGoogled?__  
 __A:__ Thorium integrates many patches from UnGoogled Chromium (see [PATCHES.md](https://github.com/Alex313031/thorium/blob/main/docs/PATCHES.md)), but one of the project
 goals, as outlined towards the top of the Readme, is to enhance usability.
 A majority of people use Google search, and prefer to have Google Sync enabled to allow seamless synchronization of their bookmarks, history, and preferences across multiple
 instances of multiple Chromium-based browsers. Thus, while Thorium has removed telemetry and field trials, there are instances where Thorium still connects to Google servers, even
 if Google Sync is not enabled. A common example is for location services. However, the user has to explicitly consent for most of these, and the data that is still exchanged with
 Google without any user interaction is minimal, and contains no personally identifiable information (PII) that could be used to identify you. Thorium is open to taking requests for
 integration of patches that tweak the UI, enhance privacy or usability, or reduce the amount of data sent to Google. However, it will never be completely de-googled, and specifically
 removing Google Sync, Google search, or disallowing connections to any Google-owned domains is a no-go. If you desire this kind of functionality, I recommend using UnGoogled Chromium.
 If you feel that you are missing out since UnGoogled Chromium does not have the performance-enhancing compiler optimizations of Thorium, then one could compile it for themselves
 following the instructions [Here](https://github.com/ungoogled-software/ungoogled-chromium/blob/master/docs/building.md), and using the changes in Thorium's [//build directory](https://github.com/Alex313031/thorium/tree/main/src/build) as a starting point for adding the compiler optimizations to UnGoogled's build config. I am
 willing to provide guidance or help with compiling Chromium or integrating Thorium's compiler optimization patches to an existing source code checkout.

__<a id="5"></a>5. Q: How can I build Thorium for myself?__  
 __A:__ See the documentation for various platforms here > https://thorium.rocks/docs/
