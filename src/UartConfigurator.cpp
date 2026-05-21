#include "UartConfigurator.h"

UartConfigurator::UartConfigurator() : parameters(), eeprom(), saved(false)
{
    paramTable[0] = {"id", parameters.id, sizeof(parameters.id), validateID, "void"};
    paramTable[1] = {"ip", parameters.ip, sizeof(parameters.ip), validateIP, "000.000.000.000"};
    paramTable[2] = {"port", parameters.port, sizeof(parameters.port), validatePort, "0"};

  if (!eeprom.load(parameters)) 
  {
      parameters.resetDefaults();
      saved = false;
  } else 
  {
      Serial.println("Configurazione caricata da EEPROM:");    
      for (int i = 0; i < NUM_PARAMS; i++)    
      {      
          Serial.printf("  %s: %s\n", paramTable[i].name, (char*)paramTable[i].valuePtr);    
      }    
      Serial.println();
      saved = true;
  }
}

bool UartConfigurator::setParameter(const char* paramName, const char* value)
{
  if (paramName == NULL)
  {
    Serial.println("Attenzione! Un parametro dev'essere fornito!");
    return false;
  }
  for (int i = 0; i < NUM_PARAMS; i++)
  {
    if (strcmp(paramName, paramTable[i].name) == 0)
    {
      if (value == NULL)
      {
        Serial.printf("Attenzione! Un valore per %s dev'essere fornito!\n", paramName);
        return false;
      }
      if (paramTable[i].validator && !paramTable[i].validator(value))
      {
        Serial.printf("Attenzione! Valore non valido per '%s'!\n", paramName);        
        return false;
      }

      strncpy((char*)paramTable[i].valuePtr, value, paramTable[i].maxSize - 1);      
      ((char*)paramTable[i].valuePtr)[paramTable[i].maxSize - 1] = '\0';      
      Serial.printf("%s impostato a %s\n", paramName, value);
      saved = false;      
      return true;

    }
  }
  Serial.println("ATTENZIONE: Parametro sconosciuto!");
  return false;
}

bool UartConfigurator::getParameter(const char* paramName) const
{
    if (paramName == NULL)
    {
        Serial.println("Attenzione! Un parametro dev'essere fornito!");
        return false;
    }
    if (strcmp(paramName, "-a") == 0)
    {
        for (int i = 0; i < NUM_PARAMS; i++)
        {
            Serial.printf("%s:  %s\n", paramTable[i].name, (char*)paramTable[i].valuePtr);
        }
        return true;
    }
    for (int i = 0; i < NUM_PARAMS; i++)   
    {    
        if (strcmp(paramTable[i].name, paramName) == 0)     
        {      
            Serial.printf("%s:  %s\n", paramTable[i].name, (char*)paramTable[i].valuePtr);
            return true;    
        }  
    }  
    Serial.println("ATTENZIONE: Parametro sconosciuto!");
    return false;
}

bool UartConfigurator::save()
{  
    if (saved)
    {
        Serial.println("Nulla da salvare!");
        return false;
    }
    if (!isConfigComplete())
    {
        Serial.print("Ci sono parametri non ancora inizializzati, ");
    }
        Serial.print("Proseguire?[y/n]\n");
    if (makeChoice()) 
    {
        Serial.println("Avvio procedura di salvataggio...");
        if (eeprom.save(parameters))
        {      
            saved = true;
            Serial.println("Configurazione salvata con successo!");    
            return true;
        }    
        else    
        {      
            Serial.println("ERRORE: Salvataggio fallito!");    
            return false;
        }
    }
    return false;
}

bool UartConfigurator::clear()
{
    Serial.println("ATTENZIONE! Stai per cancellare la configurazione salvata. Proseguire?[y/n]");  
    if (makeChoice())  
    {    
        eeprom.clear();
        for (int i = 0; i < NUM_PARAMS; i++)
        {
            strncpy((char*)paramTable[i].valuePtr, (char*)paramTable[i].defaultValue, paramTable[i].maxSize - 1);      
            ((char*)paramTable[i].valuePtr)[paramTable[i].maxSize - 1] = '\0';      
            Serial.printf("%s impostato a %s\n", (char*)paramTable[i].name, (char*)paramTable[i].defaultValue);
        }
        saved = false;
        return true; 
    }  
    return false;
}


bool UartConfigurator::load()
{
    return eeprom.load(parameters);
}

bool UartConfigurator::exit()
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
      save();
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

bool UartConfigurator::isConfigComplete()
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

bool UartConfigurator::makeChoice(unsigned long timeoutMs)
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

void UartConfigurator::printAllParams() const
{
    for (int i = 0; i < NUM_PARAMS; i++)
    {
        Serial.printf("\t%s (default: %s)\n", paramTable[i].name, paramTable[i].defaultValue);
    }
}