//---------- Interface du module <common> (fichier common.h) -------------
#if ! defined ( COMMON_H )
#define COMMON_H

//------------------------------------------------------------------------
// Rôle de l'interface <common>
// This module makes available common constants and types, as well as
// useful functions.
//------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////  INCLUDE
//--------------------------------------------------- Interfaces utilisées
#include <time.h>
#include <sys/wait.h>

#include "Outils.h"

//------------------------------------------------------------- Constantes
// For use with ftok
#define	EXEC_NAME	"parking"
#define	KEY_NUMBER	3
extern int const KEY;

// Permissions for IPCs
// Semaphores
#define	SEM_R		0400
#define	SEM_W		0200
// Mailboxes
#define	MSG_R		0400
#define	MSG_W		0200
// Shared memories
#define	SHM_R		0400
#define	SHM_W		0200

// Number of seconds to sleep right after letting a car enter
#define ENTRANCE_SLEEP_DELAY 1

//------------------------------------------------------------------ Types
// Structure representing a car
struct Car {
	// License plate counter
	static unsigned int counter;

	unsigned int licensePlate;
	TypeUsager priority;
	// Timestamp of the time at which this car has entered the lot
	// Stays at 0 until the car is not fully parked.
	time_t entranceTime;

	Car ( TypeUsager p )
		: priority ( p ), entranceTime ( 0 )
	{
		licensePlate = counter;
		counter = (counter + 1) % 999;
	}

	// Default constructor: do not increment the counter
	Car ( )
		: licensePlate ( -1 ), priority ( AUCUN ), entranceTime ( 0 )
	{
		// Empty
	}
	// Copy constructor: do not increment the counter
	Car ( Car const & other )
		: licensePlate ( other.licensePlate ),
		  priority ( other.priority ), entranceTime ( other.entranceTime )
	{
		// Empty
	}
};

// Structure representing a car that wants to enter the parking lot
// For use in mailbox and as part of the parking lot state
struct CarRequest {
	// Message type (one type per entrance)
	long type;
	// Car that wants to enter
	Car car;
	// Time at which the request is submitted
	time_t date;
	// pid of the process that submitted the request
	pid_t pid;

	CarRequest ( TypeBarriere t, Car c, time_t d, pid_t p )
		: type ( t ), car ( c ), date ( d ), pid ( p )
	{
		// Empty
	}
	CarRequest ( )
		: type ( AUCUNE ), date ( 0 )
	{
		// Empty
	}
};

// Structure holding the necessary informations to represent
// the current state of the parking lot
// For use in the shared memory
struct State {
	// Spots
	// The number of free spots can go below 0 if requests are pending
	int freeSpotsNumber;
	// The cars currently parked (in each spot)
	Car spots [ NB_PLACES ];
	// Requests currently posted
	int requestsNumber; // TODO: remove this redundant field?
	CarRequest requests [ NB_BARRIERES_ENTREE ];
};

//////////////////////////////////////////////////////////////////  PUBLIC
//---------------------------------------------------- Fonctions publiques
void SetSignalHandler ( int signalNumber, void (*handler) (int) );
// Mode d'emploi :
// Set the signal handler for the given signal
// with default options.

void MaskSignal ( int signalNumber );
// Mode d'emploi :
// Mask the given signal (i.e. set its handler to SIG_IGN)

void WaitForEnd ( pid_t pid );
// Mode d'emploi :
// Wait for the process corresponding to <pid> to return.
// Keep waiting even if the call gets interrupted by a signal.
// Contrat :
// <pid> corresponds to a process that is running or has run.

void MutexTake ( int key );
// Mode d'emploi :
// Try to obtain a lock. Wait until the lock can be obtained.
// Contrat :
// A semaphore buffer with 1 semaphore has been initialized
// with the given key.

void MutexRelease ( int key );
// Mode d'emploi :
// Try to release a lock. Wait until the lock can be released.
// Contrat :
// A semaphore buffer with 1 semaphore has been initialized
// with the given key.

State * ObtainSharedState ( );
// Mode d'emploi :
// Take the mutex, attach to the shared memory, 
// and return a pointer to the shared state of the parking lot.
// Contrat :
// ReleaseSharedState should be called as quickly as possible.

void ReleaseSharedState ( State * state );
// Mode d'emploi :
// Detach from the shared memory and release the mutex.
// Contrat :
// <state> is the pointer returned 
// by a previous call of ObtainSharedState.

#endif // COMMON_H
