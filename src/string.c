#include <windows.h>
#include "string.h"
#include "debug.h"

String StringNew(SIZE_T charCount)
{
    const SIZE_T bufferSize = sizeof(TCHAR) * (charCount + 1);

    LPVOID ptr = VirtualAlloc(NULL, bufferSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    if (ptr == NULL)
    {
        Panic(TEXT("StringNew: Can't allocate memory."));
    }
    ZeroMemory(ptr, bufferSize);

    return (String) {
        .Ptr = (LPTSTR) ptr,
        .BufferSize = bufferSize,
        .CharCount = charCount
    };
}

String StringFromLPCTSTR(const LPCTSTR str)
{
    const int charCount = lstrlen(str);
    const String result = StringNew(charCount);
    CopyMemory(result.Ptr, str, result.BufferSize);
    return result;
}

void StringDelete(const LPCString string)
{
    if (VirtualFree(string->Ptr, 0, MEM_RELEASE) == 0)
    {
        Panic(TEXT("StringDelete: Can't free memory."));
    }
}

void StringZero(const LPCString string)
{
    ZeroMemory(string->Ptr, string->BufferSize);
}

int StringLength(const LPCString string)
{
    return lstrlen(string->Ptr);
}

String StringCopy(const LPCString string)
{
    LPVOID ptr = VirtualAlloc(NULL, string->BufferSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    if (ptr == NULL)
    {
        Panic(TEXT("StringCopy: Can't allocate memory."));
    }
    ZeroMemory(ptr, string->BufferSize);
    CopyMemory(ptr, string->Ptr, string->BufferSize);

    return (String) {
        .Ptr = (LPTSTR)ptr,
        .BufferSize = string->BufferSize,
        .CharCount = string->CharCount
    };
}

bool StringStartsWithLPCTSTR(const LPCString string, const LPCTSTR prefix)
{
    const int leftLength = StringLength(string);
    const int rightLength = lstrlen(prefix);
    if (rightLength > leftLength)
    {
        return false;
    }

    bool result = true;
    for(int i = 0; i < rightLength; ++i)
    {
        result &= string->Ptr[i] == prefix[i];
    }
    return result;
}

bool StringEndsWithLPCTSTR(const LPCString string, const LPCTSTR suffix)
{
    const int leftLength = StringLength(string);
    const int rightLength = lstrlen(suffix);
    if (rightLength > leftLength)
    {
        return false;
    }

    bool result = true;
    for(int i = 0; i < rightLength; ++i)
    {
        result &= string->Ptr[leftLength - 1 - i] == suffix[rightLength - 1 - i];
    }
    return result;
}

String StringTrimStartLPCTSTR(const LPCString string, const LPCTSTR prefix)
{
    if (!StringStartsWithLPCTSTR(string, prefix))
    {
        return StringCopy(string);
    }

    const int prefixLength = lstrlen(prefix);
    const String result = StringNew(StringLength(string) - prefixLength);
    CopyMemory(result.Ptr, string->Ptr + prefixLength, result.BufferSize - sizeof(TCHAR));
    return result;
}

String StringTrimEndLPCTSTR(const LPCString string, const LPCTSTR suffix)
{
    if (!StringEndsWithLPCTSTR(string, suffix))
    {
        return StringCopy(string);
    }

    const int suffixLength = lstrlen(suffix);
    const String result = StringNew(StringLength(string) - suffixLength);
    CopyMemory(result.Ptr, string->Ptr, result.BufferSize - sizeof(TCHAR));
    return result;
}
