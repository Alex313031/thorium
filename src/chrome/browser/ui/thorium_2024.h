// Copyright 2024 Alex313031
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_UI_THORIUM_2024_UI_H_
#define CHROME_BROWSER_UI_THORIUM_2024_UI_H_

#include "base/command_line.h"

static bool Th24State() {
  static const bool thorium_2024 = base::CommandLine::ForCurrentProcess()->HasSwitch("thorium-2024");
  return thorium_2024;
}

#endif  // CHROME_BROWSER_UI_THORIUM_2024_UI_H_
