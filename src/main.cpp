#include <Arduino.h>
#include "SerialCLI.h"

SerialCLI serialCLI;
Parameters localParameters;
char* ip = "void";
char* id = "void";
char* port = "void";

void setup() 
{
  serialCLI.begin();
  localParameters = serialCLI.getParameters();
  ip = localParameters.ip;
  id = localParameters.id;
  port = localParameters.port;

  Serial.println(ip);
  Serial.println(id);
  Serial.println(port);
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
