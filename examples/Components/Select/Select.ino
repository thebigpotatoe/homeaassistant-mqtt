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

// Select example

// WiFi Data
#define WIFI_SSID "your_wifi_ssid"
#define WIFI_PASS "your_password"

// MQTT Data
#define HA_MQTT_HOST_NAME "your_broker_hostname_or_ip"
#define HA_MQTT_PORT 1883
#define HA_MQTT_RESET_ALL_COMPONENTS true

// Includes
#include "Homeassistant_MQTT.h"

// Select object
Select select("test select");

// Callbacks
void on_option_1(void* _data) {
  select.state.update_state("Option 1");
  Serial.println("Option 1 Selected");
}
void on_option_2(void* _data) {
  select.state.update_state("Option 2");
  Serial.println("Option 2 Selected");
}
void on_option_3(void* _data) {
  select.state.update_state("Option 3");
  Serial.println("Option 3 Selected");
}

void setup() {
  // Serial for debug
  // Serial.begin(115200);
  // Serial.println();

  // Add options to the select with their callbacks
  select.add_option("Option 1", &on_option_1);
  select.add_option("Option 2", &on_option_2);
  select.add_option("Option 3", &on_option_3);

  // WiFi setup
  WiFi.begin(WIFI_SSID, WIFI_PASS);
}

void loop() {}