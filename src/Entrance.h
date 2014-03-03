/*************************************************************************
							Entrance
							-------------------

*************************************************************************/

//---------- Interface de la tâche <Entrance> (fichier Entrance.h) -------
#if ! defined ( ENTRANCE_H_ )
#define ENTRANCE_H_

//------------------------------------------------------------------------
// Rôle de la tâche <Entrance>
// Manages one car queue and allows one car to enter at a time.
// A delay of at least <ENTRANCE_SLEEP_DELAY> is imposed between two
// consecutive cars.
//------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////  INCLUDE
//--------------------------------------------------- Interfaces utilisées
#include "Outils.h"
//------------------------------------------------------------- Constantes

//------------------------------------------------------------------ Types

//////////////////////////////////////////////////////////////////  PUBLIC
//---------------------------------------------------- Fonctions publiques
void Entrance ( TypeBarriere entrance );
// Mode d'emploi :
// Starts an Entrance task to handle <entrance>.

#endif // ENTRANCE_H_
