#include "console.h"
#include "debug.h"
#include "string.h"
#include <windows.h>

bool AssertMemoryFilledWith(const LPVOID left, SIZE_T length, const BYTE expectedValue);

/* Just check console prints something & application doesn't crash. */
void TestConsole()
{
    ConsoleWrite(TEXT("ConsoleWrite works?\r\n"));
    ConsoleWriteError(TEXT("ConsoleWriteError works?\r\n"));
    ConsoleWriteLn(TEXT("ConsoleWriteLn works?"));
    ConsoleWriteErrorLn(TEXT("ConsoleWriteErrorLn works?"));
}

/* Tests string lib */
bool TestString()
{
    ConsoleWriteLn(TEXT("StringNew just works"));
    String str = StringNew(32);



    ConsoleWriteLn(TEXT("StringFromLPCTSTR"));
    const LPCTSTR expected = TEXT("Hello, World!");
    String str2 = StringFromLPCTSTR(expected);
    if (lstrcmp(str2.Ptr, expected) != 0)
    {
        ConsoleWriteLn(TEXT("returned different str."));
        return false;
    }



    ConsoleWriteLn(TEXT("StringDelete just works"));
    StringDelete(&str);



    ConsoleWriteLn(TEXT("StringZero actually zeros"));
    String str3 = StringNew(100);
    FillMemory(str3.Ptr, str3.BufferSize, 0xff);
    if (!AssertMemoryFilledWith(str3.Ptr, str.BufferSize, 0xff))
    {
        ConsoleWriteLn(TEXT("Fill memorty with 0xff doesnt filled memory with 0xff"));
        return false;
    }
    StringZero(&str3);
    if(!AssertMemoryFilledWith(str3.Ptr, str3.BufferSize, 0))
    {
        ConsoleWriteLn(TEXT("didn't clean fill string with zeros."));
        return false;
    }



    ConsoleWriteLn(TEXT("StringLength returns valid length"));
    String str4 = StringNew(101);
    if (StringLength(&str4) != 0)
    {
        ConsoleWriteLn(TEXT("should have returned zero for newly created string"));
        return false;
    }
    FillMemory(str4.Ptr, str4.BufferSize - sizeof(TCHAR), 0xff);
    if(StringLength(&str4) != 101)
    {
        ConsoleWriteLn(TEXT("should have returned valid string length."));
        return false;
    }



    ConsoleWriteLn(TEXT("StringCopy returns copy of a string"));
    String str5 = StringNew(100);
    FillMemory(str5.Ptr, str5.BufferSize, 0xab);
    const String copy = StringCopy(&str5);
    if (str5.Ptr == copy.Ptr)
    {
        ConsoleWriteLn(TEXT("returned same pointer"));
        return false;
    }
    if (str5.BufferSize != copy.BufferSize)
    {
        ConsoleWriteLn(TEXT("returned copy with different buffer size"));
        return false;
    }
    if(str5.CharCount != copy.CharCount)
    {
        ConsoleWriteLn(TEXT("returned copy with different char count"));
        return false;
    }
    if (!AssertMemoryFilledWith(copy.Ptr, copy.BufferSize, 0xab))
    {
        ConsoleWriteLn(TEXT("returned buffer with different content."));
        return false;
    }



    ConsoleWriteLn(TEXT("StringStartsWithLPCTSTR"));
    String str6 = StringFromLPCTSTR(TEXT("Hello LOL TESTS"));
    if (!StringStartsWithLPCTSTR(&str6, TEXT("Hello ")))
    {
        ConsoleWriteLn(TEXT("should have returned true."));
        return false;
    }
    if(StringStartsWithLPCTSTR(&str6, TEXT("Nope")))
    {
        ConsoleWrite(TEXT("should have returned false."));
        return false;
    }



    ConsoleWriteLn(TEXT("StringEndsWithLPCTSTR"));
    if(!StringEndsWithLPCTSTR(&str6, TEXT(" TESTS")))
    {
        ConsoleWriteLn(TEXT("should have returned true."));
        return false;
    }
    if (StringEndsWithLPCTSTR(&str6, TEXT(" TeSTS")))
    {
        ConsoleWriteLn(TEXT("should have returned false."));
        return false;
    }



    ConsoleWriteLn(TEXT("StringTrimStartLPCTSTR"));
    String result1 = StringTrimStartLPCTSTR(&str6, TEXT("Hello "));
    if(lstrcmp(result1.Ptr, TEXT("LOL TESTS")) != 0)
    {
        ConsoleWriteLn(TEXT("returned bad string."));
        return false;
    }



    ConsoleWriteLn(TEXT("StringTrimEndLPCTSTR"));
    String result2 = StringTrimEndLPCTSTR(&str6, TEXT("L TESTS"));
    if(lstrcmp(result2.Ptr, TEXT("Hello LO")) != 0)
    {
        ConsoleWriteLn(TEXT("returned bad string."));
        ConsoleWriteStringLn(&result2);
        return false;
    }

    return true;
}

/* Checks just print & application doesn't crash. */
void TestDebug()
{
    ConsoleWriteLn(TEXT("Going to print last error code of value 0"));
    PrintLastErrorCode(0);
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, PSTR, int)
{
    TestConsole();
    ConsoleWrite(TEXT("\r\n\r\n"));

    if (!TestString())
    {
        ConsoleWriteLn(TEXT("String tests failed."));
        return 1;
    }

    ConsoleWrite(TEXT("\r\n\r\n"));
    TestDebug();

    ConsoleWriteLn(TEXT("Tests OK"));
    return 0;
}


bool AssertMemoryFilledWith(const LPVOID left, SIZE_T length, const BYTE expectedValue)
{
    const LPBYTE byteArray = (LPBYTE)left;

    bool result = true;
    for(SIZE_T i = 0; i < length; ++i)
    {
        result &= byteArray[i] == expectedValue;
    }

    return result;
}
