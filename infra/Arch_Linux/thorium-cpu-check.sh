#!/usr/bin/env bash

set -euo pipefail

readonly installed_package='thorium-browser-beta-bin'
readonly baseline_package='thorium-browser-beta-bin'

# Use the intersection of feature flags from all reported x86 processors.
cpu_flags() {
	awk '
		/^flags[[:space:]]*:/ {
			line = $0
			sub(/^[^:]*:[[:space:]]*/, "", line)
			count = split(line, current, /[[:space:]]+/)

			delete present
			for (field = 1; field <= count; field++) {
				if (current[field] != "") {
					present[current[field]] = 1
				}
			}

			if (!seen) {
				for (flag in present) {
					common[flag] = 1
				}
				seen = 1
			} else {
				for (flag in common) {
					if (!(flag in present)) {
						delete common[flag]
					}
				}
			}
		}
		END {
			if (!seen) {
				exit 2
			}
			for (flag in common) {
				print flag
			}
		}
	' /proc/cpuinfo
}

has_all_flags() {
	local available=$1
	shift
	local required

	for required in "$@"; do
		grep -qx -- "$required" <<< "$available" || return 1
	done
}

# Derived from gz83/thorium M156's thorium_x86_profile and check_simd.py.
# Unlike M138, neither AVX nor AVX2 requires AES-NI or PCLMUL.
required_flags() {
	case $1 in
		thorium-browser-beta-bin)
			printf '%s\n' pni
			;;
		thorium-browser-beta-sse4-bin)
			printf '%s\n' pni ssse3 sse4_1
			;;
		thorium-browser-beta-avx-bin)
			printf '%s\n' pni avx
			;;
		thorium-browser-beta-avx2-bin)
			printf '%s\n' pni avx avx2 fma f16c
			;;
		thorium-browser-beta-avx512-bin)
			printf '%s\n' pni avx avx2 fma f16c \
				avx512f avx512cd avx512vl avx512bw avx512dq
			;;
		*)
			return 1
			;;
	esac
}

package_supported() {
	local available=$1 package=$2
	local -a required
	mapfile -t required < <(required_flags "$package") || return 1
	(( ${#required[@]} > 0 )) && has_all_flags "$available" "${required[@]}"
}

select_x86_64_package() {
	local available=$1
	local package
	for package in \
		thorium-browser-beta-avx512-bin \
		thorium-browser-beta-avx2-bin \
		thorium-browser-beta-avx-bin \
		thorium-browser-beta-sse4-bin \
		thorium-browser-beta-bin; do
		if package_supported "$available" "$package"; then
			printf '%s\n' "$package"
			return 0
		fi
	done
	return 1
}

runtime_arch() {
	local machine bits
	machine=$(uname -m) || return 1
	case $machine in
		aarch64|arm64)
			printf '%s\n' 'aarch64'
			;;
		i?86)
			printf '%s\n' 'i686'
			;;
		x86_64|amd64)
			bits=$(getconf LONG_BIT 2>/dev/null) || return 1
			if [[ $bits == 32 ]]; then
				printf '%s\n' 'i686'
			elif [[ $bits == 64 ]]; then
				printf '%s\n' 'x86_64'
			else
				return 1
			fi
			;;
		*)
			return 1
			;;
	esac
}

show_popup() {
	local message=$1 urgency=$2

	printf '%s\n' "$message" >&2
	if [[ -n ${DISPLAY:-} || -n ${WAYLAND_DISPLAY:-} ]]; then
		notify-send --app-name='Thorium Browser Beta' --urgency="$urgency" \
			'Thorium Browser Beta package warning' "$message" >/dev/null 2>&1 &
	fi
}

mode=${1:-runtime}
if (( $# > 1 )); then
	printf 'Usage: %s [build|runtime]\n' "${0##*/}" >&2
	exit 2
fi
case $mode in
	build|runtime) ;;
	*)
		printf 'Usage: %s [build|runtime]\n' "${0##*/}" >&2
		exit 2
		;;
esac

if [[ $mode == build && ${THORIUM_PACKAGING_SERVER:-0} == 1 ]]; then
	exit 0
fi

if [[ $mode == build ]]; then
	target_arch=${CARCH:-}
else
	target_arch=$(runtime_arch) || target_arch=''
fi

# 0: compatible; 1: incompatible; 2: inconclusive.
detection_status=2
detected_package=''
case $target_arch in
	aarch64)
		if [[ $installed_package == "$baseline_package" ]]; then
			if [[ $mode == runtime || $(uname -m) == aarch64 ]]; then
				detection_status=0
				detected_package=$installed_package
			fi
		else
			detection_status=1
		fi
		;;
	i686)
		if [[ $installed_package != "$baseline_package" ]]; then
			detection_status=1
		elif available=$(cpu_flags); then
			if has_all_flags "$available" sse2; then
				detection_status=0
				detected_package=$installed_package
			else
				detection_status=1
			fi
		fi
		;;
	x86_64)
		if available=$(cpu_flags); then
			if detected_package=$(select_x86_64_package "$available"); then
				if package_supported "$available" "$installed_package"; then
					detection_status=0
				else
					detection_status=1
				fi
			else
				detection_status=1
			fi
		fi
		;;
esac

case $mode in
	build)
		if (( detection_status == 0 )) && [[ $detected_package == "$installed_package" ]]; then
			exit 0
		elif (( detection_status == 0 )); then
			printf 'WARNING: %s is compatible; variant %s could perform better. This is advisory only.\n' \
				"$installed_package" "$detected_package" >&2
		elif (( detection_status == 1 )); then
			printf 'WARNING: %s cannot run with these architecture/CPU features; packaging may continue.\n' \
				"$installed_package" >&2
		else
			printf 'WARNING: CPU compatibility could not be determined; packaging may continue.\n' >&2
		fi
		;;
	runtime)
		if (( detection_status == 0 )) && [[ $detected_package == "$installed_package" ]]; then
			exit 0
		elif (( detection_status == 0 )); then
			message="The installed ${installed_package} can run, but ${detected_package} may perform better. Switching packages is optional."
			urgency='low'
		elif (( detection_status == 1 )); then
			message="The installed ${installed_package} is incompatible with this CPU or architecture and is not expected to run."
			if [[ -n $detected_package ]]; then
				message+=" Compatible beta package: ${detected_package}."
			fi
			urgency='critical'
		else
			message='Thorium Browser Beta could not determine CPU compatibility.'
			urgency='normal'
		fi
		show_popup "$message" "$urgency"
		;;
esac
