/*************************************************************************
							main
							-------------------

*************************************************************************/

//---------- Réalisation de la tâche <main> (fichier main.cpp) -----------

/////////////////////////////////////////////////////////////////  INCLUDE
//-------------------------------------------------------- Include système
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/msg.h>
#include <signal.h>
//------------------------------------------------------ Include personnel
#include "Outils.h"

#include "config.h"
#include "Heure.h"
#include "KeyboardManagement.h"
#include "Entrance.h"
///////////////////////////////////////////////////////////////////  PRIVE
//------------------------------------------------------------- Constantes

//------------------------------------------------------------------ Types

//---------------------------------------------------- Variables statiques

//------------------------------------------------------ Fonctions privées
static void ack ( int signalNumber )
// Mode d'emploi :
// Acknowledges the death of every child process
{
	pid_t pid;
	do
	{
		pid = waitpid ( -1, NULL, WNOHANG );
	} while ( pid == 1 );
} // Fin de ack

static void init ( )
{
	// By default, mask SIGUSR2 signal
	struct sigaction action;
	sigemptyset ( &action.sa_mask );
	action.sa_handler = SIG_IGN;
	action.sa_flags = 0;
	sigaction ( SIGUSR2, &action, NULL );

	// TODO: check that permissions are as tight as possible
	
	// Create the mailbox (KeyboardManagement => Entrances)
	int key = ftok(EXEC_NAME, KEY);
	int mailboxId = msgget ( key, (IPC_CREAT|IPC_EXCL|0600) );
} // Find de init

static void destroy ( )
{
	// Free the mailbox
	int key = ftok(EXEC_NAME, KEY);
	int mailboxId = msgget ( key, IPC_EXCL );
	msgctl ( mailboxId, IPC_RMID, 0 );
} // Fin de destroy

//////////////////////////////////////////////////////////////////  PUBLIC
//---------------------------------------------------- Fonctions publiques

int main ( int argc, const char * argv[] )
{
	pid_t keyboardManagementPid;
	pid_t hourPid;
	pid_t entrance1Pid, entrance2Pid, entrance3Pid;


	// ---------- INITIALIZATION
	InitialiserApplication( XTERM );
	init ( );

	// We should test the value returned by fork, there could be
	// an error when forking
	if ( (entrance1Pid = fork()) == 0 )
	{
		Entrance ( PROF_BLAISE_PASCAL );
	}
	else if ( (entrance2Pid = fork()) == 0 )
	{
		Entrance ( AUTRE_BLAISE_PASCAL );
	}
	else if ( (entrance3Pid = fork()) == 0 )
	{
		Entrance ( ENTREE_GASTON_BERGER );
	}
	else if ( (keyboardManagementPid = fork()) == 0 )
	{
		KeyboardManagement ( );
	}
	else
	{
		hourPid = ActiverHeure ( );

		// Wait for the user to quit the application
		// The call could be interrupted by a received signal.
		// But since we really want to wait, we relauch it every
		// time that happens.
		while ( -1 == waitpid ( keyboardManagementPid, NULL, 0 ) )
		{
			// Empty
		}

		// ---------- DESTRUCTION
		// From now on, handle SIGCHLD for normal process end sync
		struct sigaction action;
		sigemptyset ( &action.sa_mask );
		action.sa_handler = ack;
		action.sa_flags = 0;
		sigaction ( SIGCHLD, &action, NULL );

		// Kill every created process in reverse order
		kill ( hourPid, SIGUSR2 );
		kill ( entrance3Pid, SIGUSR2 );
		kill ( entrance2Pid, SIGUSR2 );
		kill ( entrance1Pid, SIGUSR2 );

		// TODO: wait for every processes to be down?
		destroy ( );
		TerminerApplication ( );
		exit ( 0 );
	}
} // Fin de main
