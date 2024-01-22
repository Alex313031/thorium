// Copyright 2024 The Chromium Authors and Alex313031
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "components/metrics/machine_id_provider.h"

#include <stdint.h>

#include "base/base_paths.h"
#include "base/command_line.h"
#include "base/check.h"
#include "base/system/sys_info.h"

namespace metrics {

// Checks if hardware model name is available.
bool MachineIdProvider::HasId() {
  if (base::CommandLine::ForCurrentProcess()->HasSwitch("disable-machine-id")) {
    return false;
  }
  return !base::SysInfo::HardwareModelName().empty();
}

// On non-windows, the machine id is based on the hardware model name.
// This will suffice as users are unlikely to change to the same machine model.
std::string MachineIdProvider::GetMachineId() {
  if (base::CommandLine::ForCurrentProcess()->HasSwitch("disable-machine-id")) {
    return std::string();
  }
  // Gets hardware model name. (e.g. 'Macbook Pro 16,1', 'iPhone 9,3')
  std::string hardware_model_name = base::SysInfo::HardwareModelName();

  // This function should not be called if hardware model name is unavailable.
  DCHECK(!hardware_model_name.empty());

  return hardware_model_name;
}
}  //  namespace metrics
