/*
Copyright (c) 2021 thebigpotatoe

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
*/

// Tag Scanner example

// WiFi Data
#define WIFI_SSID "your_wifi_ssid"
#define WIFI_PASS "your_password"

// MQTT Data
#define HA_MQTT_HOST_NAME "your_broker_hostname_or_ip"
#define HA_MQTT_PORT 1883
#define HA_MQTT_RESET_ALL_COMPONENTS true

// Includes
#include "Homeassistant_MQTT.h"

// Tag Scanner object
TagScanner scanner("test scanner");

// Sketch Data
unsigned long loop_timer = 0;

void setup() {
  // Serial for debug
  // Serial.begin(115200);
  // Serial.println();

  // WiFi setup
  WiFi.begin(WIFI_SSID, WIFI_PASS);
}

void loop() {
  if (millis() - loop_timer > 10000) {
    char tag_name[11];
    sprintf(tag_name, "test_%li", random(0, 1000));
    Serial.printf("Sending tag %s\n", tag_name);
    scanner.send_tag(tag_name);
    loop_timer = millis();
  }
}