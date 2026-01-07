// Copyright 2024 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GOOGLE_APIS_DEFAULT_API_KEYS_H_
#define GOOGLE_APIS_DEFAULT_API_KEYS_H_

#include "build/build_config.h"

namespace google_apis {

// A trivial struct bundling default API key values defined at compile time
// through preprocessor directives. Useful for injecting these values for
// testing.
struct DefaultApiKeys {
  // Used to indicate an unset key/id/secret.  This works better with
  // various unit tests than leaving the token empty.
  static constexpr char kUnsetApiToken[] = "dummytoken";

  // Real keys
  static constexpr char kThorApiKey[] = "AIzaSyCbsbvGCe7C9mCtdaTycZB2eUFuzsYKG_E";
  static constexpr char kThorClientId[] = "77185425430.apps.googleusercontent.com";
  static constexpr char kThorClientSecret[] = "OTJgUOQcT7lO7GsGZq2G4IlT";
  // Remoting
  static constexpr char kThorApiKeyRemoting[] = "AIzaSyCbsbvGCe7C9mCtdaTycZB2eUFuzsYKG_E"; // Same as kThorApiKey
  static constexpr char kThorClientIdRemoting[] = "440925447803-avn2sj1kc099s0r7v62je5s339mu0am1.apps.googleusercontent.com";
  static constexpr char kThorClientSecretRemoting[] = "Bgur6DFiOMM1h8x-AQpuTQlK";
  // SODA
  static constexpr char kThorApiKeySoda[] = "ce04d119-129f-404e-b4fe-6b913fffb6cb";

  bool allow_unset_values;
  bool allow_override_via_environment;

  const char* google_api_key;
  const char* google_metrics_signing_key;
#if BUILDFLAG(IS_ANDROID)
  const char* google_api_key_android_non_stable;
#else
  const char* google_api_key_hats;
#endif  // BUILDFLAG(IS_ANDROID)
  const char* google_api_key_remoting;
  const char* google_api_key_soda;
#if BUILDFLAG(IS_CHROMEOS)
  const char* google_api_key_sharing;
  const char* google_api_key_read_aloud;
  const char* google_api_key_fresnel;
  const char* google_api_key_boca;
  const char* google_api_key_cros_system_geo_;
  const char* google_api_key_cros_chrome_geo_;
#endif

  const char* google_client_id_main;
  const char* google_client_secret_main;

  const char* google_client_id_remoting;
  const char* google_client_secret_remoting;

  const char* google_client_id_remoting_host;
  const char* google_client_secret_remoting_host;

  const char* google_default_client_id;
  const char* google_default_client_secret;
};

}  // namespace google_apis

#endif  // GOOGLE_APIS_DEFAULT_API_KEYS_H_
