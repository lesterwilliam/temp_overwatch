#include "main.ino"

bool initSensors() {
  // init sensors on pins SENSOR_FRONT, SENSOR_REAR
  Adafruit_MLX90614 mlx1 = Adafruit_MLX90614();
  Adafruit_MLX90614 mlx2 = Adafruit_MLX90614();
  mlx.begin();
  return 1;
}

String readSensor(char sensor) {
  if(sensor == LED1) float newT = mlx1.readObjectTempC();
  if(sensor == LED2) float newT = mlx2.readObjectTempC();
  if (isnan(newT)) {
      Serial.println("Failed to read from BME280 sensor!");
      return "";
    }
    else {
      Serial.println(newT);
      return String(newT);
    }
}
