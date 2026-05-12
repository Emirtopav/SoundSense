#include <windows.h>
#include <string>
#include <iostream>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // Path to the real executable inside the bin folder
    std::string exePath = "bin\\SoundSenseCpp.exe";
    
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    // Create the process with the runtime folder as the working directory
    if (!CreateProcessA(
        NULL,                   // No module name (use command line)
        (char*)exePath.c_str(), // Command line
        NULL,                   // Process handle not inheritable
        NULL,                   // Thread handle not inheritable
        FALSE,                  // Set handle inheritance to FALSE
        0,                      // No creation flags
        NULL,                   // Use parent's environment block
        NULL,                   // Use parent's working directory (Root)
        &si,                    // Pointer to STARTUPINFO structure
        &pi)                    // Pointer to PROCESS_INFORMATION structure
    ) {
        MessageBoxA(NULL, "Failed to launch SoundSense engine from bin folder.", "Launch Error", MB_OK | MB_ICONERROR);
        return 1;
    }

    // Close process and thread handles
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return 0;
}
