// global libraries
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Hash.h>
#include <Wire.h>
#include <Adafruit_MLX90614.h>
#include <RGBLED.h>

// local libraries
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

IPAddress apIP(42,42,42,42);

const char *ssid = "ESP8266-Access-Point";
const char *password = "123456789";

// Define a web server at port 80 for HTTP
ESP8266WebServer server(80);

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
const int ledPin = D1; // an LED is connected to NodeMCU pin D1 (ESP8266 GPIO5) via a 1K Ohm resistor

bool ledState = false;
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
void handleRoot() {
  digitalWrite (LED_BUILTIN, 0); //turn the built in LED on pin DO of NodeMCU on
  digitalWrite (ledPin, server.arg("led").toInt());
  ledState = digitalRead(ledPin);

 /* Dynamically generate the LED toggle link, based on its current state (on or off)*/
  char ledText[80];
  
  if (ledState) {
    strcpy(ledText, "LED is on. <a href=\"/?led=0\">Turn it OFF!</a>");
  }

  else {
    strcpy(ledText, "LED is OFF. <a href=\"/?led=1\">Turn it ON!</a>");
  }
 
  ledState = digitalRead(ledPin);

  char html[1000];
  int sec = millis() / 1000;
  int min = sec / 60;
  int hr = min / 60;

  int brightness = analogRead(A0);
  brightness = (int)(brightness + 5) / 10; //converting the 0-1024 value to a (approximately) percentage value

// Build an HTML page to display on the web-server root address
  snprintf ( html, 1000,

"<html>\
  <head>\
    <meta http-equiv='refresh' content='10'/>\
    <title>ESP8266 WiFi Network</title>\
    <style>\
      body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; font-size: 1.5em; Color: #000000; }\
      h1 { Color: #AA0000; }\
    </style>\
  </head>\
  <body>\
    <h1>ESP8266 Wi-Fi Access Point and Web Server Demo</h1>\
    <p>Uptime: %02d:%02d:%02d</p>\
    <p>Brightness: %d%</p>\
    <p>%s<p>\
    <p>This page refreshes every 10 seconds. Click <a href=\"javascript:window.location.reload();\">here</a> to refresh the page now.</p>\
  </body>\
</html>",

    hr, min % 60, sec % 60,
    brightness,
    ledText
  );
  server.send ( 200, "text/html", html );
  digitalWrite ( LED_BUILTIN, 1 );
}

void handleNotFound() {
  digitalWrite ( LED_BUILTIN, 0 );
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += ( server.method() == HTTP_GET ) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for ( uint8_t i = 0; i < server.args(); i++ ) {
    message += " " + server.argName ( i ) + ": " + server.arg ( i ) + "\n";
  }

  server.send ( 404, "text/plain", message );
  digitalWrite ( LED_BUILTIN, 1 ); //turn the built in LED on pin DO of NodeMCU off
}
/*void initWeb() {
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
}*/

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

void tempToRGB() {
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
  // init SPIFFS
  //if(!SPIFFS.begin()){
  //  Serial.println("An Error has occurred while mounting SPIFFS");
  //}

  //initWeb();

  // initialize outputs
  // initialize I2C communication
  // initialize web-server
  // initialize sensors
  //mlx1.begin();
  //mlx2.begin();
  pinMode ( ledPin, OUTPUT );
  digitalWrite ( ledPin, 0 );
  
  delay(1000);
  Serial.begin(115200);
  Serial.println();
  Serial.println("Configuring access point...");

  //set-up the custom IP address
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));   // subnet FF FF FF 00  
  
  /* You can remove the password parameter if you want the AP to be open. */
  WiFi.softAP(ssid, password);

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
 
  server.on ( "/", handleRoot );
  server.on ( "/led=1", handleRoot);
  server.on ( "/led=0", handleRoot);
  server.on ( "/inline", []() {
    server.send ( 200, "text/plain", "this works as well" );
  } );
  server.onNotFound ( handleNotFound );
  
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  /*unsigned long currentMillis = millis();

  if (currentMillis - previousSensor >= intervalSensors) {
    previousSensor = currentMillis;
    readSensor(LED1);
    readSensor(LED2);
  }
  if (currentMillis - previousRGB >= intervalRGB) {
    previousRGB = currentMillis;
    tempToRGB();
    outputLED(1);
    outputLED(2);
  }*/
  server.handleClient();
}
