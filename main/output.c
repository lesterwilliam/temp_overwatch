#define TEMP_MIN 50
#define TEMP_MAX 300

#define RED 0
#define GREEN 1
#define BLUE 2

// splits RGB into single colors and outputs
bool outputLED(unsigned char ledNr) {
  /*if (ledNr == 1) {
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
  }*/
  rgbLed1.writeRGB(*rgb1r,*rgb1g,*rgb1b);
  rgbLed2.writeRGB(*rgb2r,*rgb2g,*rgb2b);
}

bool tempToRGB(int *temp1, int *temp2) {
  // calculate rgb components
  if (sensorNr == 1) {
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
      return 0;
    }
    return 1;
  } else if (sensorNr == 2) {
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
      return 0;
    }
    return 1;
  }
}
