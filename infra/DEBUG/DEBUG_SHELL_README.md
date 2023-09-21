# Thorium UI Debug Shell <img src="https://raw.githubusercontent.com/Alex313031/Thorium/main/logos/NEW/thorium_debug_shell/icon_256.png" width="36">

## Summary:
This is a special program, built on top of views_examples & content_shell and incorporating a multitude of options for testing, viewing, and debugging UI resources in Thorium. It builds views_examples_with_content, and renames it to thorium_ui_debug_shell. Building views_examples builds the program, but without content_shell linked in (which can be accessed via the *"WebView"* option in the dropdown menu).

## Linux Use
Run the Thorium_Debug_Shell.sh, and you can select from the dropdown menu. Some things are interactive, some load internal resources, and some require loading external resources like viewing .icon files. In that case, you can load a file using its full path in the box towards the bottom.

## Windows Use
Run the thorium_ui_debug_shell.exe, and you can select from the dropdown menu. Some things are interactive, some load internal resources, and some require loading external resources like viewing .icon files. In that case, you can load a file using its full path in the box towards the bottom.

## Use in Thorium
I built this to view and test native Chromium UI icons in the *.icon* format.
These files are in:

//chromium/src/ui/views/vector_icons ## For many subcomponents like native UI views. \
//chromium/src/ui/views/window/vector_icons ## For icons related to the top bar. \
//chromium/src/components/vector_icons ## For components that affect many build targets. \
//chromium/src/chrome/app/vector_icons ## For icons only used in the browser. \
//chromium/src/ash/resources/vector_icons ## For icons used in Chromium on ChromiumOS. \
//chromium/src/chromeos/ui/vector_icons ## For icons specific to ChromiumOS. \
//chromium/src/chromecast/ui/vector_icons ## For icons specific to ChromeCast.

*More info can be found at > https://chromium.googlesource.com/chromium/src.git/+/refs/heads/main/components/vector_icons/README.md*

## Building <img src="https://github.com/Alex313031/thorium/blob/main/logos/NEW/build_light.svg#gh-dark-mode-only"> <img src="https://github.com/Alex313031/thorium/blob/main/logos/NEW/build_dark.svg#gh-light-mode-only">

To build, use: autoninja -C out/thorium chrome chromedriver thorium_shell setup mini_installer thorium_ui_debug_shell or just
autoninja -C out/thorium thorium_ui_debug_shell to exclude building the rest of Thorium.

