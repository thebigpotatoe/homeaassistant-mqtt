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

// Fan example

// WiFi Data
#define WIFI_SSID "your_wifi_ssid"
#define WIFI_PASS "your_password"

// MQTT Data
#define HA_MQTT_HOST_NAME "your_broker_hostname_or_ip"
#define HA_MQTT_PORT 1883
#define HA_MQTT_RESET_ALL_COMPONENTS true

// Includes
#include "Homeassistant_MQTT.h"

// Fan object
Fan fan("test fan");

// Callbacks
void on_fan_on(void* _data) {
  on();
}
void on_fan_off(void*) {
  off();
}
void on_oscillate_on(void*) {
  oscillate(true);
}
void on_oscillate_off(void*) {
  oscillate(false);
}
void on_percent_chage(void* _percent) {
  percent(fan.percent.cast_to_type(_percent));
}

// Lock functions
void setup_fan() {
  pinMode(5, OUTPUT);
}
void on() {
  fan.state.update_state(FAN_ON);
  digitalWrite(5, HIGH);
}
void off() {
  fan.state.update_state(FAN_OFF);
  digitalWrite(5, LOW);
}
void oscillate(bool on_off) {
  // User specific, maybe set a flag to cycle a pwn signal up and down?
  if (on_off)
    fan.oscillate.update_state(OSCILLATE_ON);
  else
    fan.oscillate.update_state(OSCILLATE_OFF);
}
void percent(uint8_t _percent) {
  // User specific, maybe chnage a pwn signal?
  fan.percent.update_state(_percent);
}

void setup() {
  // Serial for debug
  // Serial.begin(115200);
  // Serial.println();

  // Actual output setup
  setup_fan();

  // Setup fan as required
  fan.state_optimistic = false;

  // Setup fan callbacks
  fan.on_fan_on = &on_fan_on;
  fan.on_fan_off = &on_fan_off;
  fan.on_oscillate_on = &on_oscillate_on;
  fan.on_oscillate_off = &on_oscillate_off;
  fan.on_percent_change = &on_percent_chage;

  // WiFi setup
  WiFi.begin(WIFI_SSID, WIFI_PASS);
}

void loop() {}