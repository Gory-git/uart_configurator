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
