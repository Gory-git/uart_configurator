#include "EEPROMManager.h"
#include "Parameters.h"

#define EEPROM_SIZE 512        // Dimensione totale (su ESP32/ESP8266)
#define EEPROM_START_ADDR 0    // Indirizzo di inizio
#define MAGIC_NUMBER 0xABCD

struct EEPROMData 
{  
    uint16_t magic;  
    Parameters params;  
    uint32_t crc;
};

static uint32_t crc32(const uint8_t* data, size_t length)  
{    
    uint32_t crc = 0xFFFFFFFF;    
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

EEPROMManager::EEPROMManager() {}

uint32_t EEPROMManager::calcCRC(const Parameters& params) const
{
    // Calcola il CRC dei parametri
    return crc32(reinterpret_cast<const uint8_t*>(&params), sizeof(Parameters));
}

bool EEPROMManager::save(const Parameters& parameters)
{
    EEPROMData data;  
    data.magic = MAGIC_NUMBER;  
    data.params = parameters;  
    data.crc = calcCRC(data.params);    
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

bool EEPROMManager::load(Parameters& parameters)
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
    uint32_t calculatedCRC = calcCRC(data.params);  
    if (data.crc != calculatedCRC)  
    {    
    Serial.println("EEPROM corrotta");    
    return false;  
    }    
    parameters = data.params;  
    return true;
}

void EEPROMManager::clear()
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