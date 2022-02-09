#include "pak_get_file_type.h"

static const FileType FILE_TYPES[] = {
    PAK_GEN_FILE_TYPE(".png", "\x89\x50\x4E\x47\x0D\x0A\x1A\x0A"),
    PAK_GEN_FILE_TYPE(".html", "<!doctype html>"),
    PAK_GEN_FILE_TYPE(".html", "<!DOCTYPE html>"),
    PAK_GEN_FILE_TYPE(".html", "<html>"),
    PAK_GEN_FILE_TYPE(".html", "<!--"),
    PAK_GEN_FILE_TYPE(".html", "<link"),
    PAK_GEN_FILE_TYPE(".svg", "<svg "),
    PAK_GEN_FILE_TYPE(".js", "// "),
    PAK_GEN_FILE_TYPE(".js", "(function"),
    PAK_GEN_FILE_TYPE(".css", "/*"),
    PAK_GEN_FILE_TYPE(".json", "{"),
    PAK_GEN_FILE_TYPE(".gz", "\x1f\x8b")};

static const unsigned int FILE_TYPE_COUNT =
    sizeof(FILE_TYPES) / sizeof(FileType);

char *pakGetFileType(PakFile file) {
    for (unsigned int i = 0; i < FILE_TYPE_COUNT; i++)
        if (file.size > FILE_TYPES[i].size &&
            memcmp(file.buffer, FILE_TYPES[i].identifer, FILE_TYPES[i].size) ==
            0)
            return FILE_TYPES[i].type;
    return "";
}
