// Small alternative startfile for main()
// Build with gcc perms:
// -nostartfiles
#pragma once
#ifndef STARTFILE_MAIN_H
#define STARTFILE_MAIN_H
#include <windows.h>

#ifdef UNICODE
#define wmain(argc, argv) _wmain(argc, argv)
int wmain(int argc, wchar_t *argv[]);
#else
#include "commandlinetoargva.h"
// workaround undefined reference to `_pei386_runtime_relocator'
#define main(argc, argv) _main(argc, argv)
int main(int argc, char *argv[]);
#endif
int __cdecl mainCRTStartup() {
  int __arg_c;
#ifdef UNICODE
  wchar_t **__arg_v = CommandLineToArgvW(GetCommandLineW(), &__arg_c);
  const int exitCode = wmain(__arg_c, __arg_v);
#else
  char **__arg_v = CommandLineToArgvA(GetCommandLineA(), &__arg_c);
  const int exitCode = main(__arg_c, __arg_v);
#endif
  LocalFree(__arg_v);
  ExitProcess((UINT) exitCode);
  return exitCode;
}
#endif
