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

// Switch example

// WiFi Data
#define WIFI_SSID "your_wifi_ssid"
#define WIFI_PASS "your_password"

// MQTT Data
#define HA_MQTT_HOST_NAME "your_broker_hostname_or_ip"
#define HA_MQTT_PORT 1883
#define HA_MQTT_RESET_ALL_COMPONENTS true

// Includes
#include "Homeassistant_MQTT.h"

// Switch object
Switch test_switch("test switch");

// Callbacks
void on_switch_on(void* _data) {
  ON();
}
void on_switch_off(void* _data) {
  OFF();
}

// Switch functions
void setup_switch() {
  pinMode(5, OUTPUT);
}
void ON() {
  digitalWrite(5, HIGH);
  test_switch.state.update_state(SWITCH_ON);
}
void OFF() {
  digitalWrite(5, LOW);
  test_switch.state.update_state(SWITCH_OFF);
}

void setup() {
  // Serial for debug
  // Serial.begin(115200);
  // Serial.println();

  // Actual output setup
  setup_switch();

  // Setup callbacks
  test_switch.on_switch_on = &on_switch_on;
  test_switch.on_switch_off = &on_switch_off;

  // WiFi setup
  WiFi.begin(WIFI_SSID, WIFI_PASS);
}

void loop() {}