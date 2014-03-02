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
#include <sys/wait.h>
#include <sys/msg.h>
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
// A flag to record the authorization to let a car from the exit gate
// (set in the SIGUSR1 signal handler)
static bool authorizationReceived;

//------------------------------------------------------ Fonctions privées
static void placeCar ( unsigned int spotNumber, Car car )
// Mode d'emploi :
// Updates the state of the parking lot (in shared memory)
// to set <spotNumber> to the given car
{
	State * state = ObtainSharedState ( );
	state->isFree[spotNumber - 1] = false;
	state->spots[spotNumber - 1] = car;
	ReleaseSharedState ( state );
} // Fin de placeCar

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

			// Update the state of the parking lot
			placeCar ( spotNumber, car );

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
	MaskSignal ( SIGCHLD );

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
	MaskSignal ( SIGUSR1 );
	// Respond to SIGUSR2 (= controlled destruction)
	SetSignalHandler ( SIGUSR2, die );
	// Respond to SIGCHLD (= valet has finished parking car)
	SetSignalHandler ( SIGCHLD, ack );
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
	State * state = ObtainSharedState ( );
	bool allowed = true;
	if ( state->requestsNumber >= state->freeSpotsNumber )
	{
		allowed = false;
	}
	ReleaseSharedState ( state );
	return allowed;
}

static void decrementFreeSpots ( )
{
	State * state = ObtainSharedState ( );
	state->freeSpotsNumber--;
	ReleaseSharedState ( state );
} // Fin de decrementFreeSpots

static void placeRequest ( TypeBarriere entrance, Car car )
// Contrat :
// 0 <= entrance < NB_BARRIERES_ENTREES
{
	time_t now = time ( NULL );
	CarRequest request ( entrance, car, now, getpid ( ) );
	
	State * state = ObtainSharedState ( );
	state->requests[entrance - 1] = request;
	state->requestsNumber++;
	ReleaseSharedState ( state );
	// Display this new request
	AfficherRequete ( entrance, car.priority, now );
} // Fin de placeRequest

static void authorize ( int signalNumber )
// Mode d'emploi :
// This signal handler is called when the exit gate
// allows us to let our car in
{
	authorizationReceived = true;
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
		
		// If there's no free spot right now, place a request
		// and wait patiently to be signaled by the exit gate
		if ( ! canGoIn ( ) )
		{
			authorizationReceived = false;
			placeRequest ( entrance, next );
			
			SetSignalHandler ( SIGUSR1, authorize );
			while ( !authorizationReceived )
			{
				pause ( );
			}
			MaskSignal ( SIGUSR1 );
		}
		
		// Allow the car to go in
		decrementFreeSpots ( ); // TODO: not always necessary
		pid_t valetPid = GarerVoiture ( entrance );
		currentValets[valetPid] = next;
		
		// Sleep for at least <ENTRANCE_SLEEP_DELAY>
		// before allowing another car in
		unsigned int remaining = ENTRANCE_SLEEP_DELAY;
		while ( remaining > 0 )
		{
			remaining = sleep ( remaining );
		}
	}
} // Fin de Entrance
