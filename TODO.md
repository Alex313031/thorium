## TODO document for ideas and patches. Underneath each will be a comment with info or location of patch.

&#45; Adjust DPI from 300 to 72 for Windows .ico files.

 - Gimp and convert.exe to check ico sizes

&#45; Add native first run WebUI page

 - https://github.com/ungoogled-software/ungoogled-chromium/pull/2314/files

&#45; Update win scripts to work seamlessly

 - https://github.com/Alex313031/thorium/tree/main/win_scripts

&#45; Re-evaluate vector icons and webui images for CR23 refresh

 - To keep .icons correct

&#45; Add new Thorium shortcuts to ThoriumOS Shortcuts App

 - For ChromeOS only: If you plan on adding a new accelerator and want it
 displayed in the Shortcuts app, please follow the instructions at: `ash/webui/shortcut_customization_ui/backend/accelerator_layout_table.h`.

### Notes

## .grd text replacements (search with grep), because there are a few places we want to tweak some strings.

Chromium > Thorium
Chrome > Thorium
Google Thorium > Thorium
Google recommends Thorium > Alex313031 recommends Thorium
Thorium Web Store > Chrome Web Store (Except some UI elements like NTP)
Thorium Remote Desktop > Chrome Remote Desktop
ThoriumOS Flex > ThoriumOS
made possible by Thorium > Chromium
Experiments > Thorium Experiments

Add captive portals patch > https://github.com/uazo/cromite/blob/master/build/patches/Remove-detection-of-captive-portals.patch

Restore JXL support on Android.
