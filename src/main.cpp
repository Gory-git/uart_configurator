#include <Arduino.h>
#include <string.h>
#include <IPAddress.h>

const char * helpString = 
"Serial Configurator\n"
"USAGE:\n"
"help                       stampa questo messaggio di aiuto\n"
"set <parameter> <value>    imposta un determinato parametro al valore specificato\n"
"get <parameter>            stampa il valore assegnato al parametro specificato\n"
"get -a                     stampa tutti i parametri con il loro valore\n"
"save                       salva i parametdi di configurazione\n"
"exit                       chiude la configurazione\n"
"Parametri disponibili:";

enum class Command 
{
  HELP,
  SET,
  GET,
  SAVE,
  EXIT,
  UNKNOWN
};

struct ParamDescriptor
{
  const char* name;
  void* valuePtr;
  size_t maxSize;
  bool (*validator)(const char*);
  const char* defaultValue;
};

struct Parameters
{
  char id[16];
  char ip[16];
  char port[6];
};

struct Parameters parameters = 
{
  "void", 
  "000.000.000.000",
  "8080",
};

const ParamDescriptor paramTable[] = 
{  
  {"id", parameters.id, sizeof(parameters.id), validateID, "void"},  
  {"ip", parameters.ip, sizeof(parameters.ip), validateIP, "000.000.000.000"},  
  {"port", parameters.port, sizeof(parameters.port), validatePort, "8080"}
};
const int NUM_PARAMS = sizeof(paramTable) / sizeof(paramTable[0]);
bool saved = false;

// put function declarations here:
bool validateID(const char* value);
bool validateIP(const char* value);
bool validatePort(const char* value);
Command getCommand(const char* command);
void serialCLI();
bool makeChoice();

void setup() 
{
  Serial.begin(115200);
  serialCLI();
}

void loop() 
{

}

Command getCommand(const char* command)
{
  if (strcmp(command, "help") == 0) return Command::HELP;
  if (strcmp(command, "set") == 0) return Command::SET;
  if (strcmp(command, "get") == 0) return Command::GET;
  if (strcmp(command, "save") == 0) return Command::SAVE;
  if (strcmp(command, "exit") == 0) return Command::EXIT;
  return Command::UNKNOWN;
}

bool validateIP(const char* value) 
{  
  IPAddress ip;  return ip.fromString(value);
}

bool validateID(const char* value) 
{  
  return strlen(value) >= 3;
}

bool validateAny(const char* value) 
{  
  return value != NULL && strlen(value) > 0;
}

bool validatePort(const char* value) 
{  
  int p = atoi(value);  
  return p > 0 && p <= 65535;
}

bool makeChoice()
{
  while (!Serial.available()) 
  {
    delay(10);    
  }

  char choice = Serial.read();

  while (Serial.available()) 
  {        
    choice = Serial.read();    
  }

  choice = toupper(choice);
  
  if (choice == 'Y')
  {
    return true;
  } else
  {
    Serial.println("Operazione annullata.");  
    return false;
  }
}

void printHelp()
{
  Serial.println(helpString);
  for (int i = 0; i < NUM_PARAMS; i++)
  {
    Serial.printf("\t%s (default: %s)", paramTable[i].name, paramTable[i].defaultValue);
  }
}

void handleSet(const char* paramName, const char* value)
{        
  if (paramName == NULL)
  {
    Serial.println("Attenzione! Un parametro dev'essere fornito!");
    return;
  }


  for (int i = 0; i < NUM_PARAMS; i++)
  {
    if (strcmp(paramName, paramTable[i].name) == 0)
    {
      if (value == NULL)
      {
        Serial.printf("Attenzione! Un valore per %s dev'essere fornito!\n", paramName);
        return;
      }
      if (paramTable[i].validator && !paramTable[i].validator(value))
      {
        Serial.printf("Attenzione! Valore non valido per '%s'!\n", paramName);        
        return;
      }

      strncpy((char*)paramTable[i].valuePtr, value, paramTable[i].maxSize - 1);      
      ((char*)paramTable[i].valuePtr)[paramTable[i].maxSize - 1] = '\0';      
      Serial.printf("%s impostato a %s\n", paramName, value);      
      return;

    }
  }
  Serial.println("ATTENZIONE: Parametro sconosciuto!");
}

void handleGet(const char* paramName)
{
  if (paramName == NULL)
  {
    Serial.println("Attenzione! Un parametro dev'essere fornito!");
    return;
  }
  if (strcmp(paramName, "-a") == 0)
  {
    for (int i = 0; i < NUM_PARAMS; i++)
    {
      Serial.printf("%s:  %s\n", paramTable[i].name, (char*)paramTable[i].valuePtr);
    }
    return;
  }
  for (int i = 0; i < NUM_PARAMS; i++)   
  {    
    if (strcmp(paramTable[i].name, paramName) == 0)     
    {      
      Serial.printf("%s:  %s\n", paramTable[i].name, (char*)paramTable[i].valuePtr);
      return;    
    }  
  }  
  Serial.println("ATTENZIONE: Parametro sconosciuto!");
}

bool isConfigComplete() 
{
  for (int i = 0; i < NUM_PARAMS; i++) 
  {
    if (strcmp((char*)paramTable[i].valuePtr, paramTable[i].defaultValue) == 0) 
    {
      return false;
    }
  }
  return true;
}

void handleSave()
{
  if (!isConfigComplete())
  {
    Serial.print("Ci sono parametri non ancora inizializzati,");
  }
  Serial.print("Proseguire?[y/n]\n");
  if (makeChoice()) 
  {
    Serial.println("Avvio procedura di salvataggio...");
    // saveParametersOnEEPROM(parameters);
  }
}

bool handleExit()
{
  if (!isConfigComplete())
  {
    Serial.println("Impossibile proseguire, configurazione non terminata. Ritentare dopo aver inizializzato tutti i parametri!");
    return false;
  }
  if (!saved)
  {
    Serial.println("Attenzione! Non hai ancora salvato, vuoi salvare ora?[y/n]");
    if (makeChoice()) 
    {
      handleSave();
    } 
  }
  Serial.println("Uscire dal prompt di configurazione?[y/n]");
  if (makeChoice()) 
  {
    Serial.println("Exiting...");
    return true;
  }
  return false;
}

void serialCLI()
{
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
    IPAddress ipA;
    token = strtok(buffer, delimiter);
    if (token == NULL) 
    {
      //Serial.println("Attenzione! Un comando dev'essere fornito!");
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
        handleSet(parameter, value);
        break;
      case Command::GET:
        parameter = strtok(NULL, delimiter);
        handleGet(parameter);
        break;
      case Command::SAVE:
        handleSave();
        break;
      case Command::EXIT:
        if (handleExit()) return;
        break;
      default:
        Serial.println("ATTENZIONE: Comando sconosciuto! Inserire 'help' per conoscere i comandi disponibili.");
        break;
    }
  }
}
