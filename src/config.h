//---------- Interface du module <config> (fichier config.h) ---------
#if ! defined ( CONFIG_H )
#define CONFIG_H

//------------------------------------------------------------------------
// Rôle de l'interface <config>
// This header file makes available common constants and types.
//
//------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////  INCLUDE
//--------------------------------------------------- Interfaces utilisées

//------------------------------------------------------------- Constantes
// TODO: executable name (for use with ftok)
// TODO: other useful constants?

// Number of seconds to sleep right after letting a car enter
const int ENTRANCE_SLEEP_DELAY = 1;

//------------------------------------------------------------------ Types
// TODO: struct type for requests (used in the shared memory)
// TODO: struct typ for incoming cars (used in mailbox)

#endif // CONFIG_H

