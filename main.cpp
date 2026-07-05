#define BLYNK_TEMPLATE_ID "TMPL3vAw5Pi69"
#define BLYNK_TEMPLATE_NAME "Smart Irrigation System"
#define BLYNK_AUTH_TOKEN "JDbJe6G1V9PAaHzqPr4cKKuSD9oGZ3C4"
#define BLYNK_PRINT Serial

#include <Arduino.h>
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <DHT.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include "web_dashboard.h"

// Wi-Fi and Blynk credentials
char auth[] = "JDbJe6G1V9PAaHzqPr4cKKuSD9oGZ3C4";
char ssid[] = "HKB";
char pass[] = "22012006";

// HARDWARE DEFINITIONS
#define PIN_RELAY_PUMP      23
#define PIN_SOIL_MOISTURE   34
#define PIN_DHT             26
#define PIN_PIR             13
#define PIN_LDR             35
#define DHTTYPE DHT11

// Virtual pins
#define VPIN_BUTTON_1 V12
#define VPIN_SOIL_MOISTURE V2
#define VPIN_AUTO_MODE V10
#define VPIN_TEMP V0
#define VPIN_HUM V1
#define VPIN_PIR_BUTTON V6
#define VPIN_PIR_LED V5

// RTOS TIMING INTERVALS
#define INTERVAL_MAIN_LOOP      100    
#define INTERVAL_SENSOR_READ    2000   
#define INTERVAL_HEALTH_CHECK   10000  

unsigned long lastSensorRead    = 0;
unsigned long lastHealthCheck   = 0;

// Global variables
DHT dht(PIN_DHT, DHTTYPE);
WebServer server(80);

bool autoMode = false;
bool pumpState = false;
int pirState = LOW;
int warm_up = 0; 
unsigned long lastManualOffTime = 0;
const unsigned long manualOffDelay = 2 * 60 * 1000;

// Sensor State Variables
float currentTemp = 0.0;
float currentHum = 0.0;
float currentSoil = 0.0;
float engineHealth = 1.0;
float engineScore = 0.0;
String sysModeStr = "NORMAL";
String userModeStr = "MANUAL";
String lastAction = "INITIALIZING";
String lastReason = "SYSTEM BOOT";

void updatePumpState(bool state) {
    pumpState = state;
    digitalWrite(PIN_RELAY_PUMP, state ? LOW : HIGH);
    Serial.println(state ? "💧🔥 Pump TURNED ON" : "💧🔥 Pump TURNED OFF");
    
    lastAction = state ? "FULL_IRRIGATION" : "STANDBY";
    lastReason = state ? (autoMode ? "AI_DECISION" : "MANUAL_OVERRIDE") : "PUMP_OFF";
}

BLYNK_WRITE(VPIN_AUTO_MODE) {
    autoMode = param.asInt();
    userModeStr = autoMode ? "SMART AUTO" : "MANUAL";
    if (!autoMode) {
        updatePumpState(false);
    }
}

BLYNK_WRITE(VPIN_BUTTON_1) {
    bool manualRequest = param.asInt();
    if (!autoMode) {
        updatePumpState(manualRequest);
    } else if (!manualRequest) {
        updatePumpState(false);
        lastManualOffTime = millis();
    }
}

BLYNK_WRITE(VPIN_PIR_BUTTON) {
    pirState = param.asInt();
    if (pirState == 1) {
        Blynk.virtualWrite(VPIN_PIR_LED, HIGH);
        warm_up = 1;
    } else {
        Blynk.virtualWrite(VPIN_PIR_LED, LOW);
        warm_up = 0;
    }
}

void checkSoilMoisture() {
    int soilRaw = analogRead(PIN_SOIL_MOISTURE);
    currentSoil = map(soilRaw, 3500, 4095, 100, 0);
    Blynk.virtualWrite(VPIN_SOIL_MOISTURE, currentSoil);

    if (autoMode) {
        if (currentSoil < 12 && !pumpState && millis() - lastManualOffTime > manualOffDelay) {
            updatePumpState(true);
        } else if (currentSoil >= 60 && pumpState) {
            updatePumpState(false);
        }
    }
}

void sendDHTData() {
    float h = dht.readHumidity();
    float t = dht.readTemperature();
    if (!isnan(h) && !isnan(t)) {
        currentTemp = t;
        currentHum = h;
        Blynk.virtualWrite(VPIN_TEMP, t);
        Blynk.virtualWrite(VPIN_HUM, h);
    }
}

