solutions = [
  {
    "name": "src",
    "url": "https://chromium.googlesource.com/chromium/src.git",
    "managed": False,
    "custom_deps": {},
    "custom_vars": {
      "checkout_pgo_profiles": True
    },
  },
]

target_os = [ "linux", "win", "android", "chromeos" ]
target_cpu = [ "x64", "x86", "arm64", "arm" ]
