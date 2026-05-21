#pragma once
#include <cstddef>

struct Parameters {
    char id[16];
    char ip[16];
    char port[6];

    void resetDefaults();
    static const Parameters& defaults();
    bool validateID(const char* val) const;
    bool validateIP(const char* val) const;
    bool validatePort(const char* val) const;
    bool validateAny(const char* val) const;
};
struct ParamDescriptor 
{
    const char* name;
    void* valuePtr;
    size_t maxSize;
    bool (*validator)(const char*);
    const char* defaultValue;
};
extern const ParamDescriptor paramTable[];
extern const int NUM_PARAMS;
extern Parameters parameters;