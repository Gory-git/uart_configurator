#include "SerialCLI.h"
#include "nvs_flash.h"

SerialCLI::SerialCLI() : configurator() {}

void SerialCLI::begin()
{
  Serial.begin(115200);

  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    nvs_flash_erase();
    nvs_flash_init();
  }

  if (this->configurator.load())  
  {
      Serial.println("Modificare parametri di configurazione?[y/n]");
      if (this->configurator.makeChoice(10000)) 
      {
          handleInput();
      }
  } else  
  {    
      Serial.println("Nessuna configurazione trovata, usando valori di default. Avvio CLI...\n");  
      handleInput();
  }
  Serial.print("Configurazione terminata!");
}

SerialCLI::Command SerialCLI::getCommand(const char* command)
{
  if (strcmp(command, "help") == 0) return Command::HELP;
  if (strcmp(command, "set") == 0) return Command::SET;
  if (strcmp(command, "get") == 0) return Command::GET;
  if (strcmp(command, "save") == 0) return Command::SAVE;
  if (strcmp(command, "clear") == 0) return Command::CLEAR;
  if (strcmp(command, "exit") == 0) return Command::EXIT;
  return Command::UNKNOWN;
}

void SerialCLI::handleInput() 
{
  printHelp();
  while (true)
  {
    Serial.print("> ");
    while (!Serial.available()) 
    {
      delay(10);    
    }
    String input = Serial.readStringUntil('\n');
    input.trim();
    Serial.println(input);
    char buffer[input.length() +1];
    input.toCharArray(buffer, input.length() + 1);
    const char delimiter[] = " ";
    char *token;
    char *parameter = NULL;
    char *value = NULL;
    token = strtok(buffer, delimiter);
    if (token == NULL) 
    {
      continue;
    }
    switch (getCommand(token))
    {
      case Command::HELP:
        printHelp();
        break;
      case Command::SET:
        parameter = strtok(NULL, delimiter);
        value = strtok(NULL, delimiter);
        this->configurator.setParameter(parameter, value);
        break;
      case Command::GET:
        parameter = strtok(NULL, delimiter);
        this->configurator.getParameter(parameter);
        break;
      case Command::SAVE:
        this->configurator.save();
        break;
      case Command::CLEAR:
        this->configurator.clear();
        break;
      case Command::EXIT:
        if (this->configurator.exit()) return;
        break;
      default:
        Serial.println("ATTENZIONE: Comando sconosciuto! Inserire 'help' per conoscere i comandi disponibili.");
        break;
    }
  }
}

void SerialCLI::printHelp() 
{
  const char * helpString = 
  "Serial Configurator\n"
  "USAGE:\n"
  "help                       stampa questo messaggio di aiuto\n"
  "set <parameter> <value>    imposta un determinato parametro al valore specificato\n"
  "get <parameter>            stampa il valore assegnato al parametro specificato\n"
  "get -a                     stampa tutti i parametri con il loro valore\n"
  "save                       salva i parametdi di configurazione\n"
  "clear                      cancella la configurazione salvata\n"
  "exit                       chiude la configurazione\n"
  "Parametri disponibili:";

  Serial.println(helpString);
  configurator.printAllParams();
}