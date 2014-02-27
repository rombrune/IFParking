/*************************************************************************
							Entrance
							-------------------

*************************************************************************/

//---------- Réalisation de la tâche <Entrance> (fichier Entrance.cpp) ---

/////////////////////////////////////////////////////////////////  INCLUDE
//-------------------------------------------------------- Include système
#include <cstdlib>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/msg.h>
#include <set>
using namespace std;
//------------------------------------------------------ Include personnel
#include "Entrance.h"
#include "config.h"
///////////////////////////////////////////////////////////////////  PRIVE
//------------------------------------------------------------- Constantes

//------------------------------------------------------------------ Types

//---------------------------------------------------- Variables statiques
static set<pid_t> currentValets;

//------------------------------------------------------ Fonctions privées
static void die ( int signalNumber )
{
	// TODO: kill any running GarerVoiture task
	for ( set<pid_t>::iterator it = currentValets.begin();
			it != currentValets.end(); ++it )
	{
		kill ( *it, SIGUSR2 );
		waitpid( *it, NULL, 0 );
	}
	exit ( 0 );
}
static void init ( )
{
	// Respond to SIGUSR2 (= controlled destruction)
	struct sigaction action;
	action.sa_handler = die;
	sigemptyset ( &action.sa_mask );
	action.sa_flags = 0;
	sigaction ( SIGUSR2, &action, NULL );
}

static void waitForCar ( TypeBarriere entrance )
// Mode d'emploi :
// Waits until a new car arrives at this entrance (in the mailbox)
// (sent by the KeyboardManagement task).
// Contrat :
//
{
	// Allocate memory to receive the message
	CarMessage * message = new CarMessage();

	// Read from the mailbox
	int key = ftok(EXEC_NAME, KEY);
	int mailboxId = msgget ( key, IPC_EXCL );

	// This call will wait until it can withdraw a message from the mailbox
	// TODO: test for failure? (e.g. interrupted by a signal)
	int size = sizeof ( CarMessage ) - sizeof ( long );
	msgrcv ( mailboxId, message, size, entrance, 0 );

	delete message;
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
		waitForCar ( entrance );

		// TODO: check that it can indeed park and place request if necessary

		pid_t valetPid = GarerVoiture ( entrance );
		currentValets.insert ( valetPid );
		// TODO: use a proper signal handler instead
		// (the return value is the spot occupied by the car)
		waitpid( valetPid, NULL, 0 );

		sleep ( ENTRANCE_SLEEP_DELAY );
	}
} // Fin de Entrance
