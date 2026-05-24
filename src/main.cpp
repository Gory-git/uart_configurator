#include <Arduino.h>
#include "SerialCLI.h"

SerialCLI serialCLI;

void setup() 
{
  serialCLI.begin();
}

void loop() 
{
}

/* TODO list
 * rendere una libreria
 * scrivere una versione non bloccante che gira in background
 * valutare l'aggiunta dei parametri di configurazione da linea di comando 
 * migliorare parsing
 * migliorare validazione
 * rendere modulare <-- fatto
 * aggiungere wear leveling (rotazione degli indirizzi di scrittura per una maggiore durata della eeprom)
 * controllare sicurezza del codice (buffer o stack oerflow, stringhe ecc.)
 */
