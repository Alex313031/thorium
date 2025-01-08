# Android args.gn files

debug_args.gn is for arm64 only, but could be modified according to the other ones to
 support different microarchitectures.
&nbsp;&nbsp; android_full_debug = true can be used in debug builds

chromium_args.gn is for making a vanilla official non-debug Chromium build

API Keys allow location features, but do not allow Google Sync to function like on desktop.
This is due to Google being an asshole an adding extra restrictions to Android Chromium.
