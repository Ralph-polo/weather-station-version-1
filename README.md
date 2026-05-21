# Weather Station

An ESP32-based environmental monitoring system that measures and displays real-time environmental conditions using a **BME/BMP280 sensor**, a **GY-2561 TSL2561 luminosity sensor**, and a **2.4-inch ILI9341 TFT display**.

The system reads environmental data, displays the values locally on a TFT screen, and also hosts a simple web dashboard through the ESP32 WiFi hotspot. This allows the readings to be viewed from a phone, tablet, or computer without needing an external internet connection.

---

## Project Overview

This project was designed to demonstrate how an ESP32 can be used to collect, process, and display environmental sensor data.

The system measures:

- Temperature
- Humidity, if a BME280 module is used
- Barometric pressure
- Light intensity in lux

The ESP32 communicates with the sensors, updates the TFT display, and serves a local web page where the same readings can be viewed remotely.

This project is suitable for learning:

- ESP32 sensor interfacing
- I2C communication
- SPI display control
- Web server hosting on ESP32
- Environmental monitoring system design

---

## Main Features

- Reads temperature from the BME/BMP280 sensor
- Reads humidity when using a BME280 module
- Reads atmospheric pressure in hPa
- Reads light intensity in lux using the GY-2561 TSL2561 sensor
- Displays readings on a 2.4-inch TFT LCD
- Creates an ESP32 WiFi hotspot
- Hosts a local web dashboard
- Automatically refreshes web readings
- Prints sensor values to the Serial Monitor
- Uses a simple and expandable Arduino sketch

---

## Hardware Components

| Component | Description |
|---|---|
| ESP32 WiFi + Bluetooth Board | Main microcontroller for reading sensors, displaying data, and hosting the web dashboard |
| BME/BMP280 Sensor | Measures temperature and pressure; BME280 also supports humidity |
| GY-2561 TSL2561 Sensor | Digital light sensor for measuring light intensity in lux |
| 2.4-inch ILI9341 TFT Display | Displays sensor readings locally |
| Breadboard | Used for prototyping the circuit |
| Jumper Wires | Used for connecting the components |
| USB Cable | Used for programming and powering the ESP32 |

---

## Important Note About BME280 and BMP280

The project can work with either a **BME280** or **BMP280** style module, but they do not measure exactly the same parameters.

| Sensor | Temperature | Humidity | Pressure |
|---|---:|---:|---:|
| BME280 | Yes | Yes | Yes |
| BMP280 | Yes | No | Yes |

If you are using a **BME280**, the system can display temperature, humidity, and pressure.

If you are using a **BMP280**, humidity will not be available because the BMP280 does not measure humidity.

---

## Parameters Measured

| Parameter | Sensor Used | Unit | Meaning |
|---|---|---|---|
| Temperature | BME/BMP280 | C | Measures how hot or cold the surrounding air is |
| Humidity | BME280 only | % | Measures the amount of water vapour in the air |
| Pressure | BME/BMP280 | hPa | Measures atmospheric pressure |
| Light Intensity | TSL2561 | lux | Measures the amount of visible light falling on the sensor |

---

## Repository Structure

```text
esp32-environmental-monitoring-system/
│
├── README.md
├── LICENSE
├── .gitignore
│
├── firmware/
│   └── env_monitor_bme280_tsl2561/
│       └── env_monitor_bme280_tsl2561.ino
│
├── hardware/
│   ├── wiring-diagram.png
│   ├── pinout-table.md
│   └── circuit-description.md
│
├── images/
│   ├── assembled-system.jpg
│   ├── tft-display-output.jpg
│   └── web-dashboard-output.jpg
│
└── sample-output/
    └── serial-monitor-output.txt
```

---

## Folder Description

### `firmware/`

Contains the main Arduino sketch for the ESP32.

The sketch handles:

- Sensor initialization
- Sensor reading
- TFT display output
- WiFi hotspot setup
- Web dashboard generation
- Serial Monitor output

---

### `hardware/`

Contains hardware-related files such as:

- Wiring diagram
- Pin connection table
- Circuit explanation

---

### `images/`

Contains images of the project, such as:

- Assembled circuit
- TFT display output
- Web dashboard screenshot
- Sensor modules

---

### `sample-output/`

Contains example output from the Serial Monitor or sample readings from the system.

---

## Circuit Connections

### ESP32 to GY-2561 TSL2561 Light Sensor

The TSL2561 uses I2C communication.

| TSL2561 Pin | ESP32 Pin |
|---|---|
| VCC | 3.3V |
| GND | GND |
| SDA | GPIO21 |
| SCL | GPIO22 |
| ADDR | Not connected |
| INT | Not connected |

The ADDR pin can be left unconnected for the default I2C address.

---

### ESP32 to BME/BMP280 Sensor

The BME/BMP280 also uses I2C communication.

| BME/BMP280 Pin | ESP32 Pin |
|---|---|
| VCC | 3.3V |
| GND | GND |
| SDA | GPIO21 |
| SCL | GPIO22 |

The TSL2561 and BME/BMP280 can share the same SDA and SCL pins because they use different I2C addresses.

---

### ESP32 to ILI9341 TFT Display

