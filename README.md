# 🔧 UART Configurator

![C++](https://img.shields.io/badge/Language-C%2B%2B-blue?style=flat-square) ![Platform](https://img.shields.io/badge/Platform-ESP32-orange?style=flat-square) ![License](https://img.shields.io/badge/License-MIT-green?style=flat-square)

A powerful, production-ready firmware configuration system for ESP32 microcontrollers that allows parameters management via UART serial interface, with persistent storage on EEPROM.

## 📋 Table of Contents

- [Overview](#overview)
- [Key Features](#key-features)
- [Use Cases](#use-cases)
- [Architecture](#architecture)
- [Project Structure](#project-structure)
- [Requirements](#requirements)
- [Installation & Setup](#installation--setup)
- [Quick Start](#quick-start)
- [CLI Commands Reference](#cli-commands-reference)
- [Configuration Management](#configuration-management)
  - [Default Parameters](#default-parameters)
  - [Adding New Parameters](#adding-new-parameters)
  - [Creating Custom Validators](#creating-custom-validators)
- [Examples](#examples)
- [Data Integrity](#data-integrity)
- [Advanced Features](#advanced-features)
- [Troubleshooting](#troubleshooting)
- [Contributing](#contributing)
- [License](#license)

---

## 📖 Overview

**UART Configurator** is a modular firmware parameter management system designed for IoT and embedded applications where you need to deploy identical firmware across multiple devices while allowing each device to have unique configuration parameters.

Instead of recompiling firmware for each device, UART Configurator lets you:
- ✅ Configure parameters via a serial terminal at runtime
- ✅ Persist configurations to EEPROM with CRC validation
- ✅ Validate input data before saving
- ✅ Easily add new configurable parameters
- ✅ Recover configurations from EEPROM on boot

### Real-World Scenario

Imagine you have 50 IoT devices in the field, each needing:
- Different **server IP addresses**
- Different **port numbers**
- Unique **device IDs**
- Different **sensor polling intervals**

Without UART Configurator, you'd need to recompile firmware 50 times. With it, you compile once and configure each device via serial connection in minutes.

---

## ✨ Key Features

| Feature | Description |
|---------|-------------|
| **Serial CLI Interface** | Interactive command-line interface over UART (115200 baud) |
| **Parameter Validation** | Built-in validators (IP, port range, string length) + extensible |
| **EEPROM Persistence** | Automatic saving and loading of configurations with CRC32 integrity check |
| **Smart Startup Flow** | Asks user to modify config on boot if one is found, uses defaults otherwise |
| **Type-Safe Storage** | Parameters stored in strongly-typed `Parameters` struct |
| **Timeout Support** | Optional countdown timer for batch configuration operations |
| **Change Tracking** | Prevents accidental saves; tracks unsaved changes |
| **User Feedback** | Comprehensive validation error messages in Italian |
| **Easy Extensibility** | Simple pattern for adding new parameters and validators |

---

## 🎯 Use Cases

This project is perfect for:

- **IoT Nodes**: Devices connecting to different WiFi networks or servers
- **Sensor Networks**: Multiple sensors with different calibration parameters and poll rates
- **Industrial Equipment**: Machines with device-specific configuration (IDs, communication params)
- **Field Deployments**: When you need to configure hundreds of devices without recompiling
- **Development & Testing**: Quickly change parameters without flashing new firmware
- **Edge Computing**: Distributed nodes with unique network and storage endpoints

---

## 🏗️ Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                      SerialCLI (User Interface)             │
│  Handles: prompt, command parsing, help display             │
└──────────────────┬──────────────────────────────────────────┘
                   │
                   ▼
┌─────────────────────────────────────────────────────────────┐
│               UartConfigurator (Core Logic)                 │
│  Handles: parameter validation, state management, CLI cmds  │
└──────┬──────────────────────────────────────────────────────┘
       │
       ├────────────────────────────────────────┐
       │                                        │
       ▼                                        ▼
┌──────────────────────────┐    ┌─────────────────────────────┐
│   Parameters (Data)      │    │   EEPROMManager (Storage)   │
│  - id: char[16]          │    │  - save(params)             │
│  - ip: char[16]          │    │  - load(params)             │
│  - port: char[6]         │    │  - clear()                  │
│  - validators            │    │  - CRC32 validation         │
│  - defaults              │    │                             │
└──────────────────────────┘    └─────────────────────────────┘
                                                │
                                                ▼
                                        EEPROM (Flash)
```

### Component Overview

| Component | Responsibility |
|-----------|-----------------|
| **SerialCLI** | Manages user interaction via serial, parses commands, displays help |
| **UartConfigurator** | Core business logic: validates inputs, manages parameter table, tracks state |
| **Parameters** | Data struct holding all configurable parameters + default values |
| **EEPROMManager** | Persists parameters to EEPROM with CRC integrity checking |
| **Validators** | Functions ensuring parameter values meet requirements (IP format, port range, etc.) |

---

## 📁 Project Structure

```
uart_configurator/
├── src/
│   ├── SerialCLI.h              # CLI interface header
│   ├── SerialCLI.cpp            # CLI implementation
│   ├── UartConfigurator.h       # Core configurator header
│   ├── UartConfigurator.cpp     # Core configurator implementation
│   ├── Parameters.h             # Parameter struct & validators
│   ├── Parameters.cpp           # Validator implementations
│   ├── EEPROMManager.h          # EEPROM interface header
│   ├── EEPROMManager.cpp        # EEPROM implementation with CRC
│   └── main.cpp                 # Entry point (if applicable)
├── include/                     # Additional headers (if needed)
├── lib/                         # External libraries
├── test/                        # Test files (future)
├── platformio.ini               # PlatformIO configuration
├── .gitignore                   # Git ignore rules
└── README.md                    # This file
```

---

## 📋 Requirements

### Hardware
- **MCU**: ESP32 (or any Arduino-compatible with EEPROM)
- **Serial Connection**: USB-to-UART adapter (typically built into ESP32 dev boards)
- **EEPROM**: Internal ESP32 flash (NVS - Non-Volatile Storage)

### Software
- **Framework**: Arduino (ESP32 core)
- **Build System**: PlatformIO (recommended) or Arduino IDE
- **Compiler**: GCC for ARM (included with ESP32 toolchain)

### Dependencies
```ini
platform = espressif32
board = upesy_wroom        # or your ESP32 variant
framework = arduino
lib_deps = 
    # Standard Arduino libraries (included)
    # - EEPROM
    # - IPAddress
```

---

## 🚀 Installation & Setup

### Option 1: Using PlatformIO (Recommended)

1. **Install PlatformIO** (VS Code extension or CLI)

2. **Clone the repository**
   ```bash
   git clone https://github.com/Gory-git/uart_configurator.git
   cd uart_configurator
   ```

3. **Update `platformio.ini`** for your board (if different from `upesy_wroom`)
   ```ini
   [env:your_board]
   platform = espressif32
   board = esp32devkit    # replace with your board
   framework = arduino
   ```

4. **Build and Upload**
   ```bash
   pio run -t upload       # Build and upload to device
   pio device monitor      # Open serial monitor (115200 baud)
   ```

### Option 2: Using Arduino IDE

1. Clone/download the repository
2. Copy `.cpp` and `.h` files to your sketch folder
3. Create a `main.cpp` or `.ino` file with:
   ```cpp
   #include "SerialCLI.h"
   
   SerialCLI cli;
   
   void setup() {
       cli.begin();
   }
   
   void loop() {
       // Configurator runs in begin(), main loop can be empty
       // or used for other tasks if needed
   }
   ```
4. Select your board and upload via Arduino IDE

### Option 3: Using Arduino CLI

```bash
arduino-cli compile --fqbn esp32:esp32:esp32 .
arduino-cli upload -p /dev/ttyUSB0 --fqbn esp32:esp32:esp32 .
```

---

## ⚡ Quick Start

### 1. Hardware Setup

Connect your ESP32 to your computer via USB. The serial port will be detected automatically.

### 2. Flash the Firmware

```bash
pio run -t upload
```

### 3. Open Serial Terminal

```bash
pio device monitor
```

Expected output:
```
Configurazione caricata da EEPROM:
  id: void
  ip: 000.000.000.000
  port: 0

Modificare parametri di configurazione?[y/n]
```

### 4. First Configuration

Press `y` to enter the CLI:
```
> help
Serial Configurator
USAGE:
help                       stampa questo messaggio di aiuto
set <parameter> <value>    imposta un determinato parametro al valore specificato
get <parameter>            stampa il valore assegnato al parametro specificato
get -a                     stampa tutti i parametri con il loro valore
save                       salva i parametri di configurazione
clear                      cancella la configurazione salvata
exit                       chiude la configurazione
Parametri disponibili:
	id (default: void)
	ip (default: 000.000.000.000)
	port (default: 0)

> set id DEVICE_001
id impostato a DEVICE_001

> set ip 192.168.1.100
ip impostato a 192.168.1.100

> set port 8080
port impostato a 8080

> get -a
id:  DEVICE_001
ip:  192.168.1.100
port:  8080

> save
Avvio procedura di salvataggio...
Configurazione salvata con successo!

> exit
Uscire dal prompt di configurazione?[y/n]
y
Configurazione terminata!
```

### 5. Next Boot

On the next boot, the saved configuration will be loaded automatically:
```
Configurazione caricata da EEPROM:
  id: DEVICE_001
  ip: 192.168.1.100
  port: 8080

Modificare parametri di configurazione?[y/n]
```

Press `n` to skip configuration and proceed with your application.

---

## 🔌 Parameter Access

### Retrieving Parameters in Your Application

After the UART Configurator finishes setup, you can retrieve the loaded parameters in your `main.cpp` using the `getParameters()` method:

```cpp
#include "SerialCLI.h"

SerialCLI serialCLI;
Parameters localParameters;

void setup() 
{
  // Initialize the configurator
  serialCLI.begin();
  
  // Retrieve the configured parameters
  localParameters = serialCLI.getParameters();
  
  // Use the parameters in your application
  Serial.print("Device ID: ");
  Serial.println(localParameters.id);
  
  Serial.print("Server IP: ");
  Serial.println(localParameters.ip);
  
  Serial.print("Server Port: ");
  Serial.println(localParameters.port);
  
  // Now you can use your parameters to configure your network connection,
  // sensor settings, or any other part of your application
}

void loop()
{
  // Your application logic here
}
```

---

## 📟 CLI Commands Reference

### `help`
Display available commands and parameters.

```
> help
```

### `set <parameter> <value>`
Set a parameter to a new value. Value is validated before storing.

```
> set id MY_SENSOR_01
id impostato a MY_SENSOR_01

> set port 9000
port impostato a 9000
```

**Validation Rules:**
- `id`: Minimum 3 characters
- `ip`: Valid IPv4 address (e.g., `192.168.1.1`)
- `port`: Integer between 1 and 65535

If validation fails:
```
> set port 70000
Attenzione! Valore non valido per 'port'!
```

### `get <parameter>`
Display the current value of a parameter.

```
> get id
id:  MY_SENSOR_01

> get ip
ip:  192.168.1.100
```

### `get -a`
Display all parameters and their current values.

```
> get -a
id:  MY_SENSOR_01
ip:  192.168.1.100
port:  9000
```

### `save`
Save current configuration to EEPROM. Will ask for confirmation if:
- Any parameters are still at their default values
- Configuration hasn't changed since last save

```
> save
Avvio procedura di salvataggio...
Configurazione salvata con successo!
```

### `clear`
Erase saved configuration from EEPROM and reset all parameters to defaults. **Requires confirmation.**

```
> clear
ATTENZIONE! Stai per cancellare la configurazione salvata. Proseguire?[y/n]
y
Configurazione cancellata. Tutti i parametri ripristinati ai valori di default.
```

### `exit`
Exit the configuration prompt. Will ask to save unsaved changes.

```
> exit
Uscire dal prompt di configurazione?[y/n]
y
Exiting...
Configurazione terminata!
```

---

## ⚙️ Configuration Management

### Default Parameters

Currently configured parameters (in `src/Parameters.cpp`):

| Parameter | Type | Size | Default | Validator | Use Case |
|-----------|------|------|---------|-----------|----------|
| `id` | char[16] | 16 bytes | `"void"` | `validateID` (min 3 chars) | Device identifier, sensor name |
| `ip` | char[16] | 16 bytes | `"000.000.000.000"` | `validateIP` (valid IPv4) | Server IP, WiFi IP, API endpoint |
| `port` | char[6] | 6 bytes | `"0"` | `validatePort` (1-65535) | Server port, sensor port |

### Adding New Parameters

To add a new parameter (e.g., `sensor_interval`):

#### Step 1: Update `Parameters.h`

```cpp
#pragma once
#include <cstddef>

struct Parameters {
    char id[16];
    char ip[16];
    char port[6];
    char sensor_interval[4];  // NEW: 4 bytes for "1-999" seconds
    
    void resetDefaults();
    static const Parameters& defaults();
};

bool validateID(const char*);
bool validateIP(const char*);
bool validatePort(const char*);
bool validateSensorInterval(const char*);  // NEW validator
bool validateAny(const char*);

struct ParamDescriptor 
{
    const char* name;
    void* valuePtr;
    size_t maxSize;
    bool (*validator)(const char*);
    const char* defaultValue;
};
```

#### Step 2: Implement Validator in `Parameters.cpp`

```cpp
bool validateSensorInterval(const char* value)
{
    if (value == NULL) return false;
    int interval = atoi(value);
    return interval >= 1 && interval <= 999;  // 1-999 seconds
}
```

#### Step 3: Update Default Values in `Parameters.cpp`

```cpp
const Parameters defaultParameters = 
{
    "void",
    "000.000.000.000",
    "0",
    "60",  // NEW: default to 60 seconds
};
```

#### Step 4: Update Array Size in `UartConfigurator.h`

The `paramTable` uses a **sentinel pattern** — the array needs one extra slot for the sentinel terminator:

```cpp
// In UartConfigurator.h
private:
    ParamDescriptor paramTable[5];  // Was 4 (3 original + 1 new + 1 sentinel)
```

#### Step 5: Register Parameter in `UartConfigurator.cpp` Constructor

Add the new parameter **before the sentinel**, and move the sentinel down:

```cpp
UartConfigurator::UartConfigurator() : parameters(), eeprom(), saved(false)
{
    paramTable[0] = {"id", parameters.id, sizeof(parameters.id), validateID, "void"};
    paramTable[1] = {"ip", parameters.ip, sizeof(parameters.ip), validateIP, "000.000.000.000"};
    paramTable[2] = {"port", parameters.port, sizeof(parameters.port), validatePort, "0"};
    paramTable[3] = {"sensor_interval", parameters.sensor_interval, 
                     sizeof(parameters.sensor_interval), validateSensorInterval, "60"};  // NEW
    paramTable[4] = {nullptr, nullptr, 0, nullptr, nullptr};  // Sentinel (moved down)
    
    // ... rest of constructor
}
```

**Note:** The sentinel (`{nullptr, ...}`) is a **stop marker** for all loops. It automatically detects when to stop iterating, so no manual loop count is needed!

#### Step 6: Rebuild and Test

```bash
pio run -t upload
pio device monitor
```

Test the new parameter:
```
> set sensor_interval 120
sensor_interval impostato a 120

> get sensor_interval
sensor_interval:  120

> set sensor_interval 1000
Attenzione! Valore non valido per 'sensor_interval'!

> get -a
id:  void
ip:  000.000.000.000
port:  0
sensor_interval:  120
```

✅ **All loops automatically pick up the new parameter!** No hardcoded counts to update.

### Creating Custom Validators

Validators are simple boolean functions. Here are patterns for common types:

#### String Length Validator
```cpp
bool validateUsername(const char* value)
{
    return value != NULL && strlen(value) >= 4 && strlen(value) <= 32;
}
```

#### Numeric Range Validator
```cpp
bool validateTemperatureThreshold(const char* value)
{
    if (value == NULL) return false;
    int temp = atoi(value);
    return temp >= -40 && temp <= 125;  // Typical sensor range
}
```

#### Port Range Validator (Already Implemented)
```cpp
bool validatePort(const char* value)
{
    int p = atoi(value);
    return p > 0 && p <= 65535;
}
```

#### MAC Address Validator (Example)
```cpp
bool validateMAC(const char* value)
{
    if (!value || strlen(value) != 17) return false;
    
    // Format: "AA:BB:CC:DD:EE:FF"
    for (int i = 0; i < 17; i++) {
        if (i % 3 == 2) {
            if (value[i] != ':') return false;
        } else {
            if (!isxdigit(value[i])) return false;
        }
    }
    return true;
}
```

#### Enum/Choice Validator (Example)
```cpp
bool validateLogLevel(const char* value)
{
    if (!value) return false;
    return strcmp(value, "DEBUG") == 0 || 
           strcmp(value, "INFO") == 0 || 
           strcmp(value, "ERROR") == 0;
}
```

---

## 💡 Examples

### Example 1: IoT Sensor Node Configuration

**Scenario**: You're deploying 10 temperature sensors to different locations.

```bash
> set id TEMP_SENSOR_KITCHEN
id impostato a TEMP_SENSOR_KITCHEN

> set ip 192.168.1.50
ip impostato a 192.168.1.50

> set port 8888
port impostato a 8888

> get -a
id:  TEMP_SENSOR_KITCHEN
ip:  192.168.1.50
port:  8888

> save
Configurazione salvata con successo!

> exit
```

Then repeat for each sensor with different IDs.

### Example 2: Recovery from Wrong Configuration

**Scenario**: You accidentally set the wrong IP address.

```bash
> get ip
ip:  192.168.1.999

> set ip 192.168.1.100
ip impostato a 192.168.1.100

> get ip
ip:  192.168.1.100

> save
Configurazione salvata con successo!
```

### Example 3: Reset to Defaults

**Scenario**: You want to start fresh configuration.

```bash
> clear
ATTENZIONE! Stai per cancellare la configurazione salvata. Proseguire?[y/n]
y

> get -a
id:  void
ip:  000.000.000.000
port:  0
```

### Example 4: Batch Configuration with Timeout

**Scenario**: Configure multiple devices quickly without pressing keys.

In your code, you could set a timeout:
```cpp
// Auto-answer 'yes' to first prompt after 10 seconds (10000ms)
if (this->configurator.makeChoice(10000)) {
    handleInput();
}
```

If the user presses nothing for 10 seconds, it will timeout and use defaults.

---

## 🔐 Data Integrity

### CRC32 Validation

The `EEPROMManager` uses **CRC32 checksums** to detect corruption:

- **When saving**: Calculates CRC32 of parameters and stores it alongside
- **When loading**: Verifies CRC32 matches; if not, loads defaults
- **Prevents**: Data corruption from power loss, noise, or flash wear

**Implementation** (in `EEPROMManager.cpp`):
```cpp
uint32_t EEPROMManager::calcCRC(const Parameters& params) const
{
    // CRC32 calculation...
}
```

### Safe Parameter Updates

- Parameters are **validated before storage** using custom validators
- **No partial writes**: Either entire config is saved or nothing
- **Unsaved changes tracking**: System knows if config differs from EEPROM
- **Confirmation prompts**: Critical operations (save, clear, exit) require user confirmation

### Power Loss Safety

If power is lost during:
- **Parameter modification**: Only in-memory; EEPROM unchanged
- **Saving**: CRC mismatch on next boot triggers default reload
- **Configuration mode**: System returns to saved state on reboot

---

## 🎓 Advanced Features

### Interactive Choice with Timeout

The `makeChoice()` function supports optional countdown:

```cpp
// User has 10 seconds to press 'y' or 'n'
// Shows countdown every second
bool result = configurator.makeChoice(10000);
```

Output:
```
Modificare parametri di configurazione?[y/n]
Tempo rimanente: 9 secondi
Tempo rimanente: 8 secondi
...
Timeout scaduto. Operazione annullata.
```

### Configuration Completeness Check

The `isConfigComplete()` function ensures all parameters are set to non-default values before:
- Saving configuration
- Exiting configuration mode

This prevents accidentally saving incomplete configs with default values.

### State Tracking

`UartConfigurator` tracks:
- `saved`: Whether current config matches EEPROM
- On parameter changes: Automatically marks as unsaved
- On save: Updates flag to true
- On exit: Warns if unsaved changes exist

---

## 🐛 Troubleshooting

### Issue: Serial Connection Not Detected

**Problem**: Can't open serial port

**Solution**:
1. Check USB cable (try different cable)
2. Install CH340 driver (if using cheap Chinese boards)
3. Verify board definition in `platformio.ini`
4. List available ports:
   ```bash
   pio device list
   ```

### Issue: Garbage Characters in Terminal

**Problem**: Output shows `ÿÿÿÿÿÿÿÿÿÿ` or random characters

**Solution**:
- Check serial baud rate is **115200** (see `SerialCLI::begin()`)
- In PlatformIO monitor:
  ```bash
  pio device monitor --baud 115200
  ```
- Try different USB port or cable

### Issue: CRC Mismatch on Boot

**Problem**: "ERRORE: CRC non valido" or won't load saved config

**Causes**:
- EEPROM corrupted (rare)
- Wrong configuration format
- NVS flash needs erasing

**Solution**:
```bash
# Erase all flash
pio run -t erase

# Rebuild and upload
pio run -t upload
```

### Issue: Can't Save Configuration

**Problem**: "Salvataggio fallito!" error

**Causes**:
- EEPROM full (unlikely)
- NVS initialization failed
- Not all parameters set

**Solution**:
1. Ensure all parameters are set: `get -a`
2. Check for validation errors: `set <param> <value>`
3. Try `clear` then `set`/`save` again
4. If persistent, erase flash and reflash

### Issue: Parameter Won't Validate

**Problem**: Getting "Valore non valido" for what looks like valid input

**Solution**:
- Check exact validation rules (see CLI reference or code)
- IP address: Must be valid IPv4 (0-255 per octet)
- Port: Must be 1-65535
- ID: Must be at least 3 characters

Examples:
```
> set port 0         # WRONG: ports must be >= 1
> set port 65536     # WRONG: ports must be <= 65535
> set port 8080      # CORRECT

> set id AB          # WRONG: id must be >= 3 chars
> set id ABC         # CORRECT

> set ip 256.1.1.1   # WRONG: 256 > 255
> set ip 192.168.1.1 # CORRECT
```

---

## 🤝 Contributing

Contributions are welcome! Areas for enhancement:

1. **Additional validators**: ASCII, hostname, URL format, etc.
2. **More parameter types**: int, float, boolean storage
3. **Unit tests**: Test validators and state management
4. **Documentation**: ARCHITECTURE.md, EXAMPLES.md
5. **Features**: Parameter import/export, firmware OTA updates
6. **Optimization**: Code size reduction, memory efficiency
7. **Multi-language support**: Change Italian prompts to English or configurable

**How to contribute**:
1. Fork the repository
2. Create a feature branch: `git checkout -b feature/my-feature`
3. Commit changes: `git commit -am 'Add my feature'`
4. Push to branch: `git push origin feature/my-feature`
5. Submit a pull request

---

## 📝 License

This project is licensed under the **MIT License**. See the `LICENSE` file for details.

MIT License © 2026 Gory-git

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

---

## 📚 Additional Resources

- [Arduino Framework Documentation](https://docs.arduino.cc/)
- [ESP32 Arduino Core](https://github.com/espressif/arduino-esp32)
- [PlatformIO Documentation](https://docs.platformio.org/)
- [EEPROM Concepts](https://en.wikipedia.org/wiki/EEPROM)
- [CRC32 Algorithm](https://en.wikipedia.org/wiki/Cyclic_redundancy_check)

---

## 📧 Support

For issues, questions, or suggestions:
1. Check the [Troubleshooting](#troubleshooting) section
2. Search existing GitHub issues
3. Open a new issue with:
   - Clear problem description
   - Steps to reproduce
   - Serial output or error messages
   - Your hardware/software setup

---

**Made with ❤️ by [Gory-git](https://github.com/Gory-git)**
