#include "debug.h"
#include "console.h"
#include <windows.h>

[[noreturn]] void Panic(const LPCTSTR message)
{
    const DWORD lastError = GetLastError();

    ConsoleWriteError(TEXT("\r\n"));
    ConsoleWriteErrorLn(TEXT("*** PANIC ***"));

    ConsoleWriteErrorLn(message);
    PrintLastErrorCode(lastError);

    ConsoleWriteErrorLn(TEXT("*** PANIC END ***"));

    FatalExit(0xffffffff);
    while (1) ;
}

void PrintLastErrorCode(const DWORD lastErrorCode)
{
    TCHAR lastErrorNumericValue[128] = {0};
    TCHAR lastErrorMsg[1024] = {0};

    wsprintf(lastErrorNumericValue, TEXT("0x%lx"), lastErrorCode);

    FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,
                  NULL,
                  lastErrorCode,
                  MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL),
                  lastErrorMsg,
                  ARRAYSIZE(lastErrorMsg) - 1,
                  NULL);

    ConsoleWriteError(TEXT("GetLastError: "));
    ConsoleWriteErrorLn(lastErrorNumericValue);
    ConsoleWriteError(TEXT("GetLastError(message): "));
    ConsoleWriteError(lastErrorMsg);
}
