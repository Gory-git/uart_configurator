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
    private:
        Parameters parameters;
        EEPROMManager eeprom;
        bool saved = false;
        static constexpr int NUM_PARAMS = 3;
        ParamDescriptor paramTable[3];
};