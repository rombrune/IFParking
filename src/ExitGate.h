/*************************************************************************
							ExitGate
							-------------------

*************************************************************************/

//---------- Interface de la tâche <ExitGate> (fichier ExitGate.h) -------
#if ! defined ( EXITGATE_H_ )
#define EXITGATE_H_

//------------------------------------------------------------------------
// Rôle de la tâche <ExitGate>
// Manages the exit gate of the parking lot.
// Also responsible for signaling entrances
// when the parking lot stops being full.
//------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////  INCLUDE
//--------------------------------------------------- Interfaces utilisées
#include "Outils.h"
//------------------------------------------------------------- Constantes

//------------------------------------------------------------------ Types

//////////////////////////////////////////////////////////////////  PUBLIC
//---------------------------------------------------- Fonctions publiques
void ExitGate ( int pipeR, int pipeW );
// Mode d'emploi :
// <pipeR> and <pipeW> are the file descriptors of a previously opened
// communication pipe. This tasks use the read end to receive information
// from the KeyboardManagement task.

#endif // EXITGATE_H_
