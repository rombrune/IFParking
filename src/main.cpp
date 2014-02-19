#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "Outils.h"
#include "KeyboardManagement.h"

static pid_t KeyboardManagementPID;
int main(int argc, const char * argv[])
{

    std::cout << "Hello, World!\n";



    InitialiserApplication( XTERM );

    if (KeyboardManagementPID = fork() == 0)
    {
        KeyboardManagement ( );
        exit(0);
    }
    else
    {
        waitpid(KeyboardManagementPID, NULL, 0);
        TerminerApplication();
    }


    return 0;
}

