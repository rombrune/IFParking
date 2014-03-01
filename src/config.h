//---------- Interface du module <config> (fichier config.h) ---------
#if ! defined ( CONFIG_H )
#define CONFIG_H

//------------------------------------------------------------------------
// Rôle de l'interface <config>
// This header file makes available common constants and types.
//
//------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////  INCLUDE
//--------------------------------------------------- Interfaces utilisées
#include <time.h>
#include <vector>

#include "Outils.h"

//------------------------------------------------------------- Constantes
// For use with ftok
#define EXEC_NAME "parking"
#define KEY 3
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
	std::vector<CarRequest> requests;

	State ( )
		: freeSpots ( NB_PLACES )
	{

	}
};

#endif // CONFIG_H

