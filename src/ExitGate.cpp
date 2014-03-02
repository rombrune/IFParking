/*************************************************************************
							ExitGate
							-------------------

*************************************************************************/

//---------- Réalisation de la tâche <ExitGate> (fichier ExitGate.cpp) ---

/////////////////////////////////////////////////////////////////  INCLUDE
//-------------------------------------------------------- Include système
#include <cstdlib>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <errno.h>
#include <map>
#include <iostream> 
// TODO: remove ^
using namespace std;
//------------------------------------------------------ Include personnel
#include "Outils.h"

#include "ExitGate.h"
#include "common.h"
///////////////////////////////////////////////////////////////////  PRIVE
//------------------------------------------------------------- Constantes

//------------------------------------------------------------------ Types

//---------------------------------------------------- Variables statiques
// The file descriptor of the communication pipe
static int PipeRead;
// A map to hold the currently running SortirVoiture tasks
static map<pid_t, Car> currentValets;

//------------------------------------------------------ Fonctions privées
static void ack ( int signalNumber )
// Mode d'emploi :
// Acknowledges the death of a child SortirVoiture task.
{
	// Retrieve the pid of the terminated child
	int status;
	pid_t pid;
	// For each child task that has died
	while ( (pid = waitpid ( -1, &status, WNOHANG )) != - 1 )
	{
		if ( pid != 0 && WIFEXITED ( status ) )
		{
			Car car = currentValets[pid];

			// Remove this pid from the list of running tasks
			currentValets.erase ( pid );

			// Display the newly parked car
			AfficherSortie ( car.priority, car.licensePlate,
							car.entranceTime, time ( NULL ) );
		}
	}
} // Fin de ack

static void die ( int signalNumber )
// Mode d'emploi :
// Use for controlled destruction. Kills any child task.
{
	// Mask SIGCHLD
	struct sigaction action;
	action.sa_handler = SIG_IGN;
	sigemptyset ( &action.sa_mask );
	action.sa_flags = 0;
	sigaction ( SIGCHLD, &action, NULL );

	// Kill every running SortirVoiture
	for ( map<pid_t, Car>::iterator it = currentValets.begin();
			it != currentValets.end(); ++it )
	{
		kill ( it->first, SIGUSR2 );
		waitpid( it->first, NULL, 0 );
	}
	exit ( 0 );
} // Fin de die

static void init ( )
{
	// Respond to SIGUSR2 (= controlled destruction)
	struct sigaction action;
	action.sa_handler = die;
	sigemptyset ( &action.sa_mask );
	action.sa_flags = 0;
	sigaction ( SIGUSR2, &action, NULL );

	// Respond to SIGCHLD (= valet has finished getting car out)
	action.sa_handler = ack;
	sigaction ( SIGCHLD, &action, NULL );
} // Fin de init

static void incrementFreeSpots ( )
{
	MutexTake ( KEY );

	size_t size = sizeof ( State );
	int sharedMemId = shmget ( KEY, size, IPC_EXCL );
	State * state = (State *)shmat ( sharedMemId, NULL, 0 );
	
	state->freeSpots++;

	shmdt ( state );
	MutexRelease ( KEY );
} // Fin de incrementFreeSpots

//////////////////////////////////////////////////////////////////  PUBLIC
//---------------------------------------------------- Fonctions publiques
void ExitGate ( int pipeR, int pipeW )
// Algorithme :
// 1. Wait for a spot number to be written 
// in the communication pipe (from KeyboardManagement)
// 2. Launch a child SortirVoiture task
// 3. Process any request from the Entrances
{
	init ( );

	PipeRead = pipeR;
	// Close the unused end of the pipe
	close ( pipeW );

	for ( ; ; )
	{
		unsigned int spotNumber = -1;
		ssize_t sizeRead = 0;
		// Keep reading even when interrupted by a signal
		while ( sizeRead < sizeof ( unsigned int) )
		{
			ssize_t s;
			s = read ( PipeRead, &spotNumber, sizeof (unsigned int) );
			if ( s > 0 )
			{
				sizeRead += s;
			}
		}

		if ( spotNumber > 0 && spotNumber <= NB_PLACES )
		{
			incrementFreeSpots ( );
			pid_t valetPid = SortirVoiture ( spotNumber );
			// TODO: find the corresponding car in the state of the parking lot
			Car car;
			currentValets[valetPid] = car;

			// Now that a new spot has freed up,
			// we check if there's any open request from the entrances
			// TODO
		}
	}
} // Fin de ExitGate
