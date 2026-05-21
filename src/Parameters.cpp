#include "Parameters.h"
#include <IPAddress.h>

Parameters parameters;

const Parameters defaultParameters = 
{
    "void",
    "000.000.000.000",
    "000",
};

void Parameters::resetDefaults()
{
    *this = defaultParameters;
}

const Parameters& Parameters::defaults()
{
    return defaultParameters;
}

bool validateIP(const char* value)
{  
  IPAddress ip;  return ip.fromString(value);
}

bool validateID(const char* value)
{  
  return strlen(value) >= 3;
}

bool validatePort(const char* value)
{  
  int p = atoi(value);  
  return p > 0 && p <= 65535;
}

bool validateAny(const char* value)
{  
  return value != NULL && strlen(value) > 0;
}

const ParamDescriptor paramTable[] = 
{  
  {"id", parameters.id, sizeof(parameters.id), validateID, "void"},  
  {"ip", parameters.ip, sizeof(parameters.ip), validateIP, "000.000.000.000"},  
  {"port", parameters.port, sizeof(parameters.port), validatePort, "0"}
};

const int NUM_PARAMS = sizeof(paramTable) / sizeof(paramTable[0]);