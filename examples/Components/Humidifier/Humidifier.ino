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

// Humidifier example

// WiFi Data
#define WIFI_SSID "your_wifi_ssid"
#define WIFI_PASS "your_password"

// MQTT Data
#define HA_MQTT_HOST_NAME "your_broker_hostname_or_ip"
#define HA_MQTT_PORT 1883
#define HA_MQTT_RESET_ALL_COMPONENTS true

// Includes
#include "Homeassistant_MQTT.h"

// Humidifier object
Humidifier humidifier("test humidifier");

// Callbacks
void on_humidifier_on(void* _data) {
  on();
}
void on_humidifier_off(void*) {
  off();
}
void on_target_update(void* _target) {
  set_target(humidifier.target.cast_to_type(_target));
}

// Lock functions
void setup_humidifier() {
  pinMode(5, OUTPUT);
}
void on() {
  humidifier.state = HUMIDIFIER_ON;
  digitalWrite(5, HIGH);
}
void off() {
  humidifier.state = HUMIDIFIER_OFF;
  digitalWrite(5, LOW);
}
void set_target(uint8_t _target){
  // User specific, maybe set a bunch of pid variables or something?
  humidifier.target = _target;
}

void setup() {
  // Serial for debug
  // Serial.begin(115200);
  // Serial.println();

  // Actual output setup
  setup_humidifier();

  // Setup humidifier callbacks
  humidifier.on_humidifier_on = &on_humidifier_on;
  humidifier.on_humidifier_off = &on_humidifier_off;
  humidifier.on_target_update = &on_target_update;

  // WiFi setup
  WiFi.begin(WIFI_SSID, WIFI_PASS);
}

void loop() {}