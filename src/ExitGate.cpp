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
// A map of all the currently running SortirVoiture tasks
// spot number => pid of the task taking care of it
static map<unsigned int, pid_t> currentValets;

//------------------------------------------------------ Fonctions privées
static int comparePriority ( CarRequest const & a, CarRequest const & b )
// Mode d'emploi :
// Return 1 if request <a> is more prioritary than request <b>.
// Return -1 if request <a> is less prioritary than request <b>.
// TODO: rewrite as operator< ?
{
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
		// Licence plates are numbered in increasing order
		// TODO: handle license plate rollover
		return ( a.car.licensePlate < b.car.licensePlate ? 1 : -1 );
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
	else
	{
		return ( a.date < b.date ? 1 : -1 );
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

static Car removeCar ( unsigned int spotNumber )
// Mode d'emploi :
// Updates the state of the parking lot (in shared memory)
// to set <spotNumber> to empty.
// Return the car that was parked there.
{
	State * state = ObtainSharedState ( );
	Car previous = state->spots[spotNumber - 1];
	state->spots[spotNumber - 1].licensePlate = -1;
	ReleaseSharedState ( state );

	// Display this change
	Effacer ( TypeZone ( ETAT_P1 + (spotNumber - 1) ) );

	return previous;
} // Fin de removeCar

static void ack ( int signalNumber )
// Mode d'emploi :
// Acknowledge the death of children SortirVoiture tasks.
// Effectively free the parking spot that used to be taken by the car.
// Process the most prioritary pending request.
{
	// Retrieve the pid of the terminated child
	int status;
	pid_t pid;
	// For each child task that has died
	while ( (pid = waitpid ( -1, &status, WNOHANG )) > 0 )
	{
		if ( WIFEXITED ( status ) )
		{
			// Retrieve the spot number (encoded into the return value)
			int spotNumber = WEXITSTATUS ( status );
			// Free this spot in the shared state
			Car car = removeCar ( spotNumber );
			// Remove this pid from the list of running tasks
			currentValets.erase ( spotNumber );

			// Display the car that just went out
			AfficherSortie ( car.priority, car.licensePlate,
							car.entranceTime, time ( NULL ) );

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
	for ( map<unsigned int, pid_t>::iterator it = currentValets.begin();
			it != currentValets.end(); ++it )
	{
		kill ( it->second, SIGUSR2 );
		waitpid( it->second, NULL, 0 );
	}

	close( PipeRead );
	exit ( 0 );
} // Fin de die

static void init ( )
{
	// Respond to SIGUSR2 (= controlled destruction)
	SetSignalHandler ( SIGUSR2, die );
	// Respond to SIGCHLD (= valet has finished getting car out)
	SetSignalHandler ( SIGCHLD, ack );
} // Fin de init

//////////////////////////////////////////////////////////////////  PUBLIC
//---------------------------------------------------- Fonctions publiques
void ExitGate ( int pipeR, int pipeW )
// Algorithme :
// 1. Wait for a spot number to be written 
// in the communication pipe (from KeyboardManagement)
// 2. Launch a child SortirVoiture task
// 3. Process any request from the Entrances when the child task returns.
{
	//----------------------------------------------------- INITIALIZATION
	init ( );

	PipeRead = pipeR;
	// Close the unused end of the pipe
	close ( pipeW );

	//---------------------------------------------------------------- RUN
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

		// Check that we don't already have a child task taking care of it
		if ( currentValets.find ( spotNumber ) == currentValets.end ( ) )
		{
			pid_t valetPid = SortirVoiture ( spotNumber );
			if ( valetPid != -1 )
			{
				currentValets[spotNumber] = valetPid;
				// The parking spot will be effectively freed up
				// when the valet returns.
			}
		}
	}
} // Fin de ExitGate
