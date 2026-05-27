#pragma once
#include <EEPROMManager.h>
#include <Parameters.h>

class UartConfigurator
{
    public:
        UartConfigurator();
        bool setParameter(const char* name, const char* value);
        bool getParameter(const char* name) const;
        bool save();
        bool load();
        bool clear();
        bool exit();
        bool isConfigComplete();
        bool makeChoice(unsigned long timeoutMs = 0);
        void printAllParams() const;
        Parameters getParameters();
    private:
        EEPROMManager eeprom;
        Parameters parameters;
        bool saved = false;
        ParamDescriptor paramTable[4];  // +1 for sentinel terminator
};