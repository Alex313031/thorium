# Thorium

<img src="https://github.com/Alex313031/Thorium/blob/main/logos/product_logo_128.png">

## Chromium fork for linux named after radioactive element No. 90
- Always built with latest x64 tip-o-tree "Trunk" build of chromium \
- Intended to behave the most like Google Chrome, with differences listed below. \

 -Includes Widevine, ffmpeg, chrome plugins, as well as thinLTO and PGO compiler optimizations.

### EXPERIMENTAL FEATURES/DIFFERENCES BETWEEN CHROMIUM AND THORIUM
> - Experimental MPEG-DASH.
> - Experimental PDF annotation support (called "Ink" on ChromiumOS).
> - Patches from Debian including font rendering patch, VAAPI Patch, native notifications patch, title bar patch, and... the VDPAU Patch!! (Rejoice NVidia users)
> - DoH (DNS over HTTPS) patch from Bromite.
> - Logo and Branding/Naming changed to Thorium logo, Thorium name, and "Thorium Authors" being appended to The Chromium Authors in credits.

-args.gn exclude API Keys (you can get them yourself) and the pgo profile path is different for each chromium version. (See below.)

In general follow build instructions at https://chromium.googlesource.com/chromium/src/+/refs/heads/main/docs/linux/build_instructions.md and API Keys (if desired) at https://www.chromium.org/developers/how-tos/api-keys

# Building
_**The scripts assume the chromium source is at $HOME/chromiums/src/. You may have to sudo chmod +x the scripts to make them executable.**_ 
- After initial download of source code, run (from where you cloned this repo) `trunk.sh`. This will update and sync the sources and at the end it will download the PGO profile for chromium. The file will be downloaded to //chromium/src/chrome/build/pgo_profiles/*.profdata with the actual file name looking something like 'chrome-linux-main-1632505958-ddbb37bcdfa7dbd7b10cf3a9b6a5bc45e7a958a6.profdata', which should be added to the end of args.gn as per below.
- Then (from where you cloned this repo) run `setup.sh`. This will copy all the files and patches to the needed locations.
- Run `gn args out/thorium` and the contents of args.gn in this repo should be copy/pasted *--Include your api keys here at the top or leave blank, and edit the last line to point to the actual path and file name file name of '*.profdata'.*
- To build, run `autoninja -j8 -C out/thorium chrome chrome_sandbox content_shell -d stats` *The -j# can be changed to limit or increase the number of jobs (generally should be the number of CPU cores on your machine), and the -d stats at the end just shows better verbose stats during compiling. You could also append chromedriver after content_shell to build chromedriver, the selenium compatible browser fuzzing library.*
- To install, copy/paste the contents of your *out/thorium* dir to a good location I.E. *$HOME/bin/thorium*. **RECOMMENDED - Copy and run clean.sh within this dir to clean up build artifacts**. *Then you can just run the browser with `~/bin/thorium/chrome` or the content_shell with `~/bin/thorium/content_shell`.*
- **Proper Install:** To install with a deb, dont copy out/thorium, instead run `autoninja -C out/thorium/ "chrome/installer/linux:unstable_deb"` A nice .deb file will now be in out/thorium and you can install it with `sudo dpkg --install *.deb` It will be called chromium-browser-unstable, but the .debs in the releases section are just renamed to thorium-browser.

Thanks to https://github.com/robrich999/ for some info that went into this project.\
Also thanks to https://github.com/bromite/bromite and https://github.com/saiarcot895/chromium-ubuntu-build for patch code.
