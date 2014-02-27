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
#include "Outils.h"

//------------------------------------------------------------- Constantes
#define EXEC_NAME "parking"
#define KEY 3

// TODO: other useful constants?

// Number of seconds to sleep right after letting a car enter
#define ENTRANCE_SLEEP_DELAY 1

//------------------------------------------------------------------ Types
enum Priority {
	TEACHER, OTHER, NONE
};

// TODO: struct type for requests (used in the shared memory)


// Message structure for incoming cars (used in mailbox)
struct CarMessage {
	// Message type (one type per entrance)
	long type;
	// Message body
	Priority priority;
	// TODO: add auto-incrementing license plate?

	CarMessage ( TypeBarriere t, Priority p )
		: type( t ), priority ( p )
	{
		// Empty
	}

	CarMessage ( )
		: type( 0 ), priority ( NONE )
	{
		// Empty
	}
};

#endif // CONFIG_H

