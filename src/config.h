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
#include <vector>

#include "Outils.h"

//------------------------------------------------------------- Constantes
// For use with ftok
#define EXEC_NAME "parking"
#define KEY 3
// Number of seconds to sleep right after letting a car enter
#define ENTRANCE_SLEEP_DELAY 1

//------------------------------------------------------------------ Types
// Vehicles can have different priorities (teacher > other)
enum Priority {
	TEACHER, OTHER, NONE
};

// Structure representing a car
struct Car {
	// License plate counter
	static unsigned int counter;

	unsigned int licensePlate;
	Priority priority;

	Car ( Priority p )
		: priority ( p )
	{
		licensePlate = counter;
		counter = (counter + 1) % 999;
	}

	// Copy constructor: do not increment the counter
	Car ( Car const & other )
		: licensePlate ( other.licensePlate ), priority ( other.priority )
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

