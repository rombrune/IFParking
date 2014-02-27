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
//------------------------------------------------------ Include personnel
#include "Entrance.h"
#include "config.h"
///////////////////////////////////////////////////////////////////  PRIVE
//------------------------------------------------------------- Constantes

//------------------------------------------------------------------ Types

//---------------------------------------------------- Variables statiques

//------------------------------------------------------ Fonctions privées
static void die ( int signalNumber )
{
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
// Mode d'emploi :
//
// Contrat :
//
// Algorithme :
//
//{
//} //----- fin de nom

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
		//waitForCar ( entrance );

		// TODO : check that it can indeed park and place request if necessary

		//pid_t valetPid = GarerVoiture ( entrance );
		//waitpid( valetPid, NULL, 0 );

		sleep ( ENTRANCE_SLEEP_DELAY );
	}
} // Fin de Entrance

