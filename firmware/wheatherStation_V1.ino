#include <SPI.h>
#include <Wire.h>
#include <WiFi.h>
#include <WebServer.h>

#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <SparkFun_APDS9960.h>

// =====================================================
// WIFI HOTSPOT SETTINGS
// =====================================================
const char* ssid = "weather_station";
const char* password = "weather123456";

WebServer server(80);

// =====================================================
// ESP32 I2C PINS
// =====================================================
#define I2C_SDA 21
#define I2C_SCL 22

// =====================================================
// TFT ILI9341 PINS - YOUR CONNECTION
// =====================================================
#define TFT_SCK   18
#define TFT_MOSI  23
#define TFT_MISO  19
#define TFT_CS    5
#define TFT_DC    2
#define TFT_RST   4

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);

// =====================================================
// SENSORS
// =====================================================
Adafruit_BME280 bme;
SparkFun_APDS9960 apds;

bool bmeFound = false;
bool apdsFound = false;

// =====================================================
// SENSOR VALUES
// =====================================================
float temperature = NAN;
float humidity = NAN;
float pressure = NAN;

uint16_t redValue = 0;
uint16_t greenValue = 0;
uint16_t blueValue = 0;
uint16_t clearValue = 0;

// =====================================================
// TIMING
// =====================================================
unsigned long lastUpdate = 0;
const unsigned long updateInterval = 2000;

// =====================================================
// FORMAT VALUE
// =====================================================
String formatValue(float value, int decimals, String unit) {
  if (isnan(value)) return "N/A";
  return String(value, decimals) + " " + unit;
}

// =====================================================
// READ BME280
// =====================================================
void readBME280Sensor() {
  if (bmeFound) {
    temperature = bme.readTemperature();
    humidity = bme.readHumidity();
    pressure = bme.readPressure() / 100.0F;
  } else {
    temperature = NAN;
    humidity = NAN;
    pressure = NAN;
  }
}

// =====================================================
// READ APDS9960
// =====================================================
void readAPDS9960Sensor() {
  if (apdsFound) {
    apds.readAmbientLight(clearValue);
    apds.readRedLight(redValue);
    apds.readGreenLight(greenValue);
    apds.readBlueLight(blueValue);
  } else {
    clearValue = 0;
    redValue = 0;
    greenValue = 0;
    blueValue = 0;
  }
}

// =====================================================
// DRAW TFT SCREEN DESIGN
// =====================================================
void drawScreenLayout() {
  tft.fillScreen(ILI9341_BLACK);

  tft.fillRect(0, 0, 240, 38, ILI9341_DARKGREY);
  tft.setTextColor(ILI9341_CYAN);
  tft.setTextSize(2);
  tft.setCursor(22, 10);
  tft.print("Weather Station");

  tft.drawFastHLine(0, 42, 240, ILI9341_WHITE);

  tft.setTextSize(2);
  tft.setTextColor(ILI9341_YELLOW);

  tft.setCursor(10, 58);
  tft.print("Temp:");

  tft.setCursor(10, 93);
  tft.print("Humidity:");

  tft.setCursor(10, 128);
  tft.print("Pressure:");

  tft.setCursor(10, 163);
  tft.print("Clear:");

  tft.setCursor(10, 198);
  tft.print("Red:");

  tft.setCursor(10, 233);
  tft.print("Green:");

  tft.setCursor(10, 268);
  tft.print("Blue:");
}

// =====================================================
// UPDATE TFT VALUES ONLY
// =====================================================
void updateScreenValues() {
  tft.fillRect(115, 55, 120, 245, ILI9341_BLACK);

  tft.setTextSize(2);
  tft.setTextColor(ILI9341_WHITE);

  tft.setCursor(115, 58);
  tft.print(formatValue(temperature, 1, "C"));

  tft.setCursor(115, 93);
  tft.print(formatValue(humidity, 1, "%"));

  tft.setCursor(115, 128);
  tft.print(formatValue(pressure, 1, "hPa"));

  tft.setCursor(115, 163);
  tft.print(clearValue);

  tft.setCursor(115, 198);
  tft.print(redValue);

  tft.setCursor(115, 233);
  tft.print(greenValue);

  tft.setCursor(115, 268);
  tft.print(blueValue);
}

// =====================================================
// SERIAL MONITOR
// =====================================================
void printToSerial() {
  Serial.println("================================");
  Serial.println("BME280 + APDS9960 READINGS");
  Serial.println("================================");

  Serial.print("Temperature: ");
  Serial.println(formatValue(temperature, 1, "C"));

  Serial.print("Humidity: ");
  Serial.println(formatValue(humidity, 1, "%"));

  Serial.print("Pressure: ");
  Serial.println(formatValue(pressure, 1, "hPa"));

  Serial.print("Clear: ");
  Serial.println(clearValue);

  Serial.print("RGB: R=");
  Serial.print(redValue);
  Serial.print(" G=");
  Serial.print(greenValue);
  Serial.print(" B=");
  Serial.println(blueValue);

  Serial.println("================================");
}

