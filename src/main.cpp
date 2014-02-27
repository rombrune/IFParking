#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/msg.h>
#include <signal.h>

#include "Outils.h"
#include "config.h"
#include "Heure.h"
#include "KeyboardManagement.h"
#include "Entrance.h"

static pid_t KeyboardManagementPid;
static pid_t HourPid;
static pid_t EntrancePid;

void initResources ( )
{
	// TODO: check that permissions are as tight as possible
	
	// Create the mailbox (KeyboardManagement => Entrances)
	int key = ftok(EXEC_NAME, KEY);
	int mailboxId = msgget ( key, (IPC_CREAT|IPC_EXCL|0600) );
}

void endSync ( pid_t pid )
{
	// The call could be interrupted by a received signal.
	// But since we really want to wait, we relauch it every
	// time that happens.
	while ( -1 == waitpid ( pid, NULL, 0 ) )
	{
		// Empty
	}
}

void freeResources ( )
{
	// Free the mailbox
	int key = ftok(EXEC_NAME, KEY);
	int mailboxId = msgget ( key, IPC_EXCL );
	msgctl ( mailboxId, IPC_RMID, 0 );
}

int main ( int argc, const char * argv[] )
{
	// By default, mask SIGUSR2 signal
	struct sigaction action;
	sigemptyset ( &action.sa_mask );
	action.sa_handler = SIG_IGN;
	action.sa_flags = 0;
	sigaction ( SIGUSR2, &action, NULL );


	// ---------- INITIALIZATION
	InitialiserApplication( XTERM );
	initResources ( );

	// ---------- RUN
	// We should test the value returned by fork, there could be
	// an error when forking
	if ( (EntrancePid = fork()) == 0 )
	{
		Entrance ( PROF_BLAISE_PASCAL );
	}
	else
	{
		HourPid = ActiverHeure ( );

		if ( (KeyboardManagementPid = fork()) == 0 )
		{
			KeyboardManagement ( );
		}
		else
		{
			// Wait for the user to quit the application
			endSync ( KeyboardManagementPid );

			// ---------- DESTRUCTION
			kill ( HourPid, SIGUSR2 );
			endSync ( HourPid );

			kill ( EntrancePid, SIGUSR2 );
			endSync ( EntrancePid );

			freeResources ( );
			TerminerApplication ( );
			exit ( 0 );
		}
	}
}

