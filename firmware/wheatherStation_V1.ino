#include <SPI.h>
#include <Wire.h>
#include <WiFi.h>
#include <WebServer.h>

#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <Adafruit_TSL2561_U.h>

// =====================================================
// WIFI HOTSPOT SETTINGS
// =====================================================
const char* ssid = "weather_station";
const char* password = "weather123456";

WebServer server(80);

// =====================================================
// ESP32 I2C PINS
// BME280 and TSL2561 share these pins
// =====================================================
#define I2C_SDA 21
#define I2C_SCL 22

// =====================================================
// TFT ILI9341 PINS
// =====================================================
#define TFT_CS   15
#define TFT_DC   2
#define TFT_RST  5

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);

// =====================================================
// BME280 SENSOR
// Common I2C addresses: 0x76 or 0x77
// =====================================================
Adafruit_BME280 bme;
bool bmeFound = false;

// =====================================================
// GY-2561 TSL2561 LUMINOSITY SENSOR
// ADDR not connected = default floating address 0x39
// =====================================================
Adafruit_TSL2561_Unified tsl = Adafruit_TSL2561_Unified(TSL2561_ADDR_FLOAT, 12345);
bool tslFound = false;

// =====================================================
// SENSOR VALUES
// =====================================================
float temperature = NAN;
float humidity    = NAN;
float pressure    = NAN;
float luxValue    = NAN;

unsigned long lastUpdate = 0;
const unsigned long updateInterval = 2000;

// =====================================================
// FORMAT SENSOR VALUE
// =====================================================
String formatValue(float value, int decimals, String unit) {
  if (isnan(value)) {
    return "N/A";
  }

  return String(value, decimals) + " " + unit;
}

// =====================================================
// CONFIGURE TSL2561 SENSOR
// =====================================================
void configureTSL2561() {
  // Auto range allows the sensor to switch gain automatically
  tsl.enableAutoRange(true);

  // 402ms gives better resolution
  tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_402MS);

  Serial.println("TSL2561 configured successfully.");
  Serial.println("Gain: Auto");
  Serial.println("Integration Time: 402 ms");
}

// =====================================================
// READ ALL SENSORS
// =====================================================
void readSensors() {
  // ---------- Read BME280 ----------
  if (bmeFound) {
    temperature = bme.readTemperature();
    humidity = bme.readHumidity();
    pressure = bme.readPressure() / 100.0F;  // Convert Pa to hPa
  } 
  else {
    temperature = NAN;
    humidity = NAN;
    pressure = NAN;
  }

  // ---------- Read TSL2561 ----------
  if (tslFound) {
    sensors_event_t event;
    tsl.getEvent(&event);

    luxValue = event.light;
  } 
  else {
    luxValue = NAN;
  }
}

// =====================================================
// SERIAL MONITOR OUTPUT
// =====================================================
void printToSerial() {
  Serial.println();
  Serial.println("======================================");
  Serial.println("ESP32 WEATHER STATION READINGS");
  Serial.println("======================================");

  Serial.print("Temperature:     ");
  Serial.println(formatValue(temperature, 1, "C"));

  Serial.print("Humidity:        ");
  Serial.println(formatValue(humidity, 1, "%"));

  Serial.print("Pressure:        ");
  Serial.println(formatValue(pressure, 1, "hPa"));

  Serial.print("Light Intensity: ");
  Serial.println(formatValue(luxValue, 1, "lux"));

  Serial.println("======================================");
}

// =====================================================
// TFT SCREEN LAYOUT
// =====================================================
void drawTFTLayout() {
  tft.fillScreen(ILI9341_BLACK);
  tft.setRotation(1);

  tft.setTextColor(ILI9341_CYAN);
  tft.setTextSize(2);
  tft.setCursor(55, 8);
  tft.println("Weather Station");

  tft.drawLine(0, 35, 320, 35, ILI9341_WHITE);

  tft.setTextColor(ILI9341_YELLOW);
  tft.setTextSize(2);

  tft.setCursor(10, 55);
  tft.println("Temp:");

  tft.setCursor(10, 85);
  tft.println("Humidity:");

  tft.setCursor(10, 115);
  tft.println("Pressure:");

  tft.setCursor(10, 145);
  tft.println("Light:");

  tft.drawLine(0, 185, 320, 185, ILI9341_WHITE);

  tft.setTextSize(1);
  tft.setTextColor(ILI9341_CYAN);
  tft.setCursor(10, 198);
  
  tft.setCursor(10, 213);
 
}

// =====================================================
// UPDATE TFT VALUES
// =====================================================
void updateTFT() {
  // Clear old values only
  tft.fillRect(145, 50, 170, 125, ILI9341_BLACK);

  tft.setTextSize(2);
  tft.setTextColor(ILI9341_WHITE);

  // ---------- Temperature ----------
  tft.setCursor(145, 55);
  tft.print(formatValue(temperature, 1, "C"));

  // ---------- Humidity ----------
  tft.setCursor(145, 85);
  tft.print(formatValue(humidity, 1, "%"));

  // ---------- Pressure ----------
  tft.setCursor(145, 115);
  tft.print(formatValue(pressure, 1, "hPa"));

  // ---------- Light Intensity ----------
  tft.setCursor(145, 145);
  tft.print(formatValue(luxValue, 1, "lx"));
}

