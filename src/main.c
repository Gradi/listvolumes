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

Status QueryDevice(const LPCString volumeName, const LPString resultDeviceName)
{
    /*
    *  QueryDosDevice doesn't support strings starting with \\?\
    *  and ending with trailing slash
    */
    const String vlm0 = StringTrimEndLPCTSTR(volumeName, TEXT("\\"));
    const String vlm1 = StringTrimStartLPCTSTR(&vlm0, TEXT("\\\\?\\"));

    String deviceName = StringNew(MAX_PATH);
    while (!QueryDosDevice(vlm1.Ptr, deviceName.Ptr, deviceName.CharCount))
    {
        const DWORD errorCode = GetLastError();
        if (errorCode == ERROR_INSUFFICIENT_BUFFER)
        {
            String newDeviceName = StringNew(deviceName.CharCount * 2);
            StringDelete(&deviceName);
            deviceName = newDeviceName;
            continue;
        }

        ConsoleWriteErrorLn(TEXT("QueryDevice: QueryDosDevice failed."));
        PrintLastErrorCode(errorCode);

        StringDelete(&vlm0);
        StringDelete(&vlm1);
        StringDelete(&deviceName);
        return STATUS_ERROR;
    }

    (*resultDeviceName) = deviceName;
    StringDelete(&vlm0);
    StringDelete(&vlm1);
    return STATUS_OK;
 }

Status QueryPaths(const LPCString volumeName, const LPString resultPaths)
{
    String pathNames = StringNew(MAX_PATH);
    DWORD returnLength = 0;

    while (!GetVolumePathNamesForVolumeName(volumeName->Ptr, pathNames.Ptr, pathNames.CharCount, &returnLength))
    {
        const DWORD errorCode = GetLastError();
        if(errorCode == ERROR_MORE_DATA)
        {
            StringDelete(&pathNames);
            pathNames = StringNew(returnLength);
            continue;
        }

        ConsoleWriteErrorLn(TEXT("QueryPaths: GetVolumePathNamesForVolumeName failed."));
        PrintLastErrorCode(errorCode);
        StringDelete(&pathNames);
        return STATUS_ERROR;
    }

    (*resultPaths) = pathNames;
    return STATUS_OK;
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

void PrintVolumeInfo(const LPCString deviceName, const LPCString paths)
{
    ConsoleWriteStringLn(deviceName);

    for(LPCTSTR path = paths->Ptr; path[0] != TEXT('\0'); path += lstrlen(path) + 1)
    {
        const LPCTSTR driveType = DriveType2String(GetDriveType(path));
        VolumeInfo volumeInfo;
        if(QueryVolumeInfo(path, &volumeInfo) == STATUS_ERROR)
        {
            volumeInfo = (VolumeInfo) {
                .VolumeName = StringNew(0),
                .Filesystem = StringNew(0),
                .TotalSize = StringNew(0),
                .FreeSize = StringNew(0),
                .UsedSize = StringNew(0)
            };
        }

        String msg = StringNew(1024);
        wsprintf(msg.Ptr, TEXT("\t%s -- type \"%s\", name \"%s\", fs \"%s\", total \"%s\", free \"%s\", used \"%s\""),
            path, driveType,
            volumeInfo.VolumeName.Ptr,
            volumeInfo.Filesystem.Ptr,
            volumeInfo.TotalSize.Ptr,
            volumeInfo.FreeSize.Ptr,
            volumeInfo.UsedSize.Ptr);

        ConsoleWriteStringLn(&msg);
        StringDelete(&volumeInfo.VolumeName);
        StringDelete(&volumeInfo.Filesystem);
        StringDelete(&volumeInfo.TotalSize);
        StringDelete(&volumeInfo.FreeSize);
        StringDelete(&volumeInfo.UsedSize);
        StringDelete(&msg);
    }
}

Status IterateVolumes(const HANDLE handle, const LPCString volumeName)
{
    String deviceName;
    String paths;
    if (QueryDevice(volumeName, &deviceName) == STATUS_ERROR)
    {
        return STATUS_ERROR;
    }
    if (QueryPaths(volumeName, &paths) == STATUS_ERROR)
    {
        return STATUS_ERROR;
    }

    PrintVolumeInfo(&deviceName, &paths);
    StringDelete(&deviceName);
    StringDelete(&paths);

    StringZero(volumeName);
    if(!FindNextVolume(handle, volumeName->Ptr, volumeName->CharCount))
    {
        const DWORD lastError = GetLastError();
        if (lastError == ERROR_NO_MORE_FILES)
        {
            return STATUS_OK;
        }
        ConsoleWriteErrorLn(TEXT("IterateVolumes: FindNextVolume failed."));
        PrintLastErrorCode(lastError);
        return STATUS_ERROR;
    }
    if (StringLength(volumeName) == 0)
    {
        ConsoleWriteErrorLn(TEXT("IterateVolumes: FindNextVolume returned empty string."));
        return STATUS_ERROR;
    }

    return IterateVolumes(handle, volumeName);
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, PSTR, int)
{
    const String volumeName = StringNew(MAX_PATH);
    const HANDLE handle = FindFirstVolume(volumeName.Ptr, volumeName.CharCount);
    if (handle == INVALID_HANDLE_VALUE)
    {
        const DWORD lastError = GetLastError();
        ConsoleWriteErrorLn(TEXT("Can't get handle for volume iteration."));
        PrintLastErrorCode(lastError);
        return 1;
    }

    const Status result = IterateVolumes(handle, &volumeName);
    FindVolumeClose(handle);
    StringDelete(&volumeName);
    return result;
}
