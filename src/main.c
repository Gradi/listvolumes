#include <windows.h>
#include "debug.h"
#include "console.h"
#include "string.h"

typedef int Status;

#define STATUS_ERROR 1
#define STATUS_OK    0

typedef struct {
    String VolumeName;
    String Filesystem;
    String TotalSize;
    String FreeSize;
    String UsedSize;
} VolumeInfo;

String Bytes2HumanString(const DWORD64 bytes)
{
    const DWORD64 KB = 1024U;
    const DWORD64 MB = KB * 1024U;
    const DWORD64 GB = MB * 1024U;
    const DWORD64 TB = GB * 1024U;

    double result = 0.0;
    LPCTSTR suffix = TEXT("");

    if (bytes >= TB)
    {
        result = ((double)bytes) / ((double)TB);
        suffix = TEXT("TB");
    }
    else if (bytes >= GB)
    {
        result = ((double)bytes) / ((double)GB);
        suffix = TEXT("GB");
    }
    else if (bytes >= MB)
    {
        result = ((double)bytes) / ((double)MB);
        suffix = TEXT("MB");
    }
    else if (bytes >= KB)
    {
        result = ((double)bytes) / ((double)KB);
        suffix = TEXT("KB");
    }
    else
    {
        result = (double)bytes;
        suffix = TEXT("bytes");
    }

    String str = StringNew(64);
    wsprintf(str.Ptr, TEXT("%lu %s"), (DWORD64)result, suffix);
    return str;
}


LPCTSTR DriveType2String(const UINT deviceType)
{
    switch (deviceType)
    {
        case DRIVE_UNKNOWN:
            return TEXT("DRIVE_UNKNOWN");
        case DRIVE_NO_ROOT_DIR:
            return TEXT("DRIVE_NO_ROOT_DIR");
        case DRIVE_REMOVABLE:
            return TEXT("removable");
        case DRIVE_FIXED:
            return TEXT("fixed");
        case DRIVE_REMOTE:
            return TEXT("remote");
        case DRIVE_CDROM:
            return TEXT("cdrom");
        case DRIVE_RAMDISK:
            return TEXT("ramdisk");
        default:
            return TEXT("unknown");
    }
}

Status QueryVolumeInfo(const LPCTSTR path, VolumeInfo * const result)
{
    const String volumeName = StringNew(MAX_PATH + 1);
    const String filesystem = StringNew(MAX_PATH + 1);
    SetErrorMode(SEM_FAILCRITICALERRORS);

    if(!GetVolumeInformation(path, volumeName.Ptr, volumeName.CharCount,
                             NULL, NULL, NULL, filesystem.Ptr, filesystem.CharCount))
    {
        const DWORD lastError = GetLastError();
        ConsoleWriteErrorLn(TEXT("QueryVolumeInfo: GetVolumeInformation failed."));
        PrintLastErrorCode(lastError);

        StringDelete(&volumeName);
        StringDelete(&filesystem);
        return STATUS_ERROR;
    }

    ULARGE_INTEGER totalBytes = {0};
    ULARGE_INTEGER freeBytes = {0};
    ULARGE_INTEGER availableBytes = {0};

    if(!GetDiskFreeSpaceEx(path, &availableBytes, &totalBytes, &freeBytes))
    {
        const DWORD lastError = GetLastError();
        ConsoleWriteErrorLn(TEXT("QueryVolumeInfo: GetDiskFreeSpaceEx failed."));
        PrintLastErrorCode(lastError);
        return STATUS_ERROR;
    }

    (*result) = (VolumeInfo) {
        .VolumeName = volumeName,
        .Filesystem = filesystem,
        .TotalSize = Bytes2HumanString(totalBytes.QuadPart),
        .FreeSize = Bytes2HumanString(freeBytes.QuadPart),
        .UsedSize = Bytes2HumanString(totalBytes.QuadPart - freeBytes.QuadPart)
    };
    return STATUS_OK;
}

void PrintDiskInfo(const LPCTSTR diskLetter)
{
    VolumeInfo volumeInfo = {0};
    if (QueryVolumeInfo(diskLetter, &volumeInfo) == STATUS_ERROR)
    {
        return;
    }

    const LPCTSTR diskType = DriveType2String(GetDriveType(diskLetter));
    const String msg = StringNew(1024);
    wsprintf(msg.Ptr, TEXT("%s -- type \"%s\", name \"%s\", fs \"%s\", total \"%s\", free \"%s\", used \"%s\""),
        diskLetter,
        diskType,
        volumeInfo.VolumeName.Ptr,
        volumeInfo.Filesystem.Ptr,
        volumeInfo.TotalSize.Ptr,
        volumeInfo.FreeSize.Ptr,
        volumeInfo.UsedSize.Ptr);

    ConsoleWriteStringLn(&msg);

    StringDelete(&msg);
    StringDelete(&volumeInfo.VolumeName);
    StringDelete(&volumeInfo.Filesystem);
    StringDelete(&volumeInfo.TotalSize);
    StringDelete(&volumeInfo.FreeSize);
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, PSTR, int)
{
    String diskDrives = StringNew(128);
    while(true)
    {
        const DWORD actualSize = GetLogicalDriveStrings(diskDrives.CharCount, diskDrives.Ptr);
        if (!actualSize)
        {
            const DWORD lastError = GetLastError();
            ConsoleWriteErrorLn(TEXT("WinMain: GetLogicalDriveStrings failed."));
            PrintLastErrorCode(lastError);
            StringDelete(&diskDrives);
            return 1;
        }
        if (actualSize > diskDrives.CharCount)
        {
            StringDelete(&diskDrives);
            diskDrives = StringNew(actualSize);
        }
        else
        {
            break;
        }
    }

    for(LPCTSTR name = diskDrives.Ptr; name[0] != TEXT('\0'); name += lstrlen(name) + 1)
    {
        PrintDiskInfo(name);
    }

    StringDelete(&diskDrives);
    return 0;
}
