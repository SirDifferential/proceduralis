#include "application.hpp"

#ifdef _WIN32
#include <Windows.h>
#endif

int main(int argc, char** argv)
{
    app.readConfig();
    int retcode = app.run();

    if (retcode == 0)
    {
        // Fix crash-on-exit bug on Windows by calling TerminateProcess manually
        #ifdef _WIN32
            TerminateProcess(GetCurrentProcess(), EXIT_SUCCESS);
        #endif
        return 0;
    }
    else
        return retcode;

}
