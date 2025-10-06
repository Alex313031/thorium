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
Fix omnibox dropdown highlight row right side rounding
Fix side panel metrics and grab handle icon alignment
Fix download shelf height dynamically, inspect startpadding and Alex313031: TODO: Use NTB method to p in chrome/browser/ui/views/download/download_item_view.cc
Add version to installer dialog and compare to make sure nothing missing from primo > https://github.com/primo-browser/primo/commit/674eec1b5f55705a111b9de4a344b2df93f3611a  
https://github.com/primo-browser/primo/commit/79d120a8aa93df406f062b1367cc6404f3924a27  
https://github.com/primo-browser/primo/commit/1769f7edff841ab70783d9fc9c96e5fd98f100b0  
https://github.com/primo-browser/primo/commit/8cf302bdc10a78085cbd38cb55660de659a129d6  
MV2: https://github.com/primo-browser/primo/commit/5aa8d52a29f3b1162462a389444fb98b90ec4a1e  
     https://github.com/primo-browser/primo/commit/de205e3623e8d922c97b949e82e7937c1c7f5f8a
     https://github.com/primo-browser/primo/commit/e567420253e358977714769e5b83ea32f374e008

add thorium 2024 if else:
-  return gfx::Rect(x, NonClientTopHeight(false), std::max(0, available_width),
+  return gfx::Rect(x, GetTabStripInsetsTop(false), std::max(0, available_width),

Fix spacing
+    const gfx::Rect new_tab_button_new_bounds =
         gfx::Rect(new_tab_button_new_position, new_tab_button_size);

remove
+    static const int kCornerRadius = corner_radius;

logging_installer.cc > chromium_installer.log > thorium_installer.log
make messagebox debug fail minimizeable, add PRIMO_DEBUG fixes to Thorium.
https://github.com/primo-browser/primo/commit/946f3dfbed983efb7643fee26d31fd3c4205d9d9
https://github.com/win32ss/supermium/issues/1412
https://github.com/win32ss/supermium/commit/f46111bb8a9d29315740a28dcb2ee96fdb50f586
https://github.com/win32ss/supermium/commit/535bb42b1f22d9fdaf4db0d8ee1ce1acf14a6fbf

Notes:
top_border_thickness
kTopResizeFrameArea
kNonClientRestoredExtraThickness
//maybe remove
Th24XOffset
GetLayoutConstant(TAB_BUTTON_OFFSET)
LOCATION_BAR_CHILD_CORNER_RADIUS:
Th24StrokeOffset
text_overriding_permission_chip_indent
UpdateThumb

// Other notes
investigate transparent tabs flags.setAlphaf(0.7f)
search tab_strip_region_views for layout_constants added
investigate new metrics in layout_provider and layout_constants

kBetweenIconSpacing might cause crash
