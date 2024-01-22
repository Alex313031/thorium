#!/bin/bash

# Copyright (c) 2024 Alex313031.

# Export libs
LD_LIBRARY_PATH="$(pwd)/lib"
export LD_LIBRARY_PATH

./thorium_ui_debug_shell --debug
