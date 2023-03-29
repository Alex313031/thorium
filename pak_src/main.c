#if defined(_WIN32) && defined(_LGPL)
#include "main.h"
#endif
#include "pak_defs.h"
#include "pak_file.h"
#include "pak_file_io.h"
#include "pak_get_file_type.h"
#include "pak_header.h"
#include "pak_pack.h"
#include <stdio.h>

void red () {
  printf("\033[1;31m");
}

void yellow () {
  printf("\033[1;33m");
}

void cyan () {
  printf("\033[1;96m");
}

void green () {
  printf("\033[1;32m");
}

void bold () {
  printf("\033[1m");
}

void underline () {
  printf("\033[4m");
}

void reset () {
  printf("\033[0m");
}


#define PAK_VERSION_STRING  \
	"\n"  \
	"Pak v2.0.2\n\n"  \
	
#define CHROMIUM_ASCII  \
	"\n"  \
"\033[0m\033[1;34m\033[1m                .,:loool:,.              \n"  \
"\033[0m\033[1;34m\033[1m            .,coooooooooooooc,.          \n"  \
"\033[0m\033[1;34m\033[1m         .,lllllllllllllllllllll,.       \n"  \
"\033[0m\033[1;34m\033[1m        ;ccccccccccccccccccccccccc;      \n"  \
"\033[0m\033[36m\033[1m      ,\033[0m\033[1;34m\033[1mccccccccccccccccccccccccccccc.    \n"  \
"\033[0m\033[36m\033[1m     ,oo\033[0m\033[1;34m\033[1mc::::::::ok\033[0m\033[1;37m00000\033[0m\033[37mOOkkkkkkkkkkk:   \n"  \
"\033[0m\033[36m\033[1m    .ooool\033[0m\033[1;34m\033[1m;;;;:x\033[0m\033[1;37mK0\033[0m\033[1;34mkxxxxxk\033[0m\033[1;37m0X\033[0m\033[37mK0000000000.  \n"  \
"\033[0m\033[36m\033[1m    :oooool\033[0m\033[1;34m\033[1m;,;O\033[0m\033[1;37mK\033[0m\033[1;34mddddddddddd\033[0m\033[1;37mKX\033[0m\033[37m000000000d  \n"  \
"\033[0m\033[36m\033[1m    lllllool\033[0m\033[1;34m\033[1m;l\033[0m\033[1;37mN\033[0m\033[1;34mdllllllllllld\033[0m\033[1;37mN\033[0m\033[37mK000000000  \n"  \
"\033[0m\033[36m\033[1m    lllllllll\033[0m\033[1;34m\033[1mo\033[0m\033[1;37mM\033[0m\033[1;34mdccccccccccco\033[0m\033[1;37mW\033[0m\033[37mK000000000  \n"  \
"\033[0m\033[36m\033[1m    ;cllllllllX\033[0m\033[1;37mX\033[0m\033[1;34mc:::::::::c\033[0m\033[1;37m0X\033[0m\033[37m000000000d  \n"  \
"\033[0m\033[36m\033[1m    .ccccllllllO\033[0m\033[1;37mNk\033[0m\033[1;34mc;,,,;cx\033[0m\033[1;37mKK\033[0m\033[37m0000000000.  \n"  \
"\033[0m\033[36m\033[1m     .cccccclllllxO\033[0m\033[1;37mOOOO0\033[0m\033[36m\033[1mkx\033[0m\033[37mO0000000000;   \n"  \
"\033[0m\033[36m\033[1m      .:ccccccccllllllllo\033[0m\033[37mO0000000OOO,    \n"  \
"\033[0m\033[36m\033[1m        ,:ccccccccclllcd\033[0m\033[37m0000OOOOOOl.     \n"  \
"\033[0m\033[36m\033[1m          .::ccccccccc\033[0m\033[37mdOOOOOOOkx:.       \n"  \
"\033[0m\033[36m\033[1m            ..,::cccc\033[0m\033[37mxOOOkkko;.          \n"  \
"\033[0m\033[36m\033[1m               ..::\033[0m\033[37mdOkkxl:.              \n"  \
"\n"  \
"\033[1;32m            Long Live Chromium\041\n\033[0m\n"  \

#define HELP_TEXT  \
	"\n"  \
    "\033[1;32m\033[4mPak Version 2.0.1\033[0m\033[1;32m - Pack/Unpack Chromium .pak files.\033[0m\n\n"  \
    "\033[0mUsage:\033[1;96m\n\n"  \
    "-u \033[1;33m*.pak destination_path\033[0m\n"  \
    "   Unpack Chromium pak file from '*.pak' file to 'destination_path'.\033[1;96m\n\n"  \
    "-p \033[1;33mpak_index_file destination_pak_file\033[0m\n"  \
    "   Re-Pack Chromium pak file using 'pak_index_file' to 'destination_pak_file'.\n"  \
    "   'pak_index_file' would be found in the path of a previously unpacked file.\033[1;96m\n\n"  \
    "-v \033[0mPrint version info.\033[1;96m\n\n"  \
    "-h \033[0mPrint this help info.\033[1;96m\n\n"  \
    "-c \033[0mPrint some fun Chromium ASCII Art!.\n\n"  \
	"\033[1;31mNote: Existing destination files will be overwritten!\033[0m\n\n"  \
	
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



void printVersion() {
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
	
    printf(PAK_VERSION_STRING);
}

void printChromium() {
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
	
    printf(CHROMIUM_ASCII);
}

int pakUnpackPath(char *pakFilePath, char *outputPath) {
    PakFile pakFile = readFile(pakFilePath);
    if (pakFile.buffer == NULL) {
        printf("\033[1;31mError: Cannot read .pak file %s\033[0m\n", pakFilePath);
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
    printf("\033[1;32\nmUnpacked %s\033[0m", pakFilePath);
    printf("\033[1;32m to %s\033[0m\n\n", outputPath);
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
        printf("\033[1;31mError: Cannot read pak index file %s\033[0m\n", indexPath);
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
        printf("\033[1;31mError: Cannot write to %s\033[0m\n", outputFilePath2);
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
printf("\033[1;32m\nPacked %s\033[0m\n\n", outputFilePath);
    return returnCode;
}

#define PAK_FLAGS_HELP 0
#define PAK_FLAGS_UNPACK 1
#define PAK_FLAGS_PACK 2
#define PAK_FLAGS_VERSION 3
#define PAK_FLAGS_CHROMIUM_ASCII 4
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
            case 'v':
                flags = PAK_FLAGS_VERSION;
                break;
            case 'c':
                flags = PAK_FLAGS_CHROMIUM_ASCII;
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
    if (flags == PAK_FLAGS_VERSION && !process) {
        printVersion();
        return 3;
    }
    if (flags == PAK_FLAGS_HELP && !process) {
        printHelp();
        return 0;
    }
    if (flags == PAK_FLAGS_CHROMIUM_ASCII && !process) {
        printChromium();
        return 0;
    }
    if (flags == PAK_FLAGS_UNPACK) {
        return pakUnpackPath(path1, path2);
    } else if (flags == PAK_FLAGS_PACK) {
        return pakPackIndexFile(path1, path2);
    }
    return 32;
}
