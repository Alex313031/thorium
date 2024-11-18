// Copyright 2024 The Chromium Authors and Alex313031
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_UI_VIEWS_FRAME_WINDOW_CAPTION_UTIL_H_
#define CHROME_BROWSER_UI_VIEWS_FRAME_WINDOW_CAPTION_UTIL_H_

class Browser;

// Static-only class containing values and helper functions for frame classes
// that need to be accessible outside of /browser/ui/views.
class WindowCaptionUtil {
 public:
  // Returns true if the Windows caption button is enabled.
  static bool IsWindowsTabSearchCaptionButtonEnabled(const Browser* browser);

 private:
  WindowCaptionUtil() {}
};

#endif  // CHROME_BROWSER_UI_VIEWS_FRAME_WINDOW_CAPTION_UTIL_H_
