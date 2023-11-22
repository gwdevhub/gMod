#include "Error.h"

#include <Windows.h>
#include <stdio.h>
#include <process.h>

__declspec(noreturn) void FatalAssert(
const char *expr,
const char *file,
unsigned int line,
const char *function)
{
    const char* fmt = "%s\n%s\n%s line %d";
    int len = snprintf(NULL, 0, fmt, expr, file, function, line);

    char* buf = new char[len + 1];
    snprintf(buf,len + 1, fmt, expr, file, function, line);

    MessageBox(0, "uMod Assertion", buf, MB_OK | MB_ICONERROR);

    delete[] buf;
    abort();
}
