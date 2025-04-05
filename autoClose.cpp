#include <windows.h>
#include <tlhelp32.h>
#include <tchar.h>
#include <iostream>
#include <string>
#include <thread>
using namespace std;
DWORD FindProcessId(const wstring& processName) {
    PROCESSENTRY32 processInfo;
    processInfo.dwSize = sizeof(processInfo);

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
    DWORD pid = 0;

    if (snapshot == INVALID_HANDLE_VALUE) return 0;

    if (Process32First(snapshot, &processInfo)) {
        do {
            if (!_wcsicmp(processInfo.szExeFile, processName.c_str())) {
                pid = processInfo.th32ProcessID;
                break;
            }
        } while (Process32Next(snapshot, &processInfo));
    }

    CloseHandle(snapshot);
    return pid;
}

bool KillProcessById(DWORD pid) {
    HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pid);
    if (hProcess == NULL) {
        return false;
    }

    BOOL result = TerminateProcess(hProcess, 0);
    CloseHandle(hProcess);
    return result;
}

int main() {
    wcout << L"Enter the name of the process to auto-close (for example, notepad.exe):";
    wstring procName;
    getline(wcin, procName);

    wcout << L"I'm starting to monitor the process: " << procName << L"\n";

    while (true) {
        DWORD pid = FindProcessId(procName);
        if (pid != 0) {
            wcout << L"[!] Process detected" << procName << L" with a PID" << pid << L" - trying to close...\n";
            if (KillProcessById(pid)) {
                wcout << L"[+] The process is closed.\n";
            }
            else {
                wcout << L"[X] Failed to close the process.\n";
            }
        }

        this_thread::sleep_for(chrono::seconds(2));
    }

    return 0;
}