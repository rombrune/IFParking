//---------- Interface du module <common> (fichier common.h) ---------
#if ! defined ( CONFIG_H )
#define CONFIG_H

//------------------------------------------------------------------------
// Rôle de l'interface <common>
// This module makes available common constants and types, as well as
// useful functions.
//------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////  INCLUDE
//--------------------------------------------------- Interfaces utilisées
#include <time.h>

#include "Outils.h"

//------------------------------------------------------------- Constantes
// For use with ftok
#define EXEC_NAME "parking"
#define KEY_NUMBER 3
extern int const KEY;

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
	// Stays at 0 until the car has not fully parked.
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
		: licensePlate ( other.licensePlate ), priority ( other.priority ), entranceTime ( other.entranceTime )
	{
		// Empty
	}
};

// Structure representing a car that wants to enter the parking lot
// For use in mailbox and as part of the parking lot state
struct CarRequest {
	// Message type (one type per entrance)
	long type;
	Car car;

	CarRequest ( TypeBarriere t, Car c )
		: type ( t ), car ( c )
	{
		// Empty
	}
	CarRequest ( )
		: type ( AUCUNE )
	{
		// Empty
	}
};

// Structure holding the necessary informations to represent
// the current state of the parking lot
// For use in the shared memory
struct State {
	// Number of free spots
	int freeSpots;
	// Requests currently posted
	int requestsNumber;
	CarRequest requests [ NB_BARRIERES_ENTREE ];
};

//////////////////////////////////////////////////////////////////  PUBLIC
//---------------------------------------------------- Fonctions publiques

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

#endif // CONFIG_H
