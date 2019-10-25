bool initWeb() {
  Serial.print("Setting AP (Access Point)…");

  WiFi.softAP(ssid, password);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  // Print ESP8266 Local IP Address
  Serial.println(WiFi.localIP());

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(SPIFFS, "/index2.html");
      });

  /*server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
   request->send_P(200, "text/html", index_html, processor);
   });*/

  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send_P(200, "text/plain", String(*temp1).c_str());
      });

  // from example:
  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send_P(200, "text/plain", readBME280Temperature().c_str());
      });
  // Start web server
  server.begin();
}

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
