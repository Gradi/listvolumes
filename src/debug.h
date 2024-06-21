#pragma once

#include <windows.h>

[[noreturn]] void Panic(const LPCTSTR message);

void PrintLastErrorCode(const DWORD lastErrorCode);
