#include <Arduino.h>
#include <string.h>
#include <IPAddress.h>
#include <EEPROM.h>

#if defined(ESP32)  
  #include <esp_crc.h>  
  #define CALC_CRC32(data, len) esp_crc32_le(0, (uint8_t*)(data), (len))
  #else 
  uint32_t crc32(const uint8_t* data, size_t length)  
  {    uint32_t crc = 0xFFFFFFFF;    
    for (size_t i = 0; i < length; i++)    
    {      
      crc ^= data[i];      
      for (uint8_t j = 0; j < 8; j++)      
      {        
        crc = (crc >> 1) ^ (0xEDB88320 & -(crc & 1));      
      }    
    }    
    return ~crc;  
  }  
  #define CALC_CRC32(data, len) crc32((const uint8_t*)(data), (len))
#endif

#define EEPROM_SIZE 512        // Dimensione totale (su ESP32/ESP8266)
#define EEPROM_START_ADDR 0    // Indirizzo di inizio
#define MAGIC_NUMBER 0xABCD

const char * helpString = 
"Serial Configurator\n"
"USAGE:\n"
"help                       stampa questo messaggio di aiuto\n"
"set <parameter> <value>    imposta un determinato parametro al valore specificato\n"
"get <parameter>            stampa il valore assegnato al parametro specificato\n"
"get -a                     stampa tutti i parametri con il loro valore\n"
"save                       salva i parametdi di configurazione\n"
"clear                      cancella la confiurazione salvata\n"
"exit                       chiude la configurazione\n"
"Parametri disponibili:";

