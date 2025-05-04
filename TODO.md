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

--M132--

Add full version renaming support for Linux and Windows, and partial for MacOS.
Add GM2 inkdrop for tabstrip control buttons.
Remove //components/optimization_guide_core/optimization_guide_features.cc after upstream MacOS fix

Opt with BOLT > https://aaupov.github.io/blog/2022/11/12/bolt-chromium
SSD Restore > https://github.com/Alex313031/thorium/issues/61
Add textbox disk cache dir flag > https://github.com/Alex313031/thorium/issues/860
Fix outline of omnibox and bookmarks bar underline with themes

https://source.chromium.org/chromium/chromium/src/+/99fe5ddf4ecd908fd52d1d03565176f2f481c79e

Make UA and hints report chrome, and set hints

fix thorium and shell portable --temp profile

Test non-optimized Thorium, versus optimized Thorium, versus vanilla Chromium, versus vanilla Google Chrome, all at the same revision.

Fix NTP blank with other search engines than Google
Fix multiple profiles lag if exists
Fix download shelf height better and prevent separators from disappearing on resize
Fix unmaximized drag height for all platforms, possibly unify at 6,7,8 pixels, also GetDragHandleExtension
Add restart thorium, report issue, and whats new menu items from Primo and new function
Fix system extension integration like with dad uninstall issue
Fix button sizing and jumping in toolbar on non-pixel aligned displays
Fix omnibox dropdown alignment and search icon alignment again
Add vector icon colors for top bar from Primo
Replace old colour coded icons with the new ones from the chromium project in thorium/tree/main/src/ui/webui/resources/images
New feature that firefox has that allows you to customize the toolbar fully
Thorium is often taken advantage of using grabber software (any software that tries to grab including and not limited to: browser cookies, history) add master keys and regularly lock the browser if any unauthorized program attempts to access cookies and the things mentioned earlier.

Fix Tab group suggestions not available when you try to organise similar tabs by right clicking on a tab > "Organise similar tabs"
Fix omnibox dropdown highlight row right side rounding
Fix side panel metrics and grab handle icon alignment
Fix download shelf height dynamically, inspect startpadding and Alex313031: TODO: Use NTB method to p in chrome/browser/ui/views/download/download_item_view.cc

add thorium 2024 if else:
-  return gfx::Rect(x, NonClientTopHeight(false), std::max(0, available_width),
+  return gfx::Rect(x, GetTabStripInsetsTop(false), std::max(0, available_width),
