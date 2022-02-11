#pragma once
#ifndef __PAK_DEFINITIONS_H__
#define __PAK_DEFINITIONS_H__
// msvc hacks
#ifdef _MSC_VER
#include <windows.h>
#define ftello ftell
#define fseeko fseek
#define PATH_MAX MAX_PATH
#endif
// linux header
#ifdef __linux__
#include <linux/limits.h>
#include <unistd.h>
#endif
// macOS headers
#ifdef __APPLE__
#include <sys/syslimits.h>
#include <unistd.h>
#endif

#include <stdbool.h>
#include <stdint.h>

#pragma pack(push)
#pragma pack(1)

// v5 header:
// uint32(version), uint8(encoding), 3 bytes padding,
// uint16(resource_count), uint16(alias_count)
typedef struct PakHeaderV5 {
    uint32_t version;
    uint8_t encoding;
    uint8_t _padding[3];
    uint16_t resource_count;
    uint16_t alias_count;
} PakHeaderV5;

// v4 header: uint32(version), uint32(resource_count), uint8(encoding)
typedef struct PakHeaderV4 {
    uint32_t version;
    uint32_t resource_count;
    uint8_t encoding;
} PakHeaderV4;

// Entry: uint16_t resourceId; uint32_t offset;
typedef struct PakEntry {
    uint16_t resource_id;
    uint32_t offset;
} PakEntry;

// Alias: uint16_t resourceId; uint16_t entry_index;
typedef struct PakAlias {
    uint16_t resource_id;
    uint16_t entry_index;
} PakAlias;

#pragma pack(pop)

/**
 * Get pak version from pak buffer.
 * @param void *buffer - pointer to pak.
 * @return uint32_t - pak version.
 */
#define pakGetVerison(buffer) (*(uint32_t *)(buffer))
#define PAK_HEADER_SIZE_V4 sizeof(PakHeaderV4)
#define PAK_HEADER_SIZE_V5 sizeof(PakHeaderV5)
#define PAK_ENTRY_SIZE sizeof(PakEntry)
#define PAK_ALIAS_SIZE sizeof(PakAlias)

#define PAK_ERROR_UNKNOWN_VER "Error: Unknown pak version."
#define PAK_ERROR_TRUNCATED "Error: Probably truncated pak file."
#define PAK_ERROR_BROKEN_INDEX "Error: Probably broken pak index file."

typedef struct MyPakHeader {
    uint32_t version;
    uint32_t resource_count;
    uint16_t alias_count;
    uint8_t encoding;
    uint8_t size;
} MyPakHeader;

typedef struct PakFile {
    uint16_t id;
    uint32_t size;
    void *buffer;
} PakFile;

// file-io
#define NULL_File ((PakFile){0, 0, NULL})
#define freeFile(file) (free((file).buffer))

// pack/unpack
#define PAK_BUFFER_BLOCK_SIZE 4096
#define PAK_BUFFER_MIN_FREE_SIZE 512
#define PAK_INDEX_GLOBAL_TAG "[Global]"
#define PAK_INDEX_RES_TAG "[Resources]"
#define PAK_INDEX_ALIAS_TAG "[Alias]"
#endif // __PAK_DEFINITIONS_H__
