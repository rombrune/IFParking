//---------- Réalisation du module <common> (fichier common.cpp) ------

/////////////////////////////////////////////////////////////////  INCLUDE
//-------------------------------------------------------- Include système
#include <cstdlib>
#include <signal.h>
#include <errno.h>
#include <sys/sem.h>
#include <sys/shm.h>
//------------------------------------------------------ Include personnel
#include "common.h"
///////////////////////////////////////////////////////////////////  PRIVE
//------------------------------------------------------------- Constantes
int const KEY = ftok ( EXEC_NAME, KEY_NUMBER );
//------------------------------------------------------------------ Types
//---------------------------------------------------- Variables statiques
unsigned int Car::counter = 1;
//------------------------------------------------------ Fonctions privées
static void semOperation ( int key, short semIndex, short semOp )
// Mode d'emploi :
// Add <semOp> to the semaphore <semIndex> of the sembuf
// initialized with <key>. This call is likely to be blocking, and we keep
// trying even when interrupted by a signal.
// Contrat :
// A sembuf of size 1 has been initialized with <key>.
{
	int semId = semget ( key, 1, IPC_EXCL );

	struct sembuf operation;
	operation.sem_num = 0;
	operation.sem_op = 1;
	operation.sem_flg = 0;
	
	int status;
	do
	{
		// Keep trying even if interrupted by a signal
		status = semop ( semId, &operation, 1 );

		// Unhandled error case 
		// (better crash than risk corrupting everything)
		if ( status == -1 && errno != EINTR )
		{
			exit ( errno );
		}
	} while ( status == -1 );
} // Fin de semOperation

//////////////////////////////////////////////////////////////////  PUBLIC
//---------------------------------------------------- Fonctions publiques
void SetSignalHandler ( int signalNumber, void (*handler) (int) )
{
	struct sigaction action;
	action.sa_handler = handler;
	sigemptyset ( &action.sa_mask );
	action.sa_flags = 0;
	sigaction ( signalNumber, &action, NULL );
} // Fin de SetSignalHandler

void MaskSignal ( int signalNumber )
{
	SetSignalHandler ( signalNumber, SIG_IGN );
} // Fin de MaskSignal

void WaitForEnd ( pid_t pid )
{
	int status;
	do
	{
		// Keep waiting even if interrupted by a signal
		status = waitpid ( pid, NULL, 0 );

		// Unhandled error case 
		// (better crash than risk corrupting everything)
		if ( status == -1 && errno != EINTR )
		{
			exit ( errno );
		}
	} while ( status == -1 );
} // Fin de WaitForEnd

void MutexTake ( int key )
{
	semOperation ( key, 0, -1 );
} // Fin de MutexTake
void MutexRelease ( int key )
{
	semOperation ( key, 0, 1 );
} // Fin de MutexRelease

State * ObtainSharedState ( )
{
	MutexTake ( KEY );

	size_t size = sizeof ( State );
	int sharedMemId = shmget ( KEY, size, IPC_EXCL );
	return (State *)shmat ( sharedMemId, NULL, 0 );
} // Fin de ObtainSharedState

void ReleaseSharedState ( State * state )
{
	shmdt ( state );
	MutexRelease ( KEY );
} // Fin de ReleaseSharedState
