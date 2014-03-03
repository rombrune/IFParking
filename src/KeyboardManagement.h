/*************************************************************************
							KeyboardManagement
							-------------------

*************************************************************************/

//- Interface de la tâche <KeyboardManagement> (fichier KeyboardManagement.h)
#if ! defined ( KEYBOARDMANAGEMENT_H_ )
#define KEYBOARDMANAGEMENT_H_

//------------------------------------------------------------------------
// Rôle de la tâche <KeyboardManagement>
// This task is responsible for displaying the menu and handling the
// user interaction.
//------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////  INCLUDE
//--------------------------------------------------- Interfaces utilisées

//------------------------------------------------------------- Constantes

//------------------------------------------------------------------ Types

//////////////////////////////////////////////////////////////////  PUBLIC
//---------------------------------------------------- Fonctions publiques
void KeyboardManagement ( int pipeR, int pipeW );
// Mode d'emploi :
// Handles keyboard input and sends the appropriate command.
// This procedure should be called in its own process, it only returns
// when the user asks to quit the program.
// Contrat :
// <pipeR> and <pipeW> are the file descriptors of a previously opened
// communication pipe.

void Commande ( char code, unsigned int valeur );
// Mode d'emploi :
// Executes the command corresponding to the received input.
// For use by the Menu module.

#endif // KEYBOARDMANAGEMENT_H_
