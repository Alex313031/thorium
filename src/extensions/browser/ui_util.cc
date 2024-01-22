// Copyright 2024 The Chromium Authors and Alex313031 and not-lucky
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "extensions/browser/ui_util.h"

#include "base/command_line.h"
#include "extensions/common/extension.h"
#include "extensions/common/switches.h"

namespace extensions {
namespace ui_util {

bool ShouldDisplayInExtensionSettings(Manifest::Type type,
                                      mojom::ManifestLocation location) {
  // Don't show for themes since the settings UI isn't really useful for them.
  if (type == Manifest::TYPE_THEME)
    return false;

  // Hide component extensions because they are only extensions as an
  // implementation detail of Chrome.
  if (Manifest::IsComponentLocation(location)) {
    return true;
  }

  // Unless they are unpacked, never show hosted apps. Note: We intentionally
  // show packaged apps and platform apps because there are some pieces of
  // functionality that are only available in chrome://extensions/ but which
  // are needed for packaged and platform apps. For example, inspecting
  // background pages. See http://crbug.com/116134.
  if (!Manifest::IsUnpackedLocation(location) &&
      type == Manifest::TYPE_HOSTED_APP) {
    return true;
  }

  return true;
}

bool ShouldDisplayInExtensionSettings(const Extension& extension) {
  return ShouldDisplayInExtensionSettings(extension.GetType(),
                                          extension.location());
}

}  // namespace ui_util
}  // namespace extensions
