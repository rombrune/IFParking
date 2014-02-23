/*************************************************************************
							Entrance
							-------------------

*************************************************************************/

//---------- Réalisation de la tâche <Entrance> (fichier Entrance.cpp) ---

/////////////////////////////////////////////////////////////////  INCLUDE
//-------------------------------------------------------- Include système
#include <cstdlib>
//------------------------------------------------------ Include personnel
#include "Entrance.h"
#include "config.h"
///////////////////////////////////////////////////////////////////  PRIVE
//------------------------------------------------------------- Constantes

//------------------------------------------------------------------ Types

//---------------------------------------------------- Variables statiques

//------------------------------------------------------ Fonctions privées
//static type nom ( liste de paramètres )
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
void Entrance ( )
// Algorithme :
// 1. Dequeue a car (or wait for a car to be available)
// 2. Read the number of available spots in the semcount
// 3. Read the number of current entrance requests in the shared memory
// 4. If available > request, go to 6
// 5. Place a request in the shared memory and wait for SIGUSR1
// 6. A spot is available for us, call GarerVoiture()
// 7. Sleep for ENTRANCE_SLEEP_DELAY seconds
// 8. Goto step 1.
{
	// TODO
} // Fin de Entrance

