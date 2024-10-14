## TODO document for ideas and patches. Underneath each will be a comment with info or location of patch.

&#45; Adjust DPI from 300 to 72 for Windows .ico files.

 - Gimp and convert.exe to check ico sizes

&#45; Add new Thorium shortcuts to ThoriumOS Shortcuts App

 - For ChromeOS only: If you plan on adding a new accelerator and want it
 displayed in the Shortcuts app, please follow the instructions at: `ash/webui/shortcut_customization_ui/backend/accelerator_layout_table.h`.

&#45; Add GPC patch

## M128

&#45; https://source.chromium.org/chromium/chromium/src/+/99fe5ddf4ecd908fd52d1d03565176f2f481c79e

&#45; Make UA and hints report chrome, and set hints

&#45; fix thorium and shell portable --temp profile

&#45; Fix bookmarks bar underline, multiple profiles lag
&#45; Fix tab search button and menu position
&#45; Test non-optimized Thorium, versus optimized Thorium, versus vanilla Chromium, versus vanilla Google Chrome, all at the same revision.

### Notes to self

## .grd text replacements (search with grep), because there are a few places we want to tweak some strings.

&#45; Chromium > Thorium
&#45; Chrome > Thorium
&#45; Google Thorium > Thorium
&#45; Google recommends Thorium > Alex313031 recommends Thorium
&#45; Thorium Web Store > Chrome Web Store (Except some UI elements like NTP)
&#45; Thorium Remote Desktop > Chrome Remote Desktop
&#45; ThoriumOS Flex > ThoriumOS
&#45; made possible by Thorium > Chromium
&#45; Experiments > Thorium Experiments
&#45; Aw, Snap! > Aw, #@%!, this tab's process has gone bye bye...
&#45; Dino strings in components/error_page_strings.grdp