enum class Command 
{
  HELP,
  SET,
  GET,
  SAVE,
  CLEAR,
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

struct EEPROMData 
{  
  uint16_t magic;  
  Parameters params;  
  uint32_t crc;
};

bool validateID(const char* value);
bool validateIP(const char* value);
bool validatePort(const char* value);

struct Parameters parameters = 
{
  "void", 
  "000.000.000.000",
  "0",
};

const ParamDescriptor paramTable[] = 
{  
  {"id", parameters.id, sizeof(parameters.id), validateID, "void"},  
  {"ip", parameters.ip, sizeof(parameters.ip), validateIP, "000.000.000.000"},  
  {"port", parameters.port, sizeof(parameters.port), validatePort, "0"}
};
const int NUM_PARAMS = sizeof(paramTable) / sizeof(paramTable[0]);
bool saved = false;

// put function declarations here:
Command getCommand(const char* command);
void serialCLI();
bool saveParametersOnEEPROM();
bool loadParametersFromEEPROM();
void clearEEPROM();
bool makeChoice(unsigned long timeoutMs = 0);

void setup() 
{
  Serial.begin(115200);
  if (loadParametersFromEEPROM())  
  {    
    Serial.println("Configurazione caricata da EEPROM:");    
    for (int i = 0; i < NUM_PARAMS; i++)    
    {      
      Serial.printf("  %s: %s\n", paramTable[i].name, (char*)paramTable[i].valuePtr);    
    }    
    Serial.println();    
    saved = true;
    Serial.println("Modificare parametri di configurazione?[y/n]");
    if (makeChoice(10000)) 
    {
      serialCLI();
    }
    } else  
    {    
      Serial.println("Nessuna configurazione trovata, usando valori di default. Avvio CLI...\n");  
      serialCLI();
    }
  Serial.print("Configurazione terminata!");
}

void loop() 
{

}

bool saveParametersOnEEPROM()
{  
  EEPROMData data;  
  data.magic = MAGIC_NUMBER;  
  data.params = parameters;  
  data.crc = CALC_CRC32(&data.params, sizeof(Parameters));    
  #if defined(ESP32) || defined(ESP8266)    
    EEPROM.begin(EEPROM_SIZE);  
  #endif    
  EEPROM.put(EEPROM_START_ADDR, data);    
  #if defined(ESP32) || defined(ESP8266)    
    bool success = EEPROM.commit();    
    EEPROM.end();    
    return success;  
  #else    
    return true;  
  #endif
}

bool loadParametersFromEEPROM()
{  
  EEPROMData data;    
  #if defined(ESP32) || defined(ESP8266)    
    EEPROM.begin(EEPROM_SIZE);  
  #endif    
  EEPROM.get(EEPROM_START_ADDR, data);    
  #if defined(ESP32) || defined(ESP8266)    
    EEPROM.end();  
  #endif    
  if (data.magic != MAGIC_NUMBER)  
  {    
    Serial.println("EEPROM non inizializzata");    
    return false;  
  }    
  uint32_t calculatedCRC = CALC_CRC32(&data.params, sizeof(Parameters));  
  if (data.crc != calculatedCRC)  
  {    
    Serial.println("EEPROM corrotta");    
    return false;  
  }    
  parameters = data.params;  
  return true;
}

void clearEEPROM()
{
  #if defined(ESP32) || defined(ESP8266)
    EEPROM.begin(EEPROM_SIZE);
  #endif
  
  for (int i = 0; i < sizeof(EEPROMData); i++)
  {
    EEPROM.write(EEPROM_START_ADDR + i, 0xFF);
  }
  
  #if defined(ESP32) || defined(ESP8266)
    EEPROM.commit();
    EEPROM.end();
  #endif
  
  Serial.println("EEPROM cancellata");
}


Command getCommand(const char* command)
{
  if (strcmp(command, "help") == 0) return Command::HELP;
  if (strcmp(command, "set") == 0) return Command::SET;
  if (strcmp(command, "get") == 0) return Command::GET;
  if (strcmp(command, "save") == 0) return Command::SAVE;
  if (strcmp(command, "clear") == 0) return Command::CLEAR;
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

bool makeChoice(unsigned long timeoutMs)
{
  unsigned long startTime = millis();
  unsigned long lastPrint = 0;
  
  while (!Serial.available()) 
  {
    delay(10);
    
    // Se timeout attivo
    if (timeoutMs > 0)
    {
      unsigned long elapsed = millis() - startTime;
      
      // Timeout scaduto
      if (elapsed >= timeoutMs)
      {
        Serial.println("\nTimeout scaduto. Operazione annullata.");
        return false;
      }
      
      // Stampa countdown ogni secondo
      if (elapsed - lastPrint >= 1000)
      {
        unsigned long remaining = (timeoutMs - elapsed) / 1000;
        Serial.printf("\rTempo rimanente: %lu secondi\n", remaining);
        lastPrint = elapsed;
      }
    }
  }
  
  Serial.println();
  
  String input = Serial.readStringUntil('\n');  
  input.trim();    
  
  if (input.length() == 0)   
  {    
    Serial.println("Operazione annullata.");    
    return false;  
  }
  
  char choice = toupper(input.charAt(0));
  
  if (choice == 'Y')
  {
    return true;
  } 
  else
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
    Serial.printf("\t%s (default: %s)\n", paramTable[i].name, paramTable[i].defaultValue);
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
      saved = false;      
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
  if (saved)
  {
    Serial.println("Nulla da salvare!");
    return;
  }
  if (!isConfigComplete())
  {
    Serial.print("Ci sono parametri non ancora inizializzati, ");
  }
  Serial.print("Proseguire?[y/n]\n");
  if (makeChoice()) 
  {
    Serial.println("Avvio procedura di salvataggio...");
    if (saveParametersOnEEPROM())    
    {      
      saved = true;
      Serial.println("Configurazione salvata con successo!");    
    }    
    else    
    {      
      Serial.println("ERRORE: Salvataggio fallito!");    
    }
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
    Serial.println("Attenzione! Ci sono modifiche non salvate, vuoi salvare ora?[y/n]");
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
        handleSet(parameter, value);
        break;
      case Command::GET:
        parameter = strtok(NULL, delimiter);
        handleGet(parameter);
        break;
      case Command::SAVE:
        handleSave();
        break;
      case Command::CLEAR:
        Serial.println("ATTENZIONE! Stai per cancellare la configurazione salvata. Proseguire?[y/n]");  
        if (makeChoice())  
        {    
          clearEEPROM();
          for (int i = 0; i < NUM_PARAMS; i++)
          {
            strncpy((char*)paramTable[i].valuePtr, (char*)paramTable[i].defaultValue, paramTable[i].maxSize - 1);      
            ((char*)paramTable[i].valuePtr)[paramTable[i].maxSize - 1] = '\0';      
            Serial.printf("%s impostato a %s\n", (char*)paramTable[i].name, (char*)paramTable[i].defaultValue);
          }
          saved = false;  
        }  
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
