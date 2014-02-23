/*************************************************************************
							KeyboardManagement
							-------------------

*************************************************************************/

//---------- Réalisation de la tâche <KeyboardManagement> (fichier KeyboardManagement.cpp) ---

/////////////////////////////////////////////////////////////////  INCLUDE
//-------------------------------------------------------- Include système
#include <cstdlib>
//------------------------------------------------------ Include personnel
#include "KeyboardManagement.h"
#include "Menu.h"
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
void KeyboardManagement ( )
{
	Menu ( );
} // Fin de KeyboardManagement

void Commande ( char code, unsigned int value )
{
	switch ( code ) {
		// Quit the application
		case 'q':
			exit(0);
			break;

		// Queue a new prioritary car at entrance <value>
		case 'p':
			// Create a new priority car
			// Place it into the mailbox of entrance <value>
			break;

		// Queue a new normal car at entrance <value>
		case 'a':
			// Create a new normal car
			// Place it into the mailbox of entrance <value>
			break;

		// Car parked at spot <value> now wants to leave
		case 's':
			// Write to the exit gate through communication pipe
			break;

		default:
			break;
	}
}
