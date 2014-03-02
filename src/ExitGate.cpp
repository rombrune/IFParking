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
#include <sys/msg.h>
#include <sys/shm.h>
#include <errno.h>
#include <map>
#include <iostream> 
// TODO: remove ^
using namespace std;
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
// A map to hold the currently running SortirVoiture tasks
static map<pid_t, Car> currentValets;

//------------------------------------------------------ Fonctions privées
static void ack ( int signalNumber )
// Mode d'emploi :
// Acknowledges the death of a child SortirVoiture task.
{
	// TODO
} // Fin de ack

static void die ( int signalNumber )
// Mode d'emploi :
// Use for controlled destruction. Kills any child task.
{
	// Mask SIGCHLD
	struct sigaction action;
	action.sa_handler = SIG_IGN;
	sigemptyset ( &action.sa_mask );
	action.sa_flags = 0;
	sigaction ( SIGCHLD, &action, NULL );

	// Kill every running SortirVoiture
	for ( map<pid_t, Car>::iterator it = currentValets.begin();
			it != currentValets.end(); ++it )
	{
		kill ( it->first, SIGUSR2 );
		waitpid( it->first, NULL, 0 );
	}
	exit ( 0 );
} // Fin de die

static void init ( )
{
	// Respond to SIGUSR2 (= controlled destruction)
	struct sigaction action;
	action.sa_handler = die;
	sigemptyset ( &action.sa_mask );
	action.sa_flags = 0;
	sigaction ( SIGUSR2, &action, NULL );

	// Respond to SIGCHLD (= valet has finished getting car out)
	action.sa_handler = ack;
	sigaction ( SIGCHLD, &action, NULL );
} // Fin de init

//////////////////////////////////////////////////////////////////  PUBLIC
//---------------------------------------------------- Fonctions publiques
void ExitGate ( int pipeR, int pipeW )
// Algorithme :
// 1. Wait for a spot number to be written 
// in the communication pipe (from KeyboardManagement)
// 2. Launch a child SortirVoiture task
// 3. Process any request from the Entrances
{
	init ( );

	PipeRead = pipeR;
	// Close the unused end of the pipe
	close ( pipeW );

	for ( ; ; )
	{
		unsigned int spotNumber = -1;
		read ( PipeRead, &spotNumber, sizeof (unsigned int) );

		cout << "New exit request: " << spotNumber << endl;
	}
} // Fin de ExitGate
