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
"";

enum class Command 
{
  HELP,
  SET,
  GET,
  SAVE,
  EXIT,
  UNKNOWN
};

enum class Parameter 
{
  ID,
  IP,
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
};



// put function declarations here:
Command getCommand(const char* command);

Parameter getParameter(const char* parameter);

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

Parameter getParameter(const char* parameter)
{
  if (strcmp(parameter, "id") == 0) return Parameter::ID;
  if (strcmp(parameter, "ip") == 0) return Parameter::IP;
  return Parameter::UNKNOWN;
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

  while (Serial.available() && (choice == '\n' || choice == '\r')) 
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
    char *ip = NULL;
    char *id = NULL;
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
        Serial.println(helpString);
        break;
      case Command::SET:
        parameter = strtok(NULL, delimiter);
        if (parameter == NULL)
        {
          Serial.println("Attenzione! Un parametro dev'essere fornito!");
          break;
        }
        switch (getParameter(parameter))
        {
          case Parameter::ID :
            id = strtok(NULL, delimiter);
            if (id == NULL)
            {
              Serial.println("Attenzione! Un valore per l'ID dev'essere fornito!");
              break;
            }
            strcpy(parameters.id, id);
            Serial.printf("ID set to %s\n", id);
            break;
          case Parameter::IP:
            ip = strtok(NULL, delimiter);
            if (ip== NULL)
            {
              Serial.println("Attenzione! Un valore per l'IP dev'essere fornito!");
              break;
            }
            if (!ipA.fromString(ip))
            {
              Serial.println("Attenzione! Fornire un IP valido!");
              break;
            }
            strcpy(parameters.ip, ip);
            Serial.printf("IP set to %s\n", ip);
            break;
          default:
            Serial.println("ATTENZIONE: Parametro sconosciuto!");
            break;
        }
        break;  
      case Command::GET:
        parameter = strtok(NULL, delimiter);
        if (parameter == NULL)
        {
          Serial.println("Attenzione! Un parametro dev'essere fornito!");
          break;
        }
        if (strcmp(parameter, "-a") == 0)
        {
            Serial.printf("ID: %s\n", parameters.id);

            Serial.printf("IP: %s\n", parameters.ip);
            break;
        }
        switch (getParameter(parameter))
        {
          case Parameter::ID :
            Serial.println(parameters.id);
            break;
          case Parameter::IP:
            Serial.println(parameters.ip);
            break;
          default:
            Serial.println("ATTENZIONE: Parametro sconosciuto!");
            break;
        }
        break;
      case Command::SAVE:
        if (strcmp(parameters.id, "void") == 0) Serial.print("ID non inizializzato, ");
        if (strcmp(parameters.ip, "000.000.000.000") == 0) Serial.print("IP non inizializzato, ");
        Serial.print("Proseguire?[y/N]\n");
        if (makeChoice()) 
        {
          Serial.println("Avvio procedura di salvataggio...");
          // saveParametersOnEEPROM(parameters);
        }

        break;
      case Command::EXIT:
        
        if (strcmp(parameters.id, "void") == 0 || strcmp(parameters.ip, "000.000.000.000") == 0)
        {
          Serial.println("Impossibile proseguire, configurazione non terminata. Ritentare dopo aver inizializzato tutti i parametri!");
        } else
        {
          Serial.println("Uscire dal prompt di configurazione?[y/N]");
          if (makeChoice()) 
          {
            Serial.println("Exiting...");
            return;
          }
        }
        break;
      default:
        Serial.println("ATTENZIONE: Comando sconosciuto! Inserire 'help' per conoscere i comandi disponibili.");
        break;
    }
  }
}
