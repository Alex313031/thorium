// Copyright (c) 2022 The ungoogled-chromium Authors and Alex313031. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_THORIUM_FLAG_ENTRIES_H_
    {"custom-ntp",
     "Custom New Tab Page",
     "Allows setting a custom URL for the new tab page.  Value can be internal (e.g. `about:blank`), external (e.g. `example.com`), or local (e.g. `file:///tmp/startpage.html`).  This applies for incognito windows as well when not set to a `chrome://` internal page.",
     kOsDesktop, ORIGIN_LIST_VALUE_TYPE("custom-ntp", "")},
    {"hide-sidepanel-button",
     "Hide SidePanel Button",
     "Hides the Thorium Side Panel Button.",
     kOsDesktop, SINGLE_VALUE_TYPE("hide-sidepanel-button")},
#define CHROME_BROWSER_THORIUM_FLAG_ENTRIES_H_
#endif  // CHROME_BROWSER_THORIUM_FLAG_ENTRIES_H_