// =====================================================
// WEB PAGE
// =====================================================
void handleRoot() {
  String tempText = formatValue(temperature, 1, "C");
  String humText = formatValue(humidity, 1, "%");
  String pressureText = formatValue(pressure, 1, "hPa");
  String luxText = formatValue(luxValue, 1, "lux");

  String html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <meta http-equiv="refresh" content="2">
  <title>ESP32 Weather Station</title>

  <style>
    * {
      box-sizing: border-box;
    }

    body {
      margin: 0;
      min-height: 100vh;
      font-family: Arial, Helvetica, sans-serif;
      background: radial-gradient(circle at top, #1d4ed8 0%, #111827 45%, #030712 100%);
      color: white;
      padding: 24px;
    }

    .container {
      max-width: 900px;
      margin: auto;
    }

    .header {
      background: rgba(255, 255, 255, 0.08);
      border: 1px solid rgba(255, 255, 255, 0.12);
      border-radius: 24px;
      padding: 24px;
      margin-bottom: 20px;
      box-shadow: 0 20px 50px rgba(0, 0, 0, 0.35);
    }

    .title {
      margin: 0;
      font-size: 32px;
      letter-spacing: 1px;
    }

    .subtitle {
      margin-top: 8px;
      color: #cbd5e1;
      font-size: 15px;
    }

    .grid {
      display: grid;
      grid-template-columns: repeat(auto-fit, minmax(190px, 1fr));
      gap: 16px;
    }

    .card {
      background: rgba(15, 23, 42, 0.82);
      border: 1px solid rgba(148, 163, 184, 0.16);
      border-radius: 22px;
      padding: 22px;
      box-shadow: 0 15px 35px rgba(0, 0, 0, 0.35);
    }

    .label {
      color: #94a3b8;
      font-size: 13px;
      text-transform: uppercase;
      letter-spacing: 1px;
      margin-bottom: 12px;
    }

    .value {
      font-size: 30px;
      font-weight: bold;
      color: #f8fafc;
    }

    .small {
      font-size: 15px;
      color: #cbd5e1;
      margin-top: 8px;
    }

    .footer {
      text-align: center;
      color: #94a3b8;
      font-size: 13px;
      margin-top: 20px;
    }
  </style>
</head>

<body>
  <div class="container">

    <div class="header">
      <h1 class="title">ESP32 Weather Station</h1>
      <div class="subtitle">BME280 Sensor + GY-2561 TSL2561 Luminosity Sensor</div>
    </div>

    <div class="grid">

      <div class="card">
        <div class="label">Temperature</div>
        <div class="value">)rawliteral";

  html += tempText;

  html += R"rawliteral(</div>
        <div class="small">Measured by BME280</div>
      </div>

      <div class="card">
        <div class="label">Humidity</div>
        <div class="value">)rawliteral";

  html += humText;

  html += R"rawliteral(</div>
        <div class="small">Measured by BME280</div>
      </div>

      <div class="card">
        <div class="label">Pressure</div>
        <div class="value">)rawliteral";

  html += pressureText;

  html += R"rawliteral(</div>
        <div class="small">Measured by BME280</div>
      </div>

      <div class="card">
        <div class="label">Light Intensity</div>
        <div class="value">)rawliteral";

  html += luxText;

  html += R"rawliteral(</div>
        <div class="small">Measured by TSL2561</div>
      </div>

    </div>

    <div class="footer">
      Page refreshes automatically every 2 seconds
    </div>

  </div>
</body>
</html>
)rawliteral";

  server.send(200, "text/html", html);
}

// =====================================================
// SETUP
// =====================================================
void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println();
  Serial.println("Starting ESP32 Weather Station...");
  Serial.println("Sensors: BME280 + GY-2561 TSL2561");

  // ---------- Start I2C ----------
  Wire.begin(I2C_SDA, I2C_SCL);

  // ---------- Start BME280 ----------
  if (bme.begin(0x76)) {
    bmeFound = true;
    Serial.println("BME280 found at I2C address 0x76");
  } 
  else if (bme.begin(0x77)) {
    bmeFound = true;
    Serial.println("BME280 found at I2C address 0x77");
  } 
  else {
    bmeFound = false;
    Serial.println("BME280 NOT found. Check wiring.");
  }

  // ---------- Start TSL2561 ----------
  if (tsl.begin()) {
    tslFound = true;
    Serial.println("GY-2561 TSL2561 Luminosity Sensor found.");
    configureTSL2561();
  } 
  else {
    tslFound = false;
    Serial.println("TSL2561 NOT found. Check wiring.");
  }

  // ---------- Start TFT ----------
  tft.begin();
  drawTFTLayout();

  // ---------- Start WiFi Hotspot ----------
  WiFi.softAP(ssid, password);
  IPAddress IP = WiFi.softAPIP();

  Serial.println();
  Serial.println("ESP32 Hotspot Started");
  Serial.print("SSID: ");
  Serial.println(ssid);
  Serial.print("Password: ");
  Serial.println(password);
  Serial.print("Open this IP in your browser: ");
  Serial.println(IP);

  // ---------- Start Web Server ----------
  server.on("/", handleRoot);
  server.begin();

  Serial.println("Web server started.");
  Serial.println("Setup complete.");

  // First sensor reading
  readSensors();
  printToSerial();
  updateTFT();
}

// =====================================================
// LOOP
// =====================================================
void loop() {
  server.handleClient();

  if (millis() - lastUpdate >= updateInterval) {
    lastUpdate = millis();

    readSensors();
    printToSerial();
    updateTFT();
  }
}