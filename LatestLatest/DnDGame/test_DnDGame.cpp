#include <iostream>

#include "System.hpp"

int main(void)
{
    STARTUPINFOA si{};
    PROCESS_INFORMATION pi{};

    si.cb = sizeof(si);

    const char* exePath = "C:\\Users\\UserPc\\source\\repos\\Renderer\\x64\\Debug\\ConsoleApplication2.exe";

    BOOL success = CreateProcessA(
        exePath,      // application name
        NULL,         // command line
        NULL,         // process security
        NULL,         // thread security
        FALSE,        // inherit handles
        CREATE_NEW_CONSOLE,            // creation flags
        NULL,         // environment
        NULL,         // current directory
        &si,
        &pi
    );

    if (!success)
    {
        std::cout << "Failed to launch. Error: " << GetLastError() << "\n";
        return 1;
    }

    std::cout << "Renderer started!\n";








	System sys;
	sys.GameLoop();


    // Wait for it to finish (optional)
    WaitForSingleObject(pi.hProcess, INFINITE);

    std::cout << "Renderer finished.\n";

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

	return 0;
}




