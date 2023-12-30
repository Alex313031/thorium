#!/bin/bash

# Copyright (c) 2023 Alex313031.

program="Thorium Browser." # The program we are checking for.

YEL='\033[1;33m' # Yellow
CYA='\033[1;96m' # Cyan
RED='\033[1;31m' # Red
GRE='\033[1;32m' # Green
c0='\033[0m' # Reset Text
bold='\033[1m' # Bold Text
underline='\033[4m' # Underline Text

# Error handling
yell() { echo "$0: $*" >&2; }
die() { yell "$*"; exit 111; }
try() { "$@" || die "${RED}Failed $*"; }

script_name=${0##*/}

check_result() { # Message calls
    local ret="$1"
    local msg="$2"
    [ "$ret" -ne 0 ] && { printf "${RED}FAIL:" && tput sgr0 && echo " $msg"; }
    printf "${GRE}SUCCESS:" && tput sgr0 && echo " $msg"
}

get_cpuinfo() { # return details of the first CPU only
    cat /proc/cpuinfo | awk 'BEGIN { RS = "" ; } { printf ("%s\n", $0); exit(0); }'
}

have_cpu_feature() {
    local feature="$1"
    get_cpuinfo | egrep -q "^flags.*\<$feature\>"
}

have_sse3_cpu_feature () {
    local feature="pni"
    local desc="Streaming SIMD Extensions 3"
    local need="$desc ($feature)"
    have_cpu_feature "$feature"
    check_result "$?" "$need"
}

have_sse41_cpu_feature () {
    local feature="sse4_1"
    local desc="Streaming SIMD Extensions 4.1"
    local need="$desc ($feature)"
    have_cpu_feature "$feature"
    check_result "$?" "$need"
}

have_aes_cpu_feature () {
    local feature="aes"
    local desc="Advanced Encryption Standard Extensions"
    local need="$desc ($feature)"
    have_cpu_feature "$feature"
    check_result "$?" "$need"
}

have_avx_cpu_feature () {
    local feature="avx"
    local desc="Advanced Vector Extensions"
    local need="$desc ($feature)"
    have_cpu_feature "$feature"
    check_result "$?" "$need"
}

have_avx2_cpu_feature () {
    local feature="avx2"
    local desc="Advanced Vector Extensions 2"
    local need="$desc ($feature)"
    have_cpu_feature "$feature"
    check_result "$?" "$need"
}

have_64bit_cpu() {
    local feature="lm" # "Long mode"
    local desc="64-bit x86 CPU"
    local need="$desc ($feature)"
    have_cpu_feature "$feature"
    check_result "$?" "$need"
}

common_checks() {
    have_64bit_cpu && have_sse3_cpu_feature && have_sse41_cpu_feature && \
    have_aes_cpu_feature && have_avx_cpu_feature && have_avx2_cpu_feature
}

check_host() {
	printf "\n" &&
    printf "${YEL}Checking if host is capable of building/running $program\n" && tput sgr0; echo
    common_checks
}

main() {
    case "$1" in
        host) check_host ;;
        *) printf "${RED}ERROR: Invalid type specified: '$1'\n" 2>&1 && tput sgr0; exit 1 ;;
    esac
}

main "host" && printf "\n" && exit 0
