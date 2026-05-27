#include "UartConfigurator.h"

UartConfigurator::UartConfigurator() : parameters(), eeprom(), saved(false)
{
    paramTable[0] = {"id", parameters.id, sizeof(parameters.id), validateID, "void"};
    paramTable[1] = {"ip", parameters.ip, sizeof(parameters.ip), validateIP, "000.000.000.000"};
    paramTable[2] = {"port", parameters.port, sizeof(parameters.port), validatePort, "0"};
    paramTable[3] = {nullptr, nullptr, 0, nullptr, nullptr};  // Sentinel terminator

  if (!eeprom.load(parameters)) 
  {
      parameters.resetDefaults();
      saved = false;
  } else 
  {
      Serial.println("Configuration loaded from EEPROM:");    
      for (int i = 0; paramTable[i].name != nullptr; i++)    
      {      
          Serial.printf("  %s: %s\n", paramTable[i].name, (char*)paramTable[i].valuePtr);    
      }    
      Serial.println();
      saved = true;
  }
}

Parameters UartConfigurator::getParameters()
{
  return parameters;
}

bool UartConfigurator::setParameter(const char* paramName, const char* value)
{
  if (paramName == NULL)
  {
    Serial.println("Warning! A parameter name must be provided!");
    return false;
  }
  for (int i = 0; paramTable[i].name != nullptr; i++)
  {
    if (strcmp(paramName, paramTable[i].name) == 0)
    {
      if (value == NULL)
      {
        Serial.printf("Warning! A value for %s must be provided!\n", paramName);
        return false;
      }
      if (paramTable[i].validator && !paramTable[i].validator(value))
      {
        Serial.printf("Warning! Invalid value for '%s'!\n", paramName);        
        return false;
      }

      strncpy((char*)paramTable[i].valuePtr, value, paramTable[i].maxSize - 1);      
      ((char*)paramTable[i].valuePtr)[paramTable[i].maxSize - 1] = '\0';      
      Serial.printf("%s set to %s\n", paramName, value);
      saved = false;      
      return true;

    }
  }
  Serial.println("ERROR: Unknown parameter!");
  return false;
}

bool UartConfigurator::getParameter(const char* paramName) const
{
    if (paramName == NULL)
    {
        Serial.println("Warning! A parameter name must be provided!");
        return false;
    }
    if (strcmp(paramName, "-a") == 0)
    {
        for (int i = 0; paramTable[i].name != nullptr; i++)
        {
            Serial.printf("%s:  %s\n", paramTable[i].name, (char*)paramTable[i].valuePtr);
        }
        return true;
    }
    for (int i = 0; paramTable[i].name != nullptr; i++)   
    {    
        if (strcmp(paramTable[i].name, paramName) == 0)     
        {      
            Serial.printf("%s:  %s\n", paramTable[i].name, (char*)paramTable[i].valuePtr);
            return true;    
        }  
    }  
    Serial.println("ERROR: Unknown parameter!");
    return false;
}

bool UartConfigurator::save()
{  
    if (saved)
    {
        Serial.println("Nothing to save!");
        return false;
    }
    if (!isConfigComplete())
    {
        Serial.print("There are parameters that have not been initialized yet. ");
    }
        Serial.print("Continue? [y/n]\n");
    if (makeChoice()) 
    {
        Serial.println("Starting save procedure...");
        if (eeprom.save(parameters))
        {      
            saved = true;
            Serial.println("Configuration saved successfully!");    
            return true;
        }    
        else    
        {      
            Serial.println("ERROR: Save failed!");    
            return false;
        }
    }
    return false;
}

bool UartConfigurator::clear()
{
    Serial.println("WARNING! You are about to delete the saved configuration. Continue? [y/n]");  
    if (makeChoice())  
    {    
        eeprom.clear();
        for (int i = 0; paramTable[i].name != nullptr; i++)
        {
            strncpy((char*)paramTable[i].valuePtr, (char*)paramTable[i].defaultValue, paramTable[i].maxSize - 1);      
            ((char*)paramTable[i].valuePtr)[paramTable[i].maxSize - 1] = '\0';      
            Serial.printf("%s set to %s\n", (char*)paramTable[i].name, (char*)paramTable[i].defaultValue);
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
    Serial.println("Cannot proceed, configuration incomplete. Try again after initializing all parameters!");
    return false;
  }
  if (!saved)
  {
    Serial.println("Warning! You have unsaved changes. Save now? [y/n]");
    if (makeChoice()) 
    {
      save();
    } 
  }
  Serial.println("Exit configuration prompt? [y/n]");
  if (makeChoice()) 
  {
    Serial.println("Exiting...");
    return true;
  }
  return false;
}

bool UartConfigurator::isConfigComplete()
{
  for (int i = 0; paramTable[i].name != nullptr; i++) 
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
    
    // If timeout is active
    if (timeoutMs > 0)
    {
      unsigned long elapsed = millis() - startTime;
      
      // Timeout expired
      if (elapsed >= timeoutMs)
      {
        Serial.println("\nTimeout expired. Operation cancelled.");
        return false;
      }
      
      // Print countdown every second
      if (elapsed - lastPrint >= 1000)
      {
        unsigned long remaining = (timeoutMs - elapsed) / 1000;
        Serial.printf("\rTime remaining: %lu seconds\n", remaining);
        lastPrint = elapsed;
      }
    }
  }
  
  Serial.println();
  
  String input = Serial.readStringUntil('\n');  
  input.trim();    
  
  if (input.length() == 0)   
  {    
    Serial.println("Operation cancelled.");    
    return false;  
  }
  
  char choice = toupper(input.charAt(0));
  
  if (choice == 'Y')
  {
    return true;
  } 
  else
  {
    Serial.println("Operation cancelled.");  
    return false;
  }
}

void UartConfigurator::printAllParams() const
{
    for (int i = 0; paramTable[i].name != nullptr; i++)
    {
        Serial.printf("\t%s (default: %s)\n", paramTable[i].name, paramTable[i].defaultValue);
    }
}