#include "Error.h"

#include <Windows.h>
#include <cstdio>
#include <process.h>
#include <Main.h>

__declspec(noreturn) void FatalAssert(
const char *expr,
const char *file,
unsigned int line,
const char *function)
{
    char module_path[MAX_PATH]{};
    if (gl_hThisInstance) {
        GetModuleFileName(gl_hThisInstance, module_path, _countof(module_path));
    }
    if (!*module_path) {
        strcpy_s(module_path, "Unknown");
    }
    const char* fmt = "Module: %s\n\nExpr: %s\n\nFile: %s\n\nFunction: %s, line %d";
    int len = snprintf(NULL, 0, fmt, module_path, expr, file, function, line);
    char* buf = new char[len + 1];
    snprintf(buf,len + 1, fmt, module_path, expr, file, function, line);


    MessageBox(0, buf, "uMod Assertion Failure", MB_OK | MB_ICONERROR);

    delete[] buf;
    abort();
}
