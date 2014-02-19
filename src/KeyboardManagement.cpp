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
	
} // Fin de KeyboardManagement

void Commande ( char code, unsigned int value )
{
	switch ( code ) {
		case 'q':
			exit(0);
			break;
			
		default:
			break;
	}
}