// =====================================================
// WEB DASHBOARD PAGE
// =====================================================
void handleRoot() {
  String html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>ESP32 Weather Station</title>

  <style>
    * {
      box-sizing: border-box;
      margin: 0;
      padding: 0;
      font-family: Arial, Helvetica, sans-serif;
    }

    body {
      min-height: 100vh;
      background: linear-gradient(135deg, #0f172a, #1e293b, #064e3b);
      color: #ffffff;
      padding: 20px;
    }

    .container {
      max-width: 900px;
      margin: auto;
    }

    .header {
      text-align: center;
      margin-bottom: 25px;
      padding: 25px 15px;
      background: rgba(255, 255, 255, 0.08);
      border-radius: 20px;
      border: 1px solid rgba(255, 255, 255, 0.15);
      box-shadow: 0 20px 40px rgba(0, 0, 0, 0.25);
    }

    .header h1 {
      font-size: 32px;
      margin-bottom: 8px;
      color: #67e8f9;
    }

    .header p {
      color: #cbd5e1;
      font-size: 15px;
    }

    .status {
      display: inline-block;
      margin-top: 15px;
      padding: 8px 15px;
      border-radius: 30px;
      background: rgba(34, 197, 94, 0.18);
      color: #86efac;
      font-size: 14px;
      border: 1px solid rgba(34, 197, 94, 0.4);
    }

    .grid {
      display: grid;
      grid-template-columns: repeat(auto-fit, minmax(190px, 1fr));
      gap: 18px;
    }

    .card {
      background: rgba(255, 255, 255, 0.10);
      border-radius: 18px;
      padding: 22px;
      border: 1px solid rgba(255, 255, 255, 0.14);
      box-shadow: 0 15px 30px rgba(0, 0, 0, 0.20);
    }

    .card h2 {
      font-size: 16px;
      font-weight: normal;
      color: #cbd5e1;
      margin-bottom: 12px;
    }

    .value {
      font-size: 30px;
      font-weight: bold;
      color: #ffffff;
    }

    .unit {
      font-size: 15px;
      color: #94a3b8;
      margin-left: 4px;
    }

    .temperature {
      border-left: 5px solid #f97316;
    }

    .humidity {
      border-left: 5px solid #38bdf8;
    }

    .pressure {
      border-left: 5px solid #a78bfa;
    }

    .clear {
      border-left: 5px solid #facc15;
    }

    .red {
      border-left: 5px solid #ef4444;
    }

    .green {
      border-left: 5px solid #22c55e;
    }

    .blue {
      border-left: 5px solid #3b82f6;
    }

    .rgb-box {
      margin-top: 25px;
      background: rgba(255, 255, 255, 0.08);
      padding: 20px;
      border-radius: 18px;
      border: 1px solid rgba(255, 255, 255, 0.15);
    }

    .rgb-title {
      font-size: 20px;
      margin-bottom: 15px;
      color: #67e8f9;
    }

    .bar-container {
      margin-bottom: 14px;
    }

    .bar-label {
      display: flex;
      justify-content: space-between;
      margin-bottom: 6px;
      color: #cbd5e1;
      font-size: 14px;
    }

    .bar {
      height: 12px;
      background: rgba(255, 255, 255, 0.12);
      border-radius: 20px;
      overflow: hidden;
    }

    .bar-fill {
      height: 100%;
      width: 0%;
      border-radius: 20px;
      transition: width 0.4s ease;
    }

    .red-fill {
      background: #ef4444;
    }

    .green-fill {
      background: #22c55e;
    }

    .blue-fill {
      background: #3b82f6;
    }

    .footer {
      text-align: center;
      margin-top: 25px;
      color: #94a3b8;
      font-size: 13px;
    }

    @media (max-width: 500px) {
      .header h1 {
        font-size: 25px;
      }

      .value {
        font-size: 26px;
      }

      body {
        padding: 12px;
      }
    }
  </style>
</head>

<body>
  <div class="container">

    <div class="header">
      <h1>ESP32 Weather Station</h1>
      <p>BME280 Environmental Sensor + APDS-9960 RGB Light Sensor</p>
      <div class="status">Live Dashboard - Updates Every 2 Seconds</div>
    </div>

    <div class="grid">
      <div class="card temperature">
        <h2>Temperature</h2>
        <div><span class="value" id="temperature">--</span><span class="unit">°C</span></div>
      </div>

      <div class="card humidity">
        <h2>Humidity</h2>
        <div><span class="value" id="humidity">--</span><span class="unit">%</span></div>
      </div>

      <div class="card pressure">
        <h2>Pressure</h2>
        <div><span class="value" id="pressure">--</span><span class="unit">hPa</span></div>
      </div>

      <div class="card clear">
        <h2>Clear Light</h2>
        <div><span class="value" id="clear">--</span></div>
      </div>

      <div class="card red">
        <h2>Red Light</h2>
        <div><span class="value" id="red">--</span></div>
      </div>

      <div class="card green">
        <h2>Green Light</h2>
        <div><span class="value" id="green">--</span></div>
      </div>

      <div class="card blue">
        <h2>Blue Light</h2>
        <div><span class="value" id="blue">--</span></div>
      </div>
    </div>

    <div class="rgb-box">
      <div class="rgb-title">RGB Light Strength</div>

      <div class="bar-container">
        <div class="bar-label"><span>Red</span><span id="redText">--</span></div>
        <div class="bar"><div class="bar-fill red-fill" id="redBar"></div></div>
      </div>

      <div class="bar-container">
        <div class="bar-label"><span>Green</span><span id="greenText">--</span></div>
        <div class="bar"><div class="bar-fill green-fill" id="greenBar"></div></div>
      </div>

      <div class="bar-container">
        <div class="bar-label"><span>Blue</span><span id="blueText">--</span></div>
        <div class="bar"><div class="bar-fill blue-fill" id="blueBar"></div></div>
      </div>
    </div>

    <div class="footer">
      Connect to ESP32 hotspot and open 192.168.4.1
    </div>

  </div>

  <script>
    function limitBar(value) {
      let percent = Number(value);
      if (percent > 100) percent = 100;
      if (percent < 0) percent = 0;
      return percent;
    }

    function updateDashboard() {
      fetch('/data')
        .then(response => response.json())
        .then(data => {
          document.getElementById('temperature').innerText = data.temperature;
          document.getElementById('humidity').innerText = data.humidity;
          document.getElementById('pressure').innerText = data.pressure;
          document.getElementById('clear').innerText = data.clear;
          document.getElementById('red').innerText = data.red;
          document.getElementById('green').innerText = data.green;
          document.getElementById('blue').innerText = data.blue;

          document.getElementById('redText').innerText = data.red;
          document.getElementById('greenText').innerText = data.green;
          document.getElementById('blueText').innerText = data.blue;

          document.getElementById('redBar').style.width = limitBar(data.red) + '%';
          document.getElementById('greenBar').style.width = limitBar(data.green) + '%';
          document.getElementById('blueBar').style.width = limitBar(data.blue) + '%';
        })
        .catch(error => {
          console.log('Dashboard update error:', error);
        });
    }

    updateDashboard();
    setInterval(updateDashboard, 2000);
  </script>

</body>
</html>
)rawliteral";

  server.send(200, "text/html", html);
}

