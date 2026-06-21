# ESP32 Weather Station-version-1

This project is an ESP32-based weather station that reads environmental data using a **BME280 sensor** and an **APDS-9960 RGB light sensor**. The readings are displayed on a **2.4-inch ILI9341 TFT screen** and can also be viewed on a web dashboard through the ESP32 WiFi hotspot.

## Features

* Reads temperature, humidity, and pressure from the BME280
* Reads clear light, red, green, and blue light values from the APDS-9960
* Displays readings on an ILI9341 TFT screen
* Creates an ESP32 WiFi hotspot
* Hosts a local web dashboard
* Shows live readings in a browser at `192.168.4.1`

## Components Used

| Component           | Description                                |
| ------------------- | ------------------------------------------ |
| ESP32               | Main microcontroller                       |
| BME280              | Temperature, humidity, and pressure sensor |
| APDS-9960           | RGB and ambient light sensor               |
| ILI9341 TFT Display | 2.4-inch screen for local display          |
| Breadboard          | For prototyping                            |
| Jumper Wires        | For connections                            |
| USB Cable           | For programming and power                  |

## Pin Connections

### BME280 to ESP32

| BME280 Pin | ESP32 Pin |
| ---------- | --------- |
| VCC        | 3.3V      |
| GND        | GND       |
| SDA        | GPIO21    |
| SCL        | GPIO22    |

### APDS-9960 to ESP32

| APDS-9960 Pin | ESP32 Pin     |
| ------------- | ------------- |
| VCC           | 3.3V          |
| GND           | GND           |
| SDA           | GPIO21        |
| SCL           | GPIO22        |
| INT           | Not Connected |

### ILI9341 TFT Display to ESP32

| TFT Pin     | ESP32 Pin |
| ----------- | --------- |
| VCC         | 3.3V      |
| GND         | GND       |
| SCK / CLK   | GPIO18    |
| MOSI / SDI  | GPIO23    |
| MISO / SDO  | GPIO19    |
| CS          | GPIO5     |
| DC / A0     | GPIO2     |
| RST / RESET | GPIO4     |
| LED / BL    | 3.3V      |

## Code Pin Definitions

```cpp
#define I2C_SDA 21
#define I2C_SCL 22

#define TFT_SCK   18
#define TFT_MOSI  23
#define TFT_MISO  19
#define TFT_CS    5
#define TFT_DC    2
#define TFT_RST   4
```

## Required Libraries

Install these libraries in the Arduino IDE:

* Adafruit GFX Library
* Adafruit ILI9341
* Adafruit Unified Sensor
* Adafruit BME280 Library
* SparkFun APDS9960 RGB and Gesture Sensor Library
* WiFi
* WebServer

The `WiFi` and `WebServer` libraries usually come with the ESP32 board package.

## Setup

1. Connect the BME280, APDS-9960, and TFT display to the ESP32 using the pin tables above.
2. Open the Arduino IDE.
3. Install the required libraries.
4. Select your ESP32 board from `Tools > Board`.
5. Select the correct COM port from `Tools > Port`.
6. Upload the code to the ESP32.
7. Open the Serial Monitor and set the baud rate to `115200`.

## Web Dashboard

After uploading the code, the ESP32 creates a WiFi hotspot.

| WiFi Setting | Value           |
| ------------ | --------------- |
| WiFi Name    | weather_station |
| Password     | weather123456   |
| Web Address  | 192.168.4.1     |

Connect your phone or computer to the ESP32 hotspot, then open a browser and go to:

```text
192.168.4.1
```

The web page will show the live sensor readings.

## Sensor Readings Displayed

| Reading     | Sensor    |
| ----------- | --------- |
| Temperature | BME280    |
| Humidity    | BME280    |
| Pressure    | BME280    |
| Clear Light | APDS-9960 |
| Red Light   | APDS-9960 |
| Green Light | APDS-9960 |
| Blue Light  | APDS-9960 |


