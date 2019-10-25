void initSensors() {
  // init sensors on pins SENSOR_FRONT, SENSOR_REAR
  Adafruit_MLX90614 mlx1 = Adafruit_MLX90614();
  Adafruit_MLX90614 mlx2 = Adafruit_MLX90614();
  mlx.begin();
}

String readSensor(char sensor) {
  if(sensor == 1) float newT = mlx1.readObjectTempC();
  if(sensor == 2) float newT = mlx2.readObjectTempC();
  if (isnan(newT)) {
      Serial.println("Failed to read from BME280 sensor!");
      return "";
    }
    else {
      Serial.println(newT);
      return String(newT);
    }
}
