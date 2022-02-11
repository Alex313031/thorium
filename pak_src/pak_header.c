#include "pak_header.h"

bool pakParseHeader(void *buffer, MyPakHeader *myHeader) {
    memset(myHeader, 0, sizeof(MyPakHeader));
    myHeader->version = pakGetVerison(buffer);
    if (myHeader->version == 5) {
        PakHeaderV5 *header = (PakHeaderV5 *)buffer;
        myHeader->resource_count = header->resource_count;
        myHeader->encoding = header->encoding;
        myHeader->alias_count = header->alias_count;
        myHeader->size = PAK_HEADER_SIZE_V5;
    } else if (myHeader->version == 4) {
        PakHeaderV4 *header = (PakHeaderV4 *)buffer;
        myHeader->resource_count = header->resource_count;
        myHeader->encoding = header->encoding;
        myHeader->alias_count = 0;
        myHeader->size = PAK_HEADER_SIZE_V4;
    } else {
        puts(PAK_ERROR_UNKNOWN_VER);
        return false;
    }
    return true;
}

unsigned int pakWriteHeader(MyPakHeader *myHeader, void *buffer) {
    if (buffer == NULL || myHeader == NULL)
        return 0;
    if (myHeader->version == 5) {
        PakHeaderV5 *header = (PakHeaderV5 *)buffer;
        header->version = myHeader->version;
        header->resource_count = (uint16_t) myHeader->resource_count;
        header->encoding = myHeader->encoding;
        header->alias_count = myHeader->alias_count;
    } else if (myHeader->version == 4) {
        PakHeaderV4 *header = (PakHeaderV4 *)buffer;
        header->version = myHeader->version;
        header->resource_count = myHeader->resource_count;
        header->encoding = myHeader->encoding;
    } else {
        puts(PAK_ERROR_UNKNOWN_VER);
        return 0;
    }
    return myHeader->size;
}

bool pakCheckFormat(uint8_t *buffer, unsigned int size) {
    MyPakHeader myHeader;
    if (!pakParseHeader(buffer, &myHeader)) {
        return false;
    }
    if (size < myHeader.size + (myHeader.resource_count + 1) * PAK_ENTRY_SIZE +
                   myHeader.alias_count * PAK_ALIAS_SIZE) {
        puts(PAK_ERROR_TRUNCATED);
        return false;
    }
    PakEntry *entryPtr = (PakEntry *)(buffer + myHeader.size);
    for (unsigned int i = 0; i <= myHeader.resource_count; i++) {
        uint32_t offset = entryPtr->offset;
        if (size < offset) {
            puts(PAK_ERROR_TRUNCATED);
            return false;
        }
        entryPtr++;
    }
    return true;
}
