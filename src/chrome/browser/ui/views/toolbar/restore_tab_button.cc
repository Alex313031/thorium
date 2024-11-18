// Copyright 2024 The Chromium Authors and Alex313031
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/ui/views/toolbar/restore_tab_button.h"

#include "chrome/app/chrome_command_ids.h"
#include "chrome/browser/command_updater.h"
#include "chrome/browser/external_protocol/external_protocol_handler.h"
#include "chrome/browser/ui/browser.h"
#include "components/vector_icons/vector_icons.h"
//#include "ui/base/l10n/l10n_util.h"
#include "ui/base/metadata/metadata_impl_macros.h"
#include "ui/base/ui_base_features.h"
#include "ui/views/accessibility/view_accessibility.h"
#include "ui/views/controls/button/button_controller.h"

RestoreTabButton::RestoreTabButton(CommandUpdater* command_updater)
    : ToolbarButton(base::BindRepeating(&RestoreTabButton::ButtonPressed,
                                        base::Unretained(this))),
      command_updater_(command_updater) {

  SetIcon();

  constexpr char16_t RestoreTabAccessName[] = u"Restore Tab Button";
  GetViewAccessibility().SetName(RestoreTabAccessName);
  constexpr char16_t RestoreTabAccessToolTipName[] = u"Restore Tab";
  SetTooltipText(RestoreTabAccessToolTipName);
  button_controller()->set_notify_action(
      views::ButtonController::NotifyAction::kOnPress);
}

RestoreTabButton::~RestoreTabButton() = default;

void RestoreTabButton::ButtonPressed() {
  ExternalProtocolHandler::PermitLaunchUrl();

  int command = IDC_RESTORE_TAB;
  ExecuteBrowserCommand(command);
}

void RestoreTabButton::SetIcon() {
  SetVectorIcon(vector_icons::kRestoreIcon);
}

void RestoreTabButton::ExecuteBrowserCommand(int command) {
  if (!command_updater_) {
    return;
  }
  command_updater_->ExecuteCommand(command);
}

BEGIN_METADATA(RestoreTabButton)
END_METADATA
