#pragma once
#include "UartConfigurator.h"

class SerialCLI {
    public:
        SerialCLI();
        void begin();

    private:
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
        void handleInput();
        UartConfigurator configurator;
        void printHelp();
        Command getCommand(const char* command);
};