/*************************************************************************
							common
							-------------------

*************************************************************************/

//---------- Réalisation de la tâche <common> (fichier common.cpp) ------

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
}

void MaskSignal ( int signalNumber )
{
	SetSignalHandler ( signalNumber, SIG_IGN );
}

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
}

void MutexTake ( int key )
{
	semOperation ( key, 0, -1 );
} // Fin de mutexTake
void MutexRelease ( int key )
{
	semOperation ( key, 0, 1 );
} // Fin de mutexRelease

State * ObtainSharedState ( )
{
	MutexTake ( KEY );

	size_t size = sizeof ( State );
	int sharedMemId = shmget ( KEY, size, IPC_EXCL );
	return (State *)shmat ( sharedMemId, NULL, 0 );
}

void ReleaseSharedState ( State * state )
{
	shmdt ( state );
	MutexRelease ( KEY );
}
