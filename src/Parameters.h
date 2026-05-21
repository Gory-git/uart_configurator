#pragma once
#include <cstddef>

struct Parameters {
    char id[16];
    char ip[16];
    char port[6];

    void resetDefaults();
    static const Parameters& defaults();
};

bool validateID(const char*);
bool validateIP(const char*);
bool validatePort(const char*);
bool validateAny(const char*);

struct ParamDescriptor 
{
    const char* name;
    void* valuePtr;
    size_t maxSize;
    bool (*validator)(const char*);
    const char* defaultValue;
};
// extern const ParamDescriptor paramTable[];
// extern const int NUM_PARAMS;
// extern Parameters parameters;