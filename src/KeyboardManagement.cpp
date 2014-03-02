/*************************************************************************
							KeyboardManagement
							-------------------

*************************************************************************/

//---------- Réalisation de la tâche <KeyboardManagement> (fichier KeyboardManagement.cpp) ---

/////////////////////////////////////////////////////////////////  INCLUDE
//-------------------------------------------------------- Include système
#include <cstdlib>
#include <sys/msg.h>
//------------------------------------------------------ Include personnel
#include "Outils.h"
#include "Menu.h"

#include "config.h"
#include "KeyboardManagement.h"
///////////////////////////////////////////////////////////////////  PRIVE
//------------------------------------------------------------- Constantes

//------------------------------------------------------------------ Types

//---------------------------------------------------- Variables statiques

//------------------------------------------------------ Fonctions privées
static TypeBarriere getTypeBarriere ( int entranceNumber, TypeUsager priority )
// Mode d'emploi :
// Returns the TypeBarriere corresponding to the given entrance number.
// Contrat :
// 1 <= entranceNumber <= 2 
{
	if ( entranceNumber == 2 )
	{
		return ENTREE_GASTON_BERGER;
	}
	else
	{
		if ( PROF == priority )
		{
			return PROF_BLAISE_PASCAL;
		}
		else
		{
			return AUTRE_BLAISE_PASCAL;
		}
	}
} // Fin de getTypeBarriere

static void queueCar ( TypeBarriere entrance, TypeUsager priority )
// Mode d'emploi :
// Adds a new car with the given priority at given entrance
// Contrat :
//
{
	// Prepare the message
	// The message type indicates the entrance
	Car car ( priority );
	CarRequest message ( entrance, car );

	// Insert it into the mailbox
	int key = ftok ( EXEC_NAME, KEY );
	int mailboxId = msgget ( key, IPC_EXCL );
	int size = sizeof ( CarRequest ) - sizeof ( long );
	// TODO: test for failure?
	msgsnd ( mailboxId, &message, size, 0 );
} // Fin de queueCar

//////////////////////////////////////////////////////////////////  PUBLIC
//---------------------------------------------------- Fonctions publiques
void KeyboardManagement ( )
{
	for ( ; ; )
	{
		Menu ( );
	}
} // Fin de KeyboardManagement

void Commande ( char code, unsigned int value )
{
	switch ( code ) {
		// Quit the application
		case 'Q':
			exit( 0 );
			break;

		// Queue a new prioritary car at entrance <value>
		case 'P':
			queueCar ( getTypeBarriere ( value, PROF ),  PROF );
			break;

		// Queue a new normal car at entrance <value>
		case 'A':
			queueCar ( getTypeBarriere ( value, AUTRE ),  AUTRE );
			break;

		// Car parked at spot <value> now wants to leave
		case 'S':
			exit( 0 );
			// Write to the exit gate through communication pipe
			break;

		default:
			break;
	}
} // Fin de Commande