// =====================================================
// JSON DATA FOR WEB DASHBOARD
// =====================================================
void handleData() {
  String json = "{";

  json += "\"temperature\":\"";
  json += isnan(temperature) ? "N/A" : String(temperature, 1);
  json += "\",";

  json += "\"humidity\":\"";
  json += isnan(humidity) ? "N/A" : String(humidity, 1);
  json += "\",";

  json += "\"pressure\":\"";
  json += isnan(pressure) ? "N/A" : String(pressure, 1);
  json += "\",";

  json += "\"clear\":\"" + String(clearValue) + "\",";
  json += "\"red\":\"" + String(redValue) + "\",";
  json += "\"green\":\"" + String(greenValue) + "\",";
  json += "\"blue\":\"" + String(blueValue) + "\"";

  json += "}";

  server.send(200, "application/json", json);
}

// =====================================================
// SETUP
// =====================================================
void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("Starting ESP32 Weather Station...");

  Wire.begin(I2C_SDA, I2C_SCL);

  SPI.begin(TFT_SCK, TFT_MISO, TFT_MOSI, TFT_CS);

  tft.begin();

  // Portrait mode. If upside down, change 0 to 2.
  tft.setRotation(0);

  tft.fillScreen(ILI9341_BLACK);
  tft.setTextColor(ILI9341_GREEN);
  tft.setTextSize(2);
  tft.setCursor(25, 140);
  tft.print("Starting...");
  delay(1000);

  drawScreenLayout();

  // =====================================================
  // BME280 SETUP
  // =====================================================
  if (bme.begin(0x76)) {
    bmeFound = true;
    Serial.println("BME280 found at 0x76");
  } else if (bme.begin(0x77)) {
    bmeFound = true;
    Serial.println("BME280 found at 0x77");
  } else {
    Serial.println("BME280 NOT found");
  }

  // =====================================================
  // APDS9960 SETUP
  // =====================================================
  if (apds.init()) {
    apdsFound = true;
    apds.enableLightSensor(false);
    Serial.println("APDS9960 found");
  } else {
    Serial.println("APDS9960 NOT found");
  }

  // =====================================================
  // START ESP32 HOTSPOT
  // =====================================================
  WiFi.softAP(ssid, password);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("ESP32 Hotspot Started. IP Address: ");
  Serial.println(IP);

  // =====================================================
  // WEB ROUTES
  // =====================================================
  server.on("/", handleRoot);
  server.on("/data", handleData);
  server.begin();

  Serial.println("Web server started.");

  readBME280Sensor();
  readAPDS9960Sensor();
  updateScreenValues();
  printToSerial();
}

// =====================================================
// LOOP
// =====================================================
void loop() {
  server.handleClient();

  if (millis() - lastUpdate >= updateInterval) {
    lastUpdate = millis();

    readBME280Sensor();
    readAPDS9960Sensor();

    updateScreenValues();
    printToSerial();
  }
}
