# EFM32_Zero_Gecko_Project
This is an individual project developed using the **EFM32 Zero Gecko** kit , consisting of multiple embedded applications accessible through a menu interface, using push buttons and sensors.

The system displays a **menu interface** on a screen that allows individual selection of the following programs:

- **P1**: Modular LED Control
- **P2**: LED Brightness Control via Capacitive Buttons
- **P3**: Environmental Data Display (Temperature, Humidity, UV)
- **P4**: Proximity Sensor Visualization

### Controls:
- **PB1**: Select
- **PB0**: Confirm

  
## P1: Modular LED Control

**Features:**
- Press **PB0**: Toggle **LED0** ON/OFF  
- Press **PB1**: Toggle **LED1** ON/OFF  
- Long press **PB0**: Enable/disable blinking of **LED0** (1s interval)  
- Double press **PB1**: Enable/disable blinking of **LED1** (2s interval)  
- Implement **software debounce** for both buttons (300ms)
- Timer-based polling (10ms)
- **PB0 + PB1** pressed simultaneously: Return to main menu


## P2: LED Brightness Control (PWM via Capacitive Buttons)

**Features:**
- Configure a PWM-enabled output pin using a TIMER
- Control brightness using capacitive buttons **CP0/CP1**:
  - **0%**, **25%**, **50%**, **75%**, **100%**
- Display brightness value as number or graphical bar
- **PB0 + PB1** pressed simultaneously: Return to main menu

  
## P3: Environmental Sensors Display (Temperature / Humidity / UV)

**Features:**
- Display:
  - Ambient temperature
  - Humidity
  - UV radiation level
- Allow user to configure temperature and humidity thresholds
- Implement **hysteresis algorithm**:
  - If ambient temperature < (user temp - hysteresis) → Turn ON **LED0**  
  - If ambient temperature > (user temp + hysteresis) → Turn ON **LED1**  
  - Hysteresis range: **[0°C – 2.5°C]**, step: **0.5°C**
- **PB0 + PB1** pressed simultaneously: Return to main menu

  
## P4: Proximity Sensor Data Display (SI1147 + IR Sensors DS1–DS3)

**Features:**
- Show a **dot** based on hand position
- Draw a **circle** with radius proportional to the distance from the user’s hand to the sensor
- **PB0 + PB1** pressed simultaneously: Return to main menu
