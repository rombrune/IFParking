#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

#include "Outils.h"
#include "Heure.h"
#include "KeyboardManagement.h"

static pid_t KeyboardManagementPID;
static pid_t HourPID;
int main(int argc, const char * argv[])
{

    std::cout << "Hello, World!\n";
//////////////////////////////////////////////////////////////INITIALISATION
    InitialiserApplication( XTERM );

//////////////////////////////////////////////////////////////RUN
    // We should test the value returned by the fork, there could be
    // an error when forking
    if (KeyboardManagementPID = fork() == 0)
    {
        KeyboardManagement ( );

    }
    else
    {
        HourPID = ActiverHeure();

        waitpid(KeyboardManagementPID, NULL, 0);
        kill (HourPID, SIGUSR2);
        waitpid(HourPID, NULL, 0);
        TerminerApplication();
        exit(0);

    }


    return 0;
}