void checkPIR() {
    if (pirState == 1) {
        int sensor_output = digitalRead(PIN_PIR);
        if (sensor_output == LOW) {
            if (warm_up == 1) { warm_up = 0; delay(2000); }
            Blynk.virtualWrite(VPIN_PIR_LED, LOW);
        } else {
            Blynk.virtualWrite(VPIN_PIR_LED, HIGH);
            Blynk.logEvent("pirmotion", "WARNING! Motion Detected!");
        }
    }
}

// Web Server Handlers
void handleRoot() {
    server.send(200, "text/html", index_html);
}

void handleData() {
    StaticJsonDocument<512> doc;
    doc["sysMode"] = sysModeStr;
    doc["userMode"] = userModeStr;
    doc["health"] = engineHealth;
    doc["score"] = engineScore;
    doc["soil"] = currentSoil;
    doc["temp"] = currentTemp;
    doc["hum"] = currentHum;
    doc["rain"] = 0; // External API placeholder
    doc["et"] = 0.52; // Evapotranspiration placeholder
    doc["dri"] = 0.15; // Drought Risk Index placeholder
    doc["waterUsed"] = pumpState ? 1.5 : 0.0;
    doc["waterBase"] = 2.0;
    doc["waterSaved"] = 0.5;
    doc["action"] = lastAction;
    doc["reason"] = lastReason;
    
    String response;
    serializeJson(doc, response);
    server.send(200, "application/json", response);
}

void handleSetMode() {
    if (server.hasArg("val")) {
        String val = server.arg("val");
        if (val == "0") { autoMode = false; userModeStr = "MANUAL"; }
        else { autoMode = true; userModeStr = "SMART AUTO"; }
        Blynk.virtualWrite(VPIN_AUTO_MODE, autoMode ? 1 : 0);
        server.send(200, "text/plain", "Mode updated");
    }
}

void handleSetPump() {
    if (server.hasArg("val") && !autoMode) {
        String val = server.arg("val");
        bool pState = (val == "1");
        updatePumpState(pState);
        Blynk.virtualWrite(VPIN_BUTTON_1, pState ? 1 : 0);
        server.send(200, "text/plain", "Pump updated");
    } else {
        server.send(403, "text/plain", "Cannot manually control in Auto mode");
    }
}

void setupWiFi() {
    Serial.println("[SYSTEM] Connecting to WiFi Network...");
    WiFi.begin(ssid, pass);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\n[SYSTEM] WiFi Connected.");
}

void calculateSensorHealth() {
    // Dynamic Health based on sensor connectivity
    engineHealth = (currentSoil == 0 && currentTemp == 0) ? 0.3 : 1.0;
    sysModeStr = (engineHealth < 0.5) ? "SAFE MODE" : "NORMAL";
}

void runDecisionEngine() {
    // Synthetic Score for Dashboard based on real soil moisture
    engineScore = (100.0 - currentSoil) / 100.0;
}

void setup() {
    Serial.begin(115200);
    delay(1000);
    
    pinMode(PIN_RELAY_PUMP, OUTPUT);
    digitalWrite(PIN_RELAY_PUMP, HIGH); // Relay Active-LOW -> HIGH = OFF
    pinMode(PIN_SOIL_MOISTURE, INPUT);
    pinMode(PIN_LDR, INPUT);
    pinMode(PIN_PIR, INPUT);
    
    dht.begin();
    setupWiFi();
    Blynk.config(auth);
    Blynk.connect();
    
    // Setup WebServer
    server.on("/", HTTP_GET, handleRoot);
    server.on("/data", HTTP_GET, handleData);
    server.on("/setMode", HTTP_POST, handleSetMode);
    server.on("/setPump", HTTP_POST, handleSetPump);
    server.begin();
    Serial.println("[SYSTEM] 🟢 WebServer and Systems online.");
}

void loop() {
    unsigned long now = millis();
    Blynk.run();
    server.handleClient();
    
    if (now - lastSensorRead >= INTERVAL_SENSOR_READ) {
        lastSensorRead = now;
        checkSoilMoisture();
        sendDHTData();
        checkPIR();
    }
    
    if (now - lastHealthCheck >= INTERVAL_HEALTH_CHECK) {
        lastHealthCheck = now;
        calculateSensorHealth();
        runDecisionEngine();
    }
    delay(INTERVAL_MAIN_LOOP);
}
