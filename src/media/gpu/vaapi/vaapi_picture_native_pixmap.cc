// Copyright 2023 The Chromium Authors and Alex313031
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/gpu/vaapi/vaapi_picture_native_pixmap.h"

#include "media/gpu/macros.h"
#include "media/gpu/vaapi/va_surface.h"
#include "media/gpu/vaapi/vaapi_wrapper.h"

namespace media {

VaapiPictureNativePixmap::VaapiPictureNativePixmap(
    scoped_refptr<VaapiWrapper> vaapi_wrapper,
    const MakeGLContextCurrentCallback& make_context_current_cb,
    const BindGLImageCallback& bind_image_cb,
    int32_t picture_buffer_id,
    const gfx::Size& size,
    const gfx::Size& visible_size,
    uint32_t texture_id,
    uint32_t client_texture_id,
    uint32_t texture_target)
    : VaapiPicture(std::move(vaapi_wrapper),
                   make_context_current_cb,
                   bind_image_cb,
                   picture_buffer_id,
                   size,
                   visible_size,
                   texture_id,
                   client_texture_id,
                   texture_target) {}

VaapiPictureNativePixmap::~VaapiPictureNativePixmap() = default;

bool VaapiPictureNativePixmap::DownloadFromSurface(
    scoped_refptr<VASurface> va_surface) {
  DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);
  if (!vaapi_wrapper_->SyncSurface(va_surface->id())) {
    VLOGF(1) << "Cannot sync VPP input surface";
    return false;
  }
  if (!vaapi_wrapper_->BlitSurface(*va_surface, *va_surface_)) {
    VLOGF(1) << "Cannot convert decoded image into output buffer";
    return false;
  }

  // Sync target surface since the buffer is returning to client.
  if (!vaapi_wrapper_->SyncSurface(va_surface_->id())) {
    VLOGF(1) << "Cannot sync VPP output surface";
    return false;
  }
  return true;
}

bool VaapiPictureNativePixmap::AllowOverlay() const {
  DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);
  return true;
}

VASurfaceID VaapiPictureNativePixmap::va_surface_id() const {
  return va_surface_->id();
}

}  // namespace media
