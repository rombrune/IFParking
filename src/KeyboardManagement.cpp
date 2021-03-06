/*************************************************************************
							KeyboardManagement
							-------------------

*************************************************************************/

// Réalisation de la tâche <KeyboardManagement> (fichier KeyboardManagement.cpp)

/////////////////////////////////////////////////////////////////  INCLUDE
//-------------------------------------------------------- Include système
#include <stdlib.h>
#include <unistd.h>
#include <sys/msg.h>
//------------------------------------------------------ Include personnel
#include "Outils.h"
#include "Menu.h"

#include "common.h"
#include "KeyboardManagement.h"
///////////////////////////////////////////////////////////////////  PRIVE
//------------------------------------------------------------- Constantes
//------------------------------------------------------------------ Types
//---------------------------------------------------- Variables statiques
static int PipeWrite;

//------------------------------------------------------ Fonctions privées
static TypeBarriere getTypeBarriere ( int entranceNumber, TypeUsager priority )
// Mode d'emploi :
// Return the TypeBarriere corresponding to the given entrance number
// and kind of user (different users may use different entrance gates).
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
// Add a new car with the given priority at given entrance
{
	// Prepare the message
	// The message type indicates the entrance
	Car car ( priority );
	CarRequest message ( entrance, car, time ( NULL ), getpid ( ) );

	// Insert it into the mailbox
	int mailboxId = msgget ( KEY, IPC_EXCL );
	int size = sizeof ( CarRequest ) - sizeof ( long );
	// TODO: test for failure?
	msgsnd ( mailboxId, &message, size, 0 );
} // Fin de queueCar

static void askExit ( unsigned int spotNumber )
// Mode d'emploi :
// Tells the ExitGate that the car currently parked at <spotNumber>
// would like to go out of the parking lot.
// Contrat :
// 0 <= spotNumber <= NB_PLACES
{
	write ( PipeWrite, &spotNumber, sizeof ( unsigned int ) );
} // Fin de askExit

//////////////////////////////////////////////////////////////////  PUBLIC
//---------------------------------------------------- Fonctions publiques
void KeyboardManagement ( int pipeR, int pipeW )
{
	//----------------------------------------------------- INITIALIZATION
	PipeWrite = pipeW;
	// Close the unused end of the pipe
	close ( pipeR );

	//---------------------------------------------------------------- RUN
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
			close ( PipeWrite );
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
			askExit ( value );
			break;

		default:
			break;
	}
} // Fin de Commande
