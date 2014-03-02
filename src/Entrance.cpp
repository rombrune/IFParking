/*************************************************************************
							Entrance
							-------------------

*************************************************************************/

//---------- Réalisation de la tâche <Entrance> (fichier Entrance.cpp) ---

/////////////////////////////////////////////////////////////////  INCLUDE
//-------------------------------------------------------- Include système
#include <cstdlib>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <errno.h>
#include <map>
#include <iostream> 
// TODO: remove ^
using namespace std;
//------------------------------------------------------ Include personnel
#include "Outils.h"

#include "Entrance.h"
#include "common.h"
///////////////////////////////////////////////////////////////////  PRIVE
//------------------------------------------------------------- Constantes

//------------------------------------------------------------------ Types

//---------------------------------------------------- Variables statiques
// A map to hold the currently running GarerVoiture tasks
static map<pid_t, Car> currentValets;

//------------------------------------------------------ Fonctions privées
static void ack ( int signalNumber )
// Mode d'emploi :
// Acknowledges the death of a child GarerParking task.
{
	// Retrieve the pid of the terminated child
	int status;
	pid_t pid;
	// For each child task that has died
	while ( (pid = waitpid ( -1, &status, WNOHANG )) != - 1 )
	{
		if ( pid != 0 && WIFEXITED ( status ) )
		{
			// Retrieve the spot number (encoded into the return value)
			int spotNumber = WEXITSTATUS ( status );
			Car car = currentValets[pid];
			car.entranceTime = time ( NULL );

			// Remove this pid from the list of running tasks
			currentValets.erase ( pid );

			// Display the newly parked car
			AfficherPlace ( spotNumber, car.priority, 
							car.licensePlate, car.entranceTime);
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

	// Kill every running GarerVoiture
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

	// Respond to SIGCHLD (= valet has finished parking car)
	action.sa_handler = ack;
	sigaction ( SIGCHLD, &action, NULL );
} // Fin de init

static Car waitForCar ( TypeBarriere entrance )
// Mode d'emploi :
// Waits until a new car arrives at this entrance (in the mailbox)
// (sent by the KeyboardManagement task).
// Contrat :
//
{
	// Placeholder for the incoming message
	CarRequest message;

	// Read from the mailbox
	int mailboxId = msgget ( KEY, IPC_EXCL );
	int size = sizeof ( CarRequest ) - sizeof ( long );
	int status;
	// Restart this system call as long as we really have not 
	// actually received a car
	// (it could very well be interrupted by a signal)
	do
	{
		status = msgrcv ( mailboxId, &message, size, entrance, 0 );

		// Unhandled error case
		if ( status == -1 && errno != EINTR )
		{
			die ( 0 );
		}
	} while ( status == -1 );

	return message.car;
} // Fin de waitForCar

static bool canGoIn ( )
// Mode d'emploi :
// Checks in the parking lot state (shared memory)
// to see if we can send a car in right away, or if we should
// rather place a request and wait to be signaled.
{
	bool allowed = true;
	MutexTake ( KEY );

	size_t size = sizeof ( State );
	int sharedMemId = shmget ( KEY, size, IPC_EXCL );
	State * state = (State *)shmat ( sharedMemId, NULL, 0 );
	
	if ( state->requests.size() >= state->freeSpots )
	{
		allowed = false;
	}

	shmdt ( state );
	MutexRelease ( KEY );
	return allowed;
}

static void decrementFreeSpots ( )
{
	MutexTake ( KEY );

	size_t size = sizeof ( State );
	int sharedMemId = shmget ( KEY, size, IPC_EXCL );
	State * state = (State *)shmat ( sharedMemId, NULL, 0 );
	
	state->freeSpots--;

	shmdt ( state );
	MutexRelease ( KEY );
}

//////////////////////////////////////////////////////////////////  PUBLIC
//---------------------------------------------------- Fonctions publiques
void Entrance ( TypeBarriere entrance )
// Algorithme :
// 1. Dequeue a car from the mailbox (or wait for a car to arrive in the mailbox)
// 2. Read the number of available spots in the semcount
// 3. Read the number of current entrance requests in the shared memory
// 4. If available > request, go to 6
// 5. Place a request in the shared memory and wait for SIGUSR1
// 6. A spot is available for us, call GarerVoiture()
// 7. Sleep for ENTRANCE_SLEEP_DELAY seconds
// 8. Goto step 1.
{
	init ( );

	for ( ; ; )
	{
		Car next = waitForCar ( entrance );
		if ( next.licensePlate == -1 )
			cout << "ERROR: invalid car received!" << endl;
		// TODO: check that it can indeed park and place request if necessary

		if ( canGoIn ( ) )
		{
			decrementFreeSpots ( );
			pid_t valetPid = GarerVoiture ( entrance );
			currentValets[valetPid] = next;
		}
		else
		{
			// TODO: place request and wait to be signaled
			die ( 0 );
		}
		// TODO: check that at least ENTRANCE_SLEEP_DELAY time is slept
		unsigned int remaining = ENTRANCE_SLEEP_DELAY;
		while ( remaining > 0 )
		{
			remaining = sleep ( remaining );
		}
	}
} // Fin de Entrance
