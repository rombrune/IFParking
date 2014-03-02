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
#include <errno.h>
#include <map>
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
static void removeCar ( unsigned int spotNumber )
// Mode d'emploi :
// Updates the state of the parking lot (in shared memory)
// to set <spotNumber> to empty
{
	State * state = ObtainSharedState ( );
	state->spots[spotNumber - 1].licensePlate = -1;
	ReleaseSharedState ( state );

	// Display this change
	Effacer ( TypeZone ( ETAT_P1 + (spotNumber - 1) ) );
} // Fin de removeCar

static int comparePriority ( CarRequest const & a, CarRequest const & b )
// Mode d'emploi :
// Return 1 if request <a> is more prioritary than request <b>.
// Return 0 if both cars are as prioritary.
// Return -1 if request <a> is less prioritary than request <b>.
{
	// TODO: check thoroughly
	// TODO: rewrite as operator< ?

	// Particular case: one of the passed requests is not valid
	if ( a.date == 0 )
	{
		return -1;
	}
	else if ( b.date == 0 )
	{
		return 1;
	}

	if ( (a.car.priority == b.car.priority) && (a.date == b.date) )
	{
		return 0;
	}
	// A professor is always prioritary
	else if ( (a.car.priority == PROF) && (b.car.priority != PROF) )
	{
		return 1;
	}
	else if ( (a.car.priority != PROF) && (b.car.priority == PROF) )
	{
		return -1;
	}
	// Otherwise, first arrived => first served
	else if ( a.date < b.date )
	{
		return 1;
	}
	else
	{
		return -1;
	}
} // Fin de comparePriority

static void processRequests ( )
// Mode d'emploi :
// We assume that ONE spot has just freed up. This function
// finds the most prioritary request from the entrances, and
// signals the corresponding entrance that it can let its car in.
{
	State * state = ObtainSharedState ( );
	
	bool hasPendingRequest = (state->requestsNumber > 0);
	CarRequest prioritary;
	int index = 0;
	if ( hasPendingRequest )
	{
		// Find the most prioritary request
		for (int i = 1; i < NB_BARRIERES_ENTREE; ++i )
		{
			if ( comparePriority ( state->requests[i], 
								   state->requests[index] ) > 0 )
			{
				index = i;
			}
		}

		// Remove it from the shared memory
		prioritary = state->requests[index];
		state->requestsNumber--;
		state->requests[index].date = 0;
	}
	// We release the critical resource as fast as possible
	ReleaseSharedState ( state );
	
	if ( hasPendingRequest )
	{
		// Signal the corresponding entrance
		kill ( prioritary.pid, SIGUSR1 );
		// Un-show the request
		Effacer ( TypeZone ( REQUETE_R1 + index ) );
	}
} // Find de processRequest

static void incrementFreeSpots ( )
{
	State * state = ObtainSharedState ( );
	state->freeSpotsNumber++;
	ReleaseSharedState ( state );
} // Fin de incrementFreeSpots


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
			// Retrieve the spot number (encoded into the return value)
			int spotNumber = WEXITSTATUS ( status );
			// Free this spot number
			removeCar ( spotNumber );

			// Display the car that just went out
			Car car = currentValets[pid];
			AfficherSortie ( car.priority, car.licensePlate,
							car.entranceTime, time ( NULL ) );

			// Remove this pid from the list of running tasks
			currentValets.erase ( pid );

			// Now that a new spot has freed up,
			// we check if there's any pending request from the entrances
			processRequests ( );
			// We increment the number of free spots as late as possible
			// to make sure that there is no spot theft
			incrementFreeSpots ( );
		}
	}
} // Fin de ack

static void die ( int signalNumber )
// Mode d'emploi :
// Use for controlled destruction. Kills any child task.
{
	// Mask SIGCHLD
	MaskSignal ( SIGCHLD );

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
	SetSignalHandler ( SIGUSR2, die );
	// Respond to SIGCHLD (= valet has finished getting car out)
	SetSignalHandler ( SIGCHLD, ack );
} // Fin de init

static Car getCarAt ( unsigned int spotNumber )
// Mode d'emploi :
// Retrieves the car at the given spot number in the shared memory.
// Writes the retrieved car in <car> if <car> is not NULL.
// Return false if there is no car at this spot.
// Contrat :
// There is indeed a car parked at this spot.
{
	State * state = ObtainSharedState ( );
	Car car = state->spots[spotNumber - 1];
	ReleaseSharedState ( state );
	return car;
}

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

		pid_t valetPid = SortirVoiture ( spotNumber );
		if ( valetPid != -1 )
		{
			// Find the corresponding car in the state of the parking lot
			currentValets[valetPid] = getCarAt ( spotNumber );
			// The parking spot is effectively freed up
			// when the valet returns.
		}
	}
} // Fin de ExitGate
