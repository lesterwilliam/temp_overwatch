// global libraries
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Hash.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Wire.h>
#include <Adafruit_MLX90614.h>
#include <RGBLED.h>

// local libraries
#include "input.h"
#include "output.h"
#include "data.h"
#include "webpage.h"

// sensor pins
#define SENSOR_FRONT 1 // subject to change
#define SENSOR_REAR 2 // subject to change

// RGB pins
#define PIN_PWM1_R D1
#define PIN_PWM1_G D2
#define PIN_PWM1_B D3
#define PIN_PWM2_R D5
#define PIN_PWM2_G D6
#define PIN_PWM2_B D7

// LED number
#define LED1 1
#define LED2 2

// system defines
#define LOOP_CYCLE_TIME 1 // delay between loop cycles in ms
#define SENSOR_READ_CYCLE_TIME 1000
#define RGB_SET_CYCLE_TIME 100

const char *ssid = "ESP8266-Access-Point";
const char *password = "123456789";

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// RGB-Led objects
RGBLED rgbLed1(PIN_PWM1_R,PIN_PWM1_G,PIN_PWM1_B,COMMON_ANODE);
RGBLED rgbLed2(PIN_PWM2_R,PIN_PWM2_G,PIN_PWM2_B,COMMON_ANODE);

unsigned char *rgb1r, *rgb1g, *rgb1b, *rgb2r, *rgb2g, *rgb2b;
unsigned char rgb1[3];
unsigned char rgb2[3];
int *temp1, *temp2;

// Updates sensor readings every 10 seconds
unsigned long previousSensor = 0;
unsigned long previousRGB = 0;
const long intervalSensors = 1000;
const long intervalRGB = 1000;

// Replaces placeholder with DHT values
String processor(const String &var) {
  //Serial.println(var);
  if (var == "TEMPF") {
    return String(t);
  } else if (var == "TEMPR") {
    return String(h);
  }
  return String();
}

void setup() {
  // Serial port for debugging purposes
  Serial.begin(115200);
  // init SPIFFS
  if(!SPIFFS.begin()){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  initWeb();

  // initialize outputs
  // initialize I2C communication
  // initialize web-server
  // initialize sensors
  initSensors();
}

void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousSensor >= intervalSensors) {
    previousSensor = currentMillis;
    readSensor(LED1);
    readSensor(LED2);
  }
  if (currentMillis - previousRGB >= intervalRGB) {
    previousRGB = currentMillis;
    tempToRGB(*temp1, *temp2);
    outputLED(1);
    outputLED(2);
  }

}
