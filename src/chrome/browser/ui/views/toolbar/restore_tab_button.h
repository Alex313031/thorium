// Copyright 2024 The Chromium Authors and Alex313031
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_UI_VIEWS_TOOLBAR_RESTORE_TAB_BUTTON_H_
#define CHROME_BROWSER_UI_VIEWS_TOOLBAR_RESTORE_TAB_BUTTON_H_

#include "base/memory/raw_ptr.h"
#include "chrome/browser/ui/views/toolbar/toolbar_button.h"
#include "ui/base/metadata/metadata_header_macros.h"

class CommandUpdater;

class RestoreTabButton : public ToolbarButton {
  METADATA_HEADER(RestoreTabButton, ToolbarButton)

 public:
  explicit RestoreTabButton(CommandUpdater* command_updater);
  RestoreTabButton(const RestoreTabButton&) = delete;
  RestoreTabButton& operator=(const RestoreTabButton&) = delete;
  ~RestoreTabButton() override;

 private:
  void SetIcon();
  void ButtonPressed();
  void ExecuteBrowserCommand(int command);

  raw_ptr<CommandUpdater, DanglingUntriaged> command_updater_;
};

#endif  // CHROME_BROWSER_UI_VIEWS_TOOLBAR_RESTORE_TAB_BUTTON_H_
