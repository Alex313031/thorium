#pragma once
#ifndef __PAK_HEADERS_H__
#define __PAK_HEADERS_H__
#include "pak_defs.h"
#include <stdio.h>
#include <string.h>

/**
 * Generate MyPakHeader struct from pak buffer.
 * @param _in_ void *buffer - pointer to pak buffer.
 * @param _out_ MyPakHeader myHeader - target file content.
 * @return bool - succeed or not.
 */
bool pakParseHeader(void *buffer, MyPakHeader *myHeader);

/**
 * Write pak header from MyPakHeader struct to buffer.
 * @param MyPakHeader myHeader - target MyPakHeader struct.
 * @param void *buffer - pointer to target pak buffer.
 * @return unsigned int - pak header size, 0 on failure.
 */
unsigned int pakWriteHeader(MyPakHeader *myHeader, void *buffer);

/**
 * Check if pak format is legal.
 * @param void *buffer - pointer to pak buffer.
 * @param unsigned int size - pak buffer size in bytes.
 * @return bool - succeed or not.
 */
bool pakCheckFormat(uint8_t *buffer, unsigned int size);

#endif // __PAK_HEADERS_H__
