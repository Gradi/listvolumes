#pragma once

#include <windows.h>
#include "string.h"

void ConsoleWrite(const LPCTSTR string);

void ConsoleWriteError(const LPCTSTR string);

void ConsoleWriteLn(const LPCTSTR string);

void ConsoleWriteErrorLn(const LPCTSTR string);

void ConsoleWriteString(const LPCString string);

void ConsoleWriteStringLn(const LPCString string);

void ConsoleWriteErrorString(const LPCString string);

void ConsoleWriteErrorStringLn(const LPCString string);
