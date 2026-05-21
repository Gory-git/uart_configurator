#pragma once
#include <EEPROM.h>
#include <Parameters.h>

class EEPROMManager
{
    public:
        EEPROMManager();
        bool save(const Parameters& params);
        bool load(Parameters& parrams);
        void clear();
    private:
        uint32_t calcCRC(const Parameters& params) const;
};