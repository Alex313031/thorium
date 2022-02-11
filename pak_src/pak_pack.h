#pragma once
#ifndef __PAK_PACKER_UNPACKER_H__
#define __PAK_PACKER_UNPACKER_H__

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/stat.h>
#include <sys/types.h>
#endif
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "pak_defs.h"
#include "pak_file.h"
#include "pak_file_io.h"
#include "pak_get_file_type.h"
#include "pak_header.h"

/**
 * Unpack a pak file to target path.
 * @param void *buffer - pointer to target pak buffer.
 * @param char *outputPath - target path.
 * @return bool - succeed or not.
 */
bool pakUnpack(uint8_t *buffer, char *outputPath);

/**
 * Pack a pak file from target index (ini) file,
 * returns NULL_File on failure.
 * Note: this would allocate memory.
 * @param PakFile pakIndex - target pak index file.
 * @param char *path - target path.
 * @return PakFile - packed pak.
 */
PakFile pakPack(PakFile pakIndex, char *path);

/**
 * Count number of specified char in string.
 * @param char *string - target string.
 * @param uint32_t length - target string length.
 * @param char toCount - specified char to count.
 * @return uint32_t - number of specified char.
 */
uint32_t countChar(const char *string, uint32_t length, char toCount);

#endif // __PAK_PACKER_UNPACKER_H__
