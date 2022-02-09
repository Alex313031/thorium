#if defined(_WIN32) && defined(_LGPL)
#include "main.h"
#endif
#include "pak_defs.h"
#include "pak_file.h"
#include "pak_file_io.h"
#include "pak_get_file_type.h"
#include "pak_header.h"
#include "pak_pack.h"

#define HELP_TEXT                                                              \
    "Pack/Unpack chrome pak file.\n\n"                                         \
    "%s -u pak_file destination_path\n"                                        \
    "Unpack chrome pak file at pak_file to destination_path.\n\n"              \
    "%s -p pak_index_file destination_pak_file\n"                              \
    "Pack chrome pak file using pak_index_file to destination_pak_file.\n"     \
    "pak_index_file would be found in unpacked path.\n"                        \
    "\nNote: existing destination files would be overwritten\n"

void printHelp() {
    // get self path
    char selfName[PATH_MAX];
#ifdef _WIN32
    GetModuleFileName(NULL, selfName, PATH_MAX);
    // get file name from path
    const char *ptr = strrchr(selfName, '\\');
#else
    int ret = readlink("/proc/self/exe", selfName, sizeof(selfName) - 1);
    if (ret == -1)
        strcpy(selfName, "pak");
    else
        selfName[ret] = 0;
    // get file name from path
    const char *ptr = strrchr(selfName, '/');
#endif

    if (ptr != NULL)
        strcpy(selfName, ptr + 1);

    printf(HELP_TEXT, selfName, selfName);
}

int pakUnpackPath(char *pakFilePath, char *outputPath) {
    PakFile pakFile = readFile(pakFilePath);
    if (pakFile.buffer == NULL) {
        printf("Error: cannot read pak file %s", pakFilePath);
        return 1;
    }
    MyPakHeader myHeader;
    if (!pakParseHeader(pakFile.buffer, &myHeader)) {
        return 2;
    }

    if (!pakCheckFormat(pakFile.buffer, pakFile.size)) {
        return 3;
    }

    if (!pakUnpack(pakFile.buffer, outputPath)) {
        freeFile(pakFile);
        return 4;
    }
    freeFile(pakFile);
    return 0;
}

int pakPackIndexFile(char *indexPath, char *outputFilePath) {
    int returnCode = 0;
    PakFile pakPackedFile = NULL_File;
    PakFile pakIndexFile = NULL_File;
    char *filesPath = NULL;
    bool freeFilesPath = false;
    char *outputFilePath2 = NULL;
    char *index = strrchr(indexPath, '\\');
    if (index == NULL) {
        index = strrchr(indexPath, '/');
    }
    if (index != NULL && index > indexPath) {
        filesPath = calloc(index - indexPath + 2, sizeof(char));
        if (filesPath == NULL) {
            returnCode = 5;
            goto PAK_PACK_INDEX_END;
        }
        strncpy(filesPath, indexPath, index - indexPath + 1);
        freeFilesPath = true;
    } else {
        filesPath = "";
    }

    pakIndexFile = readFile(indexPath);
    if (pakIndexFile.buffer == NULL) {
        printf("Error: cannot read file %s", indexPath);
        returnCode = 6;
        goto PAK_PACK_INDEX_END;
    }

    // workaround outputFilePath="" after pakPack()
    outputFilePath2 = calloc(strlen(outputFilePath) + 1, sizeof(char));
    if (outputFilePath2 == NULL) {
        returnCode = 7;
        goto PAK_PACK_INDEX_END;
    }
    strcpy(outputFilePath2, outputFilePath);

    pakPackedFile = pakPack(pakIndexFile, filesPath);
    if (pakPackedFile.buffer == NULL) {
        returnCode = 8;
        goto PAK_PACK_INDEX_END;
    }
    if (!writeFile(outputFilePath2, pakPackedFile)) {
        printf("Error: cannot write to %s", outputFilePath2);
        returnCode = 9;
        goto PAK_PACK_INDEX_END;
    }

PAK_PACK_INDEX_END:
    if (pakIndexFile.buffer != NULL)
        freeFile(pakIndexFile);
    if (pakPackedFile.buffer != NULL)
        freeFile(pakPackedFile);
    if (outputFilePath2 != NULL)
        free(outputFilePath2);
    if (freeFilesPath && filesPath != NULL)
        free(filesPath);
    return returnCode;
}

#define PAK_FLAGS_HELP 0
#define PAK_FLAGS_UNPACK 1
#define PAK_FLAGS_PACK 2
int main(int argc, char *argv[]) {
    uint32_t flags = 0;
    bool process = false;
    char path1[PATH_MAX];
    memset(path1, 0, PATH_MAX);
    char path2[PATH_MAX];
    memset(path2, 0, PATH_MAX);
    for (int i = 1; i < argc; i++) {
        char *arg = argv[i];
        bool isConfig = false;
        if (*arg != '\0' && (*arg == '/' || *arg == '-')) {
            arg++;
            isConfig = true;
        }
        if (isConfig) {
            switch (*arg) {
            case 'h':
                flags = PAK_FLAGS_HELP;
                break;
            case 'a':
            case 'p':
                flags = PAK_FLAGS_PACK;
                break;
            case 'u':
            case 'e':
            case 'x':
                flags = PAK_FLAGS_UNPACK;
                break;
            }
        }
        if ((flags == PAK_FLAGS_UNPACK || flags == PAK_FLAGS_PACK) &&
            argc - i > 2) {
            strcpy(path1, argv[i + 1]);
            strcpy(path2, argv[i + 2]);
            process = true;
            break;
        }
    }
    if (flags == PAK_FLAGS_HELP || !process) {
        printHelp();
        return 0;
    }
    if (flags == PAK_FLAGS_UNPACK) {
        return pakUnpackPath(path1, path2);
    } else if (flags == PAK_FLAGS_PACK) {
        return pakPackIndexFile(path1, path2);
    }
    return 32;
}
