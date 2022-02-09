#pragma once
#ifndef __PAK_FILE_H__
#define __PAK_FILE_H__
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "pak_defs.h"
#include "pak_header.h"

/**
 * Package a pak file, returns NULL_File on failure.
 * Note: this would allocate memory.
 * @param MyPakHeader *myHeader - pointer to MyPakHeader struct.
 * @param PakFile *pakResFile - pointer to all resource files.
 * @param PakAlias *pakAlias - pointer to all alias.
 * @return PakFile - the packed file.
 */
PakFile pakPackFiles(MyPakHeader *myHeader, PakFile *pakResFile,
                     PakAlias *pakAlias);

/**
 * Get a single file from pak, returns NULL_File on failure.
 * @param void *pakBuffer - pointer to pak.
 * @param uint16_t *id - target file id.
 * @return PakFile - the target file.
 */
PakFile pakGetFile(uint8_t *pakBuffer, uint16_t id);

/**
 * Get all files from pak, returns NULL on failure.
 * Note: this would allocate memory.
 * @param void* buffer - pointer to pak.
 * @return PakFile* - pointer to all files in pak.
 */
PakFile *pakGetFiles(uint8_t *buffer);

#endif // __PAK_FILE_H__