The TFT display uses SPI communication.

| TFT Pin | ESP32 Pin |
|---|---|
| VCC | 3.3V |
| GND | GND |
| CS | GPIO15 |
| DC | GPIO2 |
| RST | GPIO5 |
| SCK / CLK | GPIO18 |
| MOSI | GPIO23 |
| MISO | GPIO19 |
| LED / BL | 3.3V |

Some TFT modules may have slightly different pin labels, so always confirm the labels printed on your own display module.

---

## Communication Interfaces

### I2C

The BME/BMP280 and TSL2561 sensors communicate with the ESP32 using I2C.

In this project:

```text
SDA = GPIO21
SCL = GPIO22
```

I2C allows multiple devices to share the same two communication lines.

---

### SPI

The TFT display communicates with the ESP32 using SPI.

SPI is used for the display because it is faster and better suited for sending screen data.

---

## Required Arduino Libraries

Install these libraries in the Arduino IDE before uploading the code:

- Adafruit GFX Library
- Adafruit ILI9341
- Adafruit Unified Sensor
- Adafruit BME280 Library
- Adafruit TSL2561
- WiFi
- WebServer

The `WiFi` and `WebServer` libraries usually come with the ESP32 board package.

---

## How to Use the Project

### 1. Connect the Hardware

Connect the ESP32, sensors, and TFT display according to the wiring table above.

Make sure:

- All GND pins are connected together
- Sensors are powered correctly
- SDA and SCL connections are correct
- TFT SPI pins match the code

---

### 2. Open the Arduino Sketch

Open the main sketch from:

```text
firmware/env_monitor_bme280_tsl2561/env_monitor_bme280_tsl2561.ino
```

---

### 3. Select the ESP32 Board

In Arduino IDE, go to:

```text
Tools → Board
```

Select your ESP32 board. A common option is:

```text
ESP32 Dev Module
```

---

### 4. Select the COM Port

Connect the ESP32 to your computer and select the correct port:

```text
Tools → Port
```

---

### 5. Upload the Code

Click the upload button in the Arduino IDE.

After uploading, open the Serial Monitor and set the baud rate to:

```text
115200
```

---

## WiFi Web Dashboard

After the code is uploaded, the ESP32 creates its own WiFi hotspot.

Default hotspot details:

```text
SSID: weather_station
Password: weather123456
```

Connect your phone, tablet, or computer to this WiFi network.

Then open a browser and visit:

```text
192.168.4.1
```

The web dashboard will display the live environmental readings.

No internet connection is required because the ESP32 hosts the web page locally.

---

## Expected TFT Display Output

The TFT display should show readings similar to:

```text
ENV MONITOR

Temp:      30.9 C
Humidity:  65.0 %
Pressure: 1008.5 hPa
Lux:       540.0 lx
```

If a BMP280 is used instead of a BME280, humidity will not be available.

---

## Expected Serial Monitor Output

The Serial Monitor should show readings similar to:

```text
Temperature: 30.90 C
Humidity:    65.00 %
Pressure:    1008.50 hPa
Lux:         540.00 lux
--------------------
```

If a sensor is not detected, the Serial Monitor will show a warning message.

---

## Example Web Dashboard Readings

The web dashboard displays:

- Temperature
- Humidity, if supported by the sensor
- Pressure
- Light intensity
- Sensor status

The page refreshes automatically to show updated sensor values.

---



## Troubleshooting

### TSL2561 Not Detected

Check the following:

- VCC is connected to 3.3V
- GND is connected properly
- SDA is connected to GPIO21
- SCL is connected to GPIO22
- The Adafruit TSL2561 library is installed
- The Adafruit Unified Sensor library is installed

---

### BME/BMP280 Not Detected

Check the following:

- VCC is connected to 3.3V
- GND is connected properly
- SDA is connected to GPIO21
- SCL is connected to GPIO22
- Try I2C address `0x76`
- Try I2C address `0x77`
- The Adafruit BME280 library is installed

---

### Humidity Not Showing

Check the sensor type.

If the module is a BMP280, humidity will not show because BMP280 does not support humidity.

To display humidity, use a BME280 module.

---

### TFT Display Is Blank

Check the following:

- TFT power connection
- TFT ground connection
- SPI pin connections
- CS, DC, and RST pin definitions in the code
- Adafruit GFX library installation
- Adafruit ILI9341 library installation

---

### Web Dashboard Not Opening

Check the following:

- ESP32 is powered on
- The ESP32 hotspot is visible
- You connected to the correct WiFi network
- The password is correct
- You opened `192.168.4.1` in the browser

---

### Upload Error

If the code does not upload:

- Check that the correct ESP32 board is selected
- Check that the correct COM port is selected
- Use a proper USB data cable
- Close the Serial Monitor before uploading
- Hold the ESP32 BOOT button during upload if required

---

## Safety Notes

- Use 3.3V logic with the ESP32
- Do not connect 5V signal outputs directly to ESP32 GPIO pins
- Make sure all modules share a common ground
- Double-check wiring before powering the circuit
- Avoid loose jumper wire connections during testing



## License

This project can be released under an open-source license such as the MIT License.

---

## Author

Developed as an ESP32 environmental monitoring project.
