# Feather Flight Data Recorder (FDR)  

A lightweight, low-cost **Flight Data Recorder** (FDR) designed for **Cubesats, Cansats, and DIY aerospace projects**. Based on the **ATmega328P**, it logs critical flight parameters and sensor data to both onboard microSD card and via LoRa transmission.

### Key Features  
- **Multi-Sensor Support**:  
  - **BMP280** – Barometric pressure & temperature  
  - **LSM6DS3TR** – 3D accelerometer & gyroscope (IMU)  
  - **INA226AIDGSR** – Current/voltage/power monitoring  
  - **E22-433MM22S** – LoRa module (433 MHz)  
- **Dual Data Storage**:
  - Real-time transmission via LoRa
  - Reliable microSD card backup
- **Built-In Test Equipment (B.I.T.E.)**:  
  - Self-diagnostic firmware for pre-flight checks  
  - POST codes via **buzzer** and **RGB LED**  
  - Error logging with fail-safe modes  
- **Arduino-Compatible**:  
  - Programmable via **Arduino IDE** (like an **Arduino UNO**)  
  - Uses **CH340E** for USB-to-serial  
- **Power Management**:  
  - **FM4057** (Li-ion/LiPo charger) + **MT36291** (voltage regulator)  

---

## B.I.T.E. Firmware  
**Built-in Test Equipment (B.I.T.E.)** is the test/validation firmware for the FDR. It verifies hardware functionality and logs errors before flight.  

### POST Codes (Power-On Self-Test)  
The board emits **audible beeps** (buzzer) and **RGB LED flashes** to indicate status:  

| Code | Buzzer Pattern | LED Color  | Meaning               |  
|------|----------------|------------|-----------------------|  
| `0`  | 1 short        | Green      | All systems OK!       |  

**TODO**

---

## Installation & Flashing  
### Required Software  
1. **Arduino IDE** ([Download](https://www.arduino.cc/en/software))  
2. **Board Support**:  
   - Install **ATmega328P** support via *Tools > Board > Boards Manager* (`Arduino AVR Boards`)  

### Flashing Steps  
1. Connect via USB (**CH340E** driver required on Windows)  
2. Select board: *Tools > Board > Arduino Uno* (ATmega328P, 16MHz)  
3. Upload `feather_bite.ino` (main firmware file)  

---

## Data Storage and Transmission
The FDR implements redundant data storage:
1. **Primary storage**: microSD card (FAT32 format)
   - Stores complete flight data with timestamps
   - Survives power interruptions
2. **Real-time transmission**: LoRa (433MHz)
   - Sends condensed dataset for ground monitoring
   - Configurable transmission interval

### Data Format  
Both storage methods use consistent CSV format:  
```  
TODO
```  

---

## Power & Safety  
- **Input Voltage**: 3.7V–5V (Li-ion/LiPo via **FM4057** or USB)  
- **Fail-Safe Features**:  
  - Automatic fallback to microSD-only logging if LoRa fails  
  - Write caching to prevent SD card corruption  
  - Low-voltage cutoff (configurable via **INA226**)  
  - Emergency power preservation mode  

---

## Credits & License  
**Developers**: Nate Hunter, Crypto Schizo  
**License**: MIT  
- Free for personal and commercial use with attribution  
- Includes custom libraries for all supported sensors