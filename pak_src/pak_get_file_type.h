#pragma once
#ifndef __PAK_GET_FILE_TYPE_H__
#define __PAK_GET_FILE_TYPE_H__
#include "pak_defs.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct FileType {
    char *type;
    char *identifer;
    uint8_t size;
} FileType;

/**
 * Generate a FileType, for internal uses only.
 * @param char* type - file extension.
 * @param char* identifer - file header identifer in string.
 * @see pakGetFileType()
 * @return FileType
 */
#define PAK_GEN_FILE_TYPE(type, identifer) \
    {type, identifer, sizeof(identifer) - 1}

/**
 * Get file extension form a file, returns "" on failure.
 * @param PakFile file - the file to parse.
 * @return char* - pointer to file extension.
 */
char *pakGetFileType(PakFile file);

#endif // __PAK_GET_FILE_TYPE_H__
