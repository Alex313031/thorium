// Copyright 2024 The Chromium Authors and Alex313031
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "components/offline_pages/core/offline_page_model.h"

#include "url/gurl.h"

namespace offline_pages {

const int64_t OfflinePageModel::kInvalidOfflineId;

OfflinePageModel::SavePageParams::SavePageParams()
    : proposed_offline_id(OfflinePageModel::kInvalidOfflineId),
      is_background(false) {}

OfflinePageModel::SavePageParams::SavePageParams(const SavePageParams& other) =
    default;

OfflinePageModel::SavePageParams::~SavePageParams() = default;

// static
bool OfflinePageModel::CanSaveURL(const GURL& url) {
  return url.is_valid();
}

OfflinePageModel::OfflinePageModel() = default;

OfflinePageModel::~OfflinePageModel() = default;

}  // namespace offline_pages
