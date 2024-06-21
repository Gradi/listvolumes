#include <windows.h>
#include "console.h"

void ConsoleWrite(const LPCTSTR string)
{
    const HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    if (handle != INVALID_HANDLE_VALUE)
    {
        const int length = lstrlen(string);
        WriteConsole(handle, string, length, NULL, NULL);
    }
}

void ConsoleWriteError(const LPCTSTR string)
{
    const HANDLE handle = GetStdHandle(STD_ERROR_HANDLE);
    if (handle != INVALID_HANDLE_VALUE)
    {
        const int length = lstrlen(string);
        WriteConsole(handle, string, length, NULL, NULL);
    }
}

void ConsoleWriteLn(const LPCTSTR string)
{
    ConsoleWrite(string);
    ConsoleWrite(TEXT("\r\n"));
}

void ConsoleWriteErrorLn(const LPCTSTR string)
{
    ConsoleWriteError(string);
    ConsoleWriteError(TEXT("\r\n"));
}

void ConsoleWriteString(const LPCString string)
{
    ConsoleWrite(string->Ptr);
}

void ConsoleWriteStringLn(const LPCString string)
{
    ConsoleWriteString(string);
    ConsoleWrite(TEXT("\r\n"));
}

void ConsoleWriteErrorString(const LPCString string)
{
    ConsoleWriteError(string->Ptr);
}

void ConsoleWriteErrorStringLn(const LPCString string)
{
    ConsoleWriteErrorString(string);
    ConsoleWriteError(TEXT("\r\n"));
}
