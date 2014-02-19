/*************************************************************************
							KeyboardManagement
							-------------------

*************************************************************************/

//---------- Interface de la tâche <KeyboardManagement> (fichier KeyboardManagement.h) -------
#if ! defined ( KeyboardManagement_H_ )
#define KeyboardManagement_H_

//------------------------------------------------------------------------
// Rôle de la tâche <KeyboardManagement>
//
//
//------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////  INCLUDE
//--------------------------------------------------- Interfaces utilisées

//------------------------------------------------------------- Constantes

//------------------------------------------------------------------ Types

//////////////////////////////////////////////////////////////////  PUBLIC
//---------------------------------------------------- Fonctions publiques
void KeyboardManagement ( );
// Mode d'emploi :
// Handles keyboard input and sends the appropriate command.
// This procedure should be called in its own process, it only returns
// when the user asks to quit the program.

void Commande ( char code, unsigned int valeur );
// Mode d'emploi :
// Executes the command corresponding to the received input.
// For use by the Menu module.

#endif // KeyboardManagement_H_