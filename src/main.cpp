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
 * make it a library
 * write a non-blocking version that runs in the background
 * evaluate adding configuration parameters from command line
 * improve parsing
 * improve validation
 * make it modular <-- done
 * add wear leveling (rotation of write addresses for greater EEPROM durability)
 * check code security (buffer or stack overflow, strings, etc.)
 */
