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

#define TEMP_MIN 50
#define TEMP_MAX 300

#define RED 0
#define GREEN 1
#define BLUE 2

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

// MLX objects
Adafruit_MLX90614 mlx1 = Adafruit_MLX90614();
Adafruit_MLX90614 mlx2 = Adafruit_MLX90614();
  
unsigned char *rgb1r, *rgb1g, *rgb1b, *rgb2r, *rgb2g, *rgb2b;
unsigned char rgb1[3];
unsigned char rgb2[3];
int *temp1;
int *temp2;

// Updates sensor readings every 10 seconds
unsigned long previousSensor = 0;
unsigned long previousRGB = 0;
const long intervalSensors = 1000;
const long intervalRGB = 1000;

// Replaces placeholder with temp values
String processor(const String& var) {
  if(var == "TEMPF") {
    return String(*temp1);
  }
  else if(var == "TEMPR") {
    return String(*temp2);
  }
  return String();
}

void initWeb() {
  Serial.print("Setting AP (Access Point)�");

  WiFi.softAP(ssid, password);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  // Print ESP8266 Local IP Address
  Serial.println(WiFi.localIP());

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(SPIFFS, "/index.html");
      });

  // from example:
  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send_P(200, "text/plain", readSensor(1).c_str());
      });
  // Start web server
  server.begin();
}

// splits RGB into single colors and outputs
int outputLED(unsigned char ledNr) {
  if (ledNr == 1) {
    analogWrite(PIN_PWM1_R, *rgb1r);
    analogWrite(PIN_PWM1_G, *rgb1g);
    analogWrite(PIN_PWM1_B, *rgb1b);
    return 1;
  } else if (ledNr == 2) {
    analogWrite(PIN_PWM2_R, *rgb2r);
    analogWrite(PIN_PWM2_G, *rgb2g);
    analogWrite(PIN_PWM2_B, *rgb2b);
    return 1;
  } else {
    return 0;
  }
  rgbLed1.writeRGB(*rgb1r,*rgb1g,*rgb1b);
  rgbLed2.writeRGB(*rgb2r,*rgb2g,*rgb2b);
}

void tempToRGB(int *temp1, int *temp2) {
  // calculate rgb components
  if (1 == 1) {
    if (*temp1 < TEMP_MIN) {
      *rgb1r = 0x00;
      *rgb1b = 0xFF;
    } else if (*temp1 >= TEMP_MIN && *temp1 <= TEMP_MAX) {
      *rgb1r = (unsigned int) ((*temp1 - TEMP_MIN) * 0xFF
          / (TEMP_MAX - TEMP_MIN));
      *rgb1b = 0xFF
          - (unsigned int) ((*temp1 - TEMP_MIN) * 0xFF / (TEMP_MAX - TEMP_MIN));
    } else if (*temp1 > TEMP_MAX) {
      *rgb1r = 0xFF;
      *rgb1b = 0x00;
    } else {
    }
  } else if (2 == 2) {
    if (*temp2 < TEMP_MIN) {
      *rgb2r = 0x00;
      *rgb2b = 0xFF;
    } else if (*temp2 >= TEMP_MIN && *temp2 <= TEMP_MAX) {
      *rgb2r = (unsigned int) ((*temp2 - TEMP_MIN) * 0xFF
          / (TEMP_MAX - TEMP_MIN));
      *rgb2b = 0xFF
          - (unsigned int) ((*temp2 - TEMP_MIN) * 0xFF / (TEMP_MAX - TEMP_MIN));
    } else if (*temp2 > TEMP_MAX) {
      *rgb2r = 0xFF;
      *rgb2b = 0x00;
    } else {
    }
  }
}
String readSensor(char sensor) {
  float newT;
  if(1 == 1){
    newT = mlx1.readObjectTempC();
  }
  if(1 == 2){
    newT = mlx2.readObjectTempC();
  }
  if (isnan(newT)) {
      Serial.println("Failed to read from BME280 sensor!");
      return "";
    }
    else {
      Serial.println(newT);
      return String(newT);
    }
}

void setup() {
  // Serial port for debugging purposes
  Serial.begin(115200);
  // init SPIFFS
  if(!SPIFFS.begin()){
    Serial.println("An Error has occurred while mounting SPIFFS");
  }

  initWeb();

  // initialize outputs
  // initialize I2C communication
  // initialize web-server
  // initialize sensors
  mlx1.begin();
  mlx2.begin();
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
