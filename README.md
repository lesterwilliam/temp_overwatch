# temp_overwatch
This is a project I made for school.

A Lolin ESP8266 NodeMCU v3 reads a pair of MLX90614 IR-temperature sensors and controlls a corresponding pair of RGB LED. Via two #defines the temperature minimum and maximum can be set. If the measured temperature is equal or bellow the lower threshold, the LED is blue, if it's equal or higher to the upper threshold it's red. Everything inbetween is a linear mix of red and blue, it basically fades with the temperature.
Additionally the ESP provides a Webserver you can connect to via WiFi. It displays both temperatures and lets you download the logged temperatures as a .csv-file.
The HTML-code and the .csv-log are both saved in the SPIFFS space.

If you are interested in using my sources or have any questions, I'd be glad to help you.
Contact me via adrian.schwizgebel@gmail.com