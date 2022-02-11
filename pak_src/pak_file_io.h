#pragma once
#ifndef __PAK_FILE_IO_H__
#define __PAK_FILE_IO_H__

#include "pak_defs.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * Read a file into memory, returns NULL_File on failure.
 * Note: this would allocate memory.
 * @param const char *fileName - target file name.
 * @return PakFile - target file content.
 */
PakFile readFile(const char *fileName);

/**
 * Write a file to storage, this would overwrite existing file.
 * @param const char *fileName - target file name.
 * @param PakFile file - target file content.
 * @return bool - succeed or not.
 */
bool writeFile(const char *fileName, const PakFile file);

#endif // __PAK_FILE_IO_H__
