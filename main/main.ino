// Libraries
#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>
#include <Adafruit_MLX90614.h>
#include <RGBLED.h>
#include <FS.h>

// RGB pins
#define PIN_PWM1_R D4
#define PIN_PWM1_G D3
#define PIN_PWM1_B D8
#define PIN_PWM2_R D6
#define PIN_PWM2_G D5
#define PIN_PWM2_B D7

// Define lower and upper threshold for RGB output in [°C]
#define TEMP_MIN 25 // full blue at this temperature
#define TEMP_MAX 50 // full red at this temperature

// RGB-Led objects and components
RGBLED rgbLed1(PIN_PWM1_R,PIN_PWM1_G,PIN_PWM1_B,COMMON_CATHODE);
RGBLED rgbLed2(PIN_PWM2_R,PIN_PWM2_G,PIN_PWM2_B,COMMON_CATHODE);
unsigned char rgb1r, rgb1b, rgb2r, rgb2b;

// MLX sensor objects
Adafruit_MLX90614 mlx_front = Adafruit_MLX90614();
Adafruit_MLX90614 mlx_rear =  Adafruit_MLX90614();

// SPIFFS file system object
File file;

// WiFi credentials
const char* ssid     = "Temp_overwatch";
const char* password = "temP_Overwatch7";

// Current temperature, updated in loop()
int temp_front = 0;
int temp_rear  = 0;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// Run time variable
unsigned long currentMillis = 0;

// Store last timestapts when services got executed
unsigned long mlx_previousMillis = 0;
unsigned long rgb_previousMillis = 0;
unsigned long spiffs_previousMillis = 0;

// Update interval for services in [ms]
const long mlx_interval = 100;
const long rgb_interval = 200;
const long spiffs_interval = 5000;

// Replaces placeholder in HTML with MLX values
String processor(const String& var){
  if(var == "TEMPERATURE_FRONT"){
    return String(temp_front);
  }
  else if(var == "TEMPERATURE_REAR"){
    return String(temp_rear);
  }
  else if(var == "RUN_TIME"){
    return String((int)(currentMillis / 1000));
  }
  return String();
}

// Initialize File System, start & format
void InitSPIFFS() {
  SPIFFS.begin();
  if (!(SPIFFS.begin())) // Try to format SPIFFS
  {
    SPIFFS.format();
  }
  if (!(SPIFFS.begin())) // Try again if failed
  {
    SPIFFS.format();
  }
}

// Converts temperatures to RGB components
void TempToRGB() {
  // Front temperature to RGB components
  rgb1r = (unsigned int) ((temp_front - TEMP_MIN) * 0xFF / (TEMP_MAX - TEMP_MIN));
  rgb1b = 0xFF - rgb1r;

  // Rear temperature to RGB components
  rgb2r = (unsigned int) ((temp_rear - TEMP_MIN) * 0xFF / (TEMP_MAX - TEMP_MIN));
  rgb2b = 0xFF - rgb2r;
}

void setup(){
  // Initialize SPIFFS
  InitSPIFFS();
  if(SPIFFS.exists("/temp_log.csv")){
    SPIFFS.remove("/temp_log.csv");
  }
  file = SPIFFS.open("/temp_log.csv", "w");
  file.println("Time [s],Front temp [°C],Rear temp [°C]");
  file.close();

  // Start MLX service
  mlx_front.begin();
  mlx_rear.begin();

  // Initialize WiFi service
  WiFi.softAP(ssid, password);
  IPAddress IP = WiFi.softAPIP();

  // Define HTTP services
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });
  server.on("/run_time", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", String().c_str());
  });
  server.on("/temperature_front", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", String(temp_front).c_str());
  });
  server.on("/temperature_rear", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", String(temp_rear).c_str());
  });
    server.on("/download", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/temp_log.csv", "text/txt");
  });
  
  // Start web server
  server.begin();
}
 
void loop(){
  // MLX service
  currentMillis = millis();
  if (currentMillis - mlx_previousMillis >= mlx_interval) {
    mlx_previousMillis = currentMillis;
    temp_front = (int)mlx_front.readObjectTempC();
    temp_rear = (int)mlx_rear.readObjectTempC();

    // this is a fix to write the smaller temp to both positions because only one sensor works
    int smaller_temp;
    smaller_temp = temp_front;
    if(temp_rear < smaller_temp) smaller_temp = temp_rear;
      temp_front = smaller_temp;
      temp_rear = smaller_temp;
  }

  // RGB service
  if (currentMillis - rgb_previousMillis >= rgb_interval) {
    rgb_previousMillis = currentMillis;
    TempToRGB();
    rgbLed1.writeRGB(0x00, rgb1r, rgb1b);
    rgbLed2.writeRGB(0x00, rgb2r, rgb2b);
  }

  // SPIFFS service
  if (currentMillis - spiffs_previousMillis >= spiffs_interval) {
    spiffs_previousMillis = currentMillis;
    file = SPIFFS.open("/temp_log.csv", "a");
    file.print(String(currentMillis / 1000));
    file.print(",");
    file.print(String(temp_front));
    file.print(",");
    file.println(String(temp_rear));
    file.close();
  }
}
