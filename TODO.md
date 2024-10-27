## TODO document for ideas and patches. Underneath each will be a comment with info or location of patch.

&#45; Adjust DPI from 300 to 72 for Windows .ico files.

 - Gimp and convert.exe to check ico sizes

&#45; Add new Thorium shortcuts to ThoriumOS Shortcuts App

 - For ChromeOS only: If you plan on adding a new accelerator and want it
 displayed in the Shortcuts app, please follow the instructions at: `ash/webui/shortcut_customization_ui/backend/accelerator_layout_table.h`.

### Notes to self

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
Aw, Snap! > Aw, #@%!, this tab's process has gone bye bye...
Dino strings in components/error_page_strings.grdp

 - Add GPC patch

--M128--

https://source.chromium.org/chromium/chromium/src/+/99fe5ddf4ecd908fd52d1d03565176f2f481c79e

Make UA and hints report chrome, and set hints

fix thorium and shell portable --temp profile

Fix bookmarks bar underline, multiple profiles lag
Fix tab search button and menu position
Fix download shelf height
Test non-optimized Thorium, versus optimized Thorium, versus vanilla Chromium, versus vanilla Google Chrome, all at the same revision.

