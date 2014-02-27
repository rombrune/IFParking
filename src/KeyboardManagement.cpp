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
static TypeBarriere getTypeBarriere ( int entranceNumber, Priority priority )
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
		if ( TEACHER == priority )
		{
			return PROF_BLAISE_PASCAL;
		}
		else
		{
			return AUTRE_BLAISE_PASCAL;
		}
	}
}
// Contrat :
//
// Algorithme :
//
//{
//} //----- fin de nom

//////////////////////////////////////////////////////////////////  PUBLIC
//---------------------------------------------------- Fonctions publiques
void KeyboardManagement ( )
{
	Menu ( );
} // Fin de KeyboardManagement

void Commande ( char code, unsigned int value )
{
	switch ( code ) {
		// Quit the application
		case 'q':
			exit( 0 );
			break;

		// Queue a new prioritary car at entrance <value>
		case 'p':
			exit( 0 );
			break;

		// Queue a new normal car at entrance <value>
		case 'a':
			exit( 0 );
			break;

		// Car parked at spot <value> now wants to leave
		case 's':
			exit( 0 );
			// Write to the exit gate through communication pipe
			break;

		default:
			exit( 0 );
			break;
	}
}
