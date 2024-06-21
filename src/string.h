#pragma once

#include <windows.h>

typedef struct {
    LPTSTR Ptr;
    SIZE_T BufferSize;
    SIZE_T CharCount;
} String, *LPString;

typedef const String CString;
typedef CString *LPCString;


String StringNew(const SIZE_T bufferSize);

String StringFromLPCTSTR(const LPCTSTR str);

void StringDelete(const LPCString string);

void StringZero(const LPCString string);

int StringLength(const LPCString string);

String StringCopy(const LPCString string);

bool StringStartsWithLPCTSTR(const LPCString string, const LPCTSTR prefix);

bool StringEndsWithLPCTSTR(const LPCString string, const LPCTSTR suffix);

String StringTrimStartLPCTSTR(const LPCString string, const LPCTSTR prefix);

String StringTrimEndLPCTSTR(const LPCString string, const LPCTSTR suffix);
