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
#include <sys/shm.h>
#include <sys/sem.h>
#include <signal.h>
//------------------------------------------------------ Include personnel
#include "Outils.h"
#include "Heure.h"

#include "common.h"
#include "Entrance.h"
#include "ExitGate.h"
#include "KeyboardManagement.h"
///////////////////////////////////////////////////////////////////  PRIVE
//------------------------------------------------------------- Constantes

//------------------------------------------------------------------ Types

//---------------------------------------------------- Variables statiques
static int PipeRead, PipeWrite;

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
// Contrat :
// No other task can start before this function has been called.
{
	// By default, mask SIGUSR2 signal
	struct sigaction action;
	sigemptyset ( &action.sa_mask );
	action.sa_handler = SIG_IGN;
	action.sa_flags = 0;
	sigaction ( SIGUSR2, &action, NULL );

	// TODO: check that permissions are as tight as possible
	int permissions = 0600;

	// Create the mailbox (KeyboardManagement => Entrances)
	int mailboxId = msgget ( KEY, (IPC_CREAT|IPC_EXCL|permissions) );

	// Create the shared memory (Entrances <=> Exit)
	size_t size = sizeof ( State );
	int sharedMemId = shmget ( KEY, size, (IPC_CREAT|IPC_EXCL|permissions) );

	// Set the initial state of the parking lot
	State * state = (State *)shmat ( sharedMemId, NULL, 0 );
	state->freeSpotsNumber = NB_PLACES;
	for (unsigned int i = 0; i < NB_PLACES; ++i)
	{
		state->isFree[i] = true;
	}
	state->requestsNumber = 0;
	shmdt ( state );

	// Create the shared memory mutex
	int semId = semget ( KEY, 1, (IPC_CREAT|IPC_EXCL|permissions) );
	// Initialize the mutex to 1
	// (because the shared memory is initially accessible)
	MutexRelease ( KEY );

	// Create the pipe (KeyboardManagement => ExitGate)
	int pipeFiles[2];
	pipe ( pipeFiles );
	PipeRead = pipeFiles[0];
	PipeWrite = pipeFiles[1];

} // Find de init

static void destroy ( )
{

	// Free the mailbox
	int mailboxId = msgget ( KEY, IPC_EXCL );
	msgctl ( mailboxId, IPC_RMID, 0 );

	// Free the shared memory
	size_t size = sizeof ( State );
	int sharedMemId = shmget ( KEY, size, IPC_EXCL );
	shmctl ( sharedMemId, IPC_RMID, 0 );

	// Free the shared memory mutex
	int semId = semget ( KEY, 1, IPC_EXCL );
	semctl ( semId, 0, IPC_RMID, 0);

	// Free the pipe
	close ( PipeRead );
	close ( PipeWrite );

} // Fin de destroy

//////////////////////////////////////////////////////////////////  PUBLIC
//---------------------------------------------------- Fonctions publiques

int main ( int argc, const char * argv[] )
{
	pid_t keyboardManagementPid;
	pid_t hourPid;
	pid_t entrance1Pid, entrance2Pid, entrance3Pid;
	pid_t exitGatePid;

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
	else if ( (exitGatePid = fork()) == 0 )
	{
		ExitGate ( PipeRead, PipeWrite );
	}
	else if ( (keyboardManagementPid = fork()) == 0 )
	{
		KeyboardManagement ( PipeRead, PipeWrite );
	}
	else
	{
		hourPid = ActiverHeure ( );

		WaitForEnd ( keyboardManagementPid );

		// ---------- DESTRUCTION
		// From now on, handle SIGCHLD for normal process end sync
		struct sigaction action;
		sigemptyset ( &action.sa_mask );
		action.sa_handler = ack;
		action.sa_flags = 0;
		sigaction ( SIGCHLD, &action, NULL );

		// Kill every created process in reverse order
		kill ( hourPid, SIGUSR2 );
		kill ( exitGatePid, SIGUSR2 );
		kill ( entrance3Pid, SIGUSR2 );
		kill ( entrance2Pid, SIGUSR2 );
		kill ( entrance1Pid, SIGUSR2 );

		// TODO: wait for every processes to be down?
		destroy ( );
		TerminerApplication ( );
		exit ( 0 );
	}
} // Fin de main
