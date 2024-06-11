## Chromium Flatpak

### Extension points

To avoid having to expose more of the host filesystem in the sandbox but still
allowing extending Chromium, the `rocks.thorium.Thorium.Extension` extension
point is defined.

This extension point is currently on version '1' and will expose any extension
manifests under the `extensions` subdirectory, policy files under
`policies/managed` and `policies/recommended`, and [native messaging host
manifests](https://developer.chrome.com/docs/apps/nativeMessaging/) under
`native-messaging-hosts`.

#### Legacy extension points

This application also supports two other extension points:
`rocks.thorium.Thorium.Policy` and `rocks.thorium.Thorium.NativeMessagingHost`.
These primarily exist for compatibility reasons and should not be used.

#### Using extension points

Extension points can be provided as regular flatpaks and an example is provided
under `examples/policies/google-safe-search`. Important to note that extension
points' name must follow the syntax of `Extension.<id>`, where `<id>` is a
generic id for this specific extension point.

Flatpak also supports “unmanaged extensions”, allowing loading extensions installed
into `/var/lib/flatpak/extension` and `$XDG_DATA_HOME/flatpak/extension`.
This can be useful for example to allow system administrators to expose system installed
policies, extensions, etc.

One example of such "unmanaged extension" could be an extension point that exposes
all system policies installed under `/etc/chromium-browser/policies/{managed,recommended}`.
This could be done for example by creating an extension point under
`/var/lib/flatpak/extension/rocks.thorium.Thorium.Extension.system-policies`, with
`/var/lib/flatpak/extension/rocks.thorium.Thorium.Extension.system-policies/<arch>/<version>`
being a symlink to `/etc/chromium-browser`. Note that `<version>` must match the
extension point version.

Also important to note that in the example above one would not be able to symlink the
actual policy file directly, as otherwise flatpak would not be able to resolve the
symlink when bind mounting the extension point.

### Building and updating

[CroFT](https://github.com/refi64/croft) is used to manage the patches in this
repository and work with a build environment.

### Build arguments to pass to gn

```
# Not required but makes builds faster.
use_lld = true
# NaCL hasn't been tested and is being removed from Linux builds.
enable_nacl = false
# Unrelated to Flatpak but helps speed up builds.
blink_symbol_level = 0
# Outdated
use_gnome_keyring = false
# Not supported
use_sysroot = false
```
