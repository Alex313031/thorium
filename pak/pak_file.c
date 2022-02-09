#include "pak_file.h"
PakFile pakPackFiles(MyPakHeader *myHeader, PakFile *pakResFile,
                     PakAlias *pakAlias) {
    uint8_t *buffer = NULL;
    uint32_t size = myHeader->size;
    uint32_t entrySize = (myHeader->resource_count + 1) * PAK_ENTRY_SIZE;
    uint32_t aliasSize = myHeader->alias_count * PAK_ALIAS_SIZE;
    size += entrySize + aliasSize;
    for (uint32_t i = 0; i < myHeader->resource_count; i++) {
        size += (pakResFile + i)->size;
    }
    buffer = calloc(size, sizeof(uint8_t));
    if (buffer == NULL)
        return NULL_File;
    uint32_t offset = pakWriteHeader(myHeader, buffer);
    if (offset == 0) {
        free(buffer);
        return NULL_File;
    }

    PakEntry *enrtyPtr = (PakEntry *)(buffer + offset);
    uint8_t *filePtr = buffer + offset + entrySize + aliasSize;
    for (uint32_t i = 0; i < myHeader->resource_count; i++) {
        memcpy(filePtr, pakResFile->buffer, pakResFile->size);
        enrtyPtr->resource_id = pakResFile->id;
        enrtyPtr->offset = filePtr - buffer;
        filePtr += pakResFile->size;
        enrtyPtr++;
        pakResFile++;
    }

    enrtyPtr->resource_id = 0;
    enrtyPtr->offset = (pakResFile - 1)->size + (enrtyPtr - 1)->offset;
    void *aliasPtr = (void *)(enrtyPtr + 1);
    memcpy(aliasPtr, pakAlias, aliasSize);
    PakFile pakFile;
    pakFile.buffer = buffer;
    pakFile.size = size;
    return pakFile;
}

PakFile pakGetFile(uint8_t *pakBuffer, uint16_t id) {
    PakFile pakFile = NULL_File;
    MyPakHeader myHeader;
    if (!pakParseHeader(pakBuffer, &myHeader)) {
        return NULL_File;
    }
    PakEntry *entryPtr = (PakEntry *)(pakBuffer + myHeader.size);
    if (myHeader.version == 5) {
        PakAlias *aliasPtr =
            (PakAlias *)(pakBuffer + myHeader.size +
                         (myHeader.resource_count + 1) * PAK_ENTRY_SIZE);
        for (uint16_t i = 0; i < myHeader.alias_count; i++) {
            if (aliasPtr->resource_id == id) {
                entryPtr += aliasPtr->entry_index;
                pakFile.buffer = pakBuffer + entryPtr->offset;
                pakFile.size = (entryPtr + 1)->offset - entryPtr->offset;
                return pakFile;
            }
        }
    }
    for (uint32_t i = 0; i < myHeader.resource_count; i++) {
        if (entryPtr->resource_id == id) {
            pakFile.buffer = pakBuffer + entryPtr->offset;
            pakFile.size = (entryPtr + 1)->offset - entryPtr->offset;
            return pakFile;
        }
        entryPtr++;
    }
    return NULL_File;
}

PakFile *pakGetFiles(uint8_t *buffer) {
    PakFile *pakResFile = NULL;
    MyPakHeader myHeader;
    if (!pakParseHeader(buffer, &myHeader)) {
        return NULL;
    }
    pakResFile = (PakFile *)calloc(myHeader.resource_count, sizeof(PakFile));
    PakFile *returnValue = pakResFile;
    if (pakResFile == NULL) {
        return NULL;
    }
    PakEntry *pakEntryPtr = (PakEntry *)(buffer + myHeader.size);
    for (uint32_t i = 0; i < myHeader.resource_count; i++) {
        uint32_t offset = pakEntryPtr->offset;
        pakResFile->id = pakEntryPtr->resource_id;
        pakResFile->buffer = buffer + offset;
        pakEntryPtr++;
        pakResFile->size = pakEntryPtr->offset - offset;
        pakResFile++;
    }
    return returnValue;
}
