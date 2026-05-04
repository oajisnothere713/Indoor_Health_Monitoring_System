#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME680.h>
#include <BH1750.h>
#include <math.h>

// ================= WIFI (NORMAL WIFI) =================
const char* ssid = "creating";
const char* password = "987654321";

// ================= BACKEND =================
// ⚠ Change this IP to YOUR laptop hotspot IP using ipconfig
const char* serverURL = "http://172.20.10.2:8001/data";  

// ================= DEVICE =================
const char* device_id = "monacos_room_01";

// ================= PINS =================
#define BME_CS 5
#define MQ135_PIN 34
#define SOUND_ANALOG_PIN 35

// ================= MQ135 CONFIG =================
#define RL_VALUE 10
#define RO_CLEAN_AIR_FACTOR 3.6
float RO = 10.0;

// ================= OBJECTS =================
Adafruit_BME680 bme(BME_CS);
BH1750 lightMeter;

// ================= WIFI CONNECT =================
void connectWiFi() {

  Serial.println("Connecting to WIFI LEN...");

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\n✅ Connected!");
  Serial.print("ESP32 IP Address: ");
  Serial.println(WiFi.localIP());
}

// ================= SETUP =================
void setup() {
  Serial.begin(115200);
  delay(1000);

  connectWiFi();

  SPI.begin(18, 19, 23, BME_CS);
  Wire.begin(21, 22);

  if (!bme.begin()) {
    Serial.println("BME680 not found!");
    while (1);
  }

  bme.setTemperatureOversampling(BME680_OS_8X);
  bme.setHumidityOversampling(BME680_OS_2X);
  bme.setPressureOversampling(BME680_OS_4X);
  bme.setGasHeater(320, 150);

  lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE);

  pinMode(SOUND_ANALOG_PIN, INPUT);

  calibrateMQ135();
}

// ================= LOOP =================
void loop() {

  if (WiFi.status() != WL_CONNECTED) {
    connectWiFi();
  }

  if (!bme.performReading()) {
    delay(2000);
    return;
  }

  float temperature = bme.temperature;
  float humidity    = bme.humidity;
  float pressure    = bme.pressure / 100.0;
  float altitude    = bme.readAltitude(1013.25);
  float light       = lightMeter.readLightLevel();
  float noise       = readSoundLevel();

  int raw = analogRead(MQ135_PIN);
  float voltage = raw * (3.3 / 4095.0);
  float RS = ((3.3 * RL_VALUE) / voltage) - RL_VALUE;
  float ratio = RS / RO;

  float co2  = getCO2_PPM(ratio);
  float vocs = getVOC_PPM(ratio);

  float pm25 = 12.0;  // no PM sensor
  float pm10 = 35.0;

  float aqi = pm25 * 3.5;
  float air_quality_score = calculateAirQuality(bme.gas_resistance);

  StaticJsonDocument<512> doc;

  doc["device_id"]         = device_id;
  doc["temperature"]       = temperature;
  doc["humidity"]          = humidity;
  doc["pm25"]              = pm25;
  doc["pm10"]              = pm10;
  doc["noise"]             = noise;
  doc["light"]             = light;
  doc["altitude"]          = altitude;
  doc["pressure"]          = pressure;
  doc["co2"]               = co2;
  doc["vocs"]              = vocs;
  doc["aqi"]               = aqi;
  doc["air_quality_score"] = air_quality_score;

  String payload;
  serializeJson(doc, payload);

  Serial.println("Sending JSON:");
  Serial.println(payload);

  HTTPClient http;
  http.begin(serverURL);
  http.addHeader("Content-Type", "application/json");

  int httpResponseCode = http.POST(payload);

  Serial.print("HTTP Response: ");
  Serial.println(httpResponseCode);

  if (httpResponseCode > 0) {
    Serial.println(http.getString());
  }

  http.end();

  delay(3000);
}

// ================= SOUND =================
float readSoundLevel() {

  int signalMax = 0;
  int signalMin = 4095;
  unsigned long startMillis = millis();

  while (millis() - startMillis < 50) {
    int sample = analogRead(SOUND_ANALOG_PIN);
    if (sample > signalMax) signalMax = sample;
    if (sample < signalMin) signalMin = sample;
  }

  int peakToPeak = signalMax - signalMin;
  float voltage = (peakToPeak * 3.3) / 4095.0;

  if (peakToPeak > 10) {
    float dB = 20.0 * log10(voltage / 0.00631) + 50;
    return constrain(dB, 30, 120);
  } else {
    return 30;
  }
}

// ================= MQ135 =================
void calibrateMQ135() {
  float sum = 0;
  for(int i = 0; i < 50; i++) {
    int raw = analogRead(MQ135_PIN);
    float voltage = raw * (3.3 / 4095.0);
    float RS = ((3.3 * RL_VALUE) / voltage) - RL_VALUE;
    sum += RS;
    delay(50);
  }
  RO = (sum / 50.0) / RO_CLEAN_AIR_FACTOR;
}

float calculateAirQuality(float gas_resistance) {
  gas_resistance = gas_resistance / 1000.0;
  if(gas_resistance > 50) return 100;
  else if(gas_resistance > 30) return 80;
  else if(gas_resistance > 20) return 60;
  else if(gas_resistance > 10) return 40;
  else if(gas_resistance > 5) return 20;
  else return 10;
}

float getCO2_PPM(float ratio) {
  return 116.6020682 * pow(ratio, -2.769034857);
}

float getVOC_PPM(float ratio) {
  return 77.255 * pow(ratio, -3.18);
}