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

// Cover example

// WiFi Data
#define WIFI_SSID "your_wifi_ssid"
#define WIFI_PASS "your_password"

// MQTT Data
#define HA_MQTT_HOST_NAME "your_broker_hostname_or_ip"
#define HA_MQTT_PORT 1883
#define HA_MQTT_RESET_ALL_COMPONENTS true

// Includes
#include "Homeassistant_MQTT.h"

// Cover object
Cover cover("test cover");
#define open_pin 5
#define close_pin 4
#define stop_pin 12

// Sketch Data
unsigned long loop_timer = 0;

// Callbacks
void on_open(void*) {
  Serial.println("Opening");
  cover.state.update_state(Cover::CoverState::opening);
  open_cover();
}
void on_close(void*) {
  Serial.println("Closing");
  cover.state.update_state(Cover::CoverState::closing);
  close_cover();
}
void on_stop(void*) {
  Serial.println("Stopping");
  stop_cover();
}
void on_position(void* _pos) {
  Serial.printf("Setting position to %d\n", cover.position.cast_to_type(_pos));
  set_position(cover.position.cast_to_type(_pos));
}
void on_tilt(void* _tilt) {
  Serial.printf("Setting tilt to %d\n", cover.position.cast_to_type(_tilt));
  set_tilt(cover.tilt.cast_to_type(_tilt));
}

// Cover functions
void setup_cover() {
  pinMode(open_pin, OUTPUT);
  pinMode(close_pin, OUTPUT);
  pinMode(stop_pin, OUTPUT);
}
void open_cover() {
  digitalWrite(open_pin, HIGH);
  delay(10);
  digitalWrite(open_pin, LOW);
  cover.state.update_state(Cover::CoverState::open);
}
void close_cover() {
  digitalWrite(close_pin, HIGH);
  delay(10);
  digitalWrite(close_pin, LOW);
  cover.state.update_state(Cover::CoverState::closed);
}
void stop_cover() {
  digitalWrite(stop_pin, HIGH);
  delay(10);
  digitalWrite(stop_pin, LOW);
  cover.state.update_state(Cover::CoverState::stopped);
}
void set_position(uint8_t _pos) {
  // Could possibly set the position through velocity or positional control
  // Example of velocity control:
  // open_cover();
  // delay(_pos * 10);
  // stop_cover();
  cover.position.update_state(_pos);
}
void set_tilt(uint8_t _tilt) {
  // Set the tilt through any method, perhaps a stepper motor
  // Example:
  // set_stepper_position(_tilt);
  cover.tilt.update_state(_tilt);
}

void setup() {
  // Serial for debug
  // Serial.begin(115200);
  // Serial.println();

  // Actual output setup
  setup_cover();

  // Setup the cover as required
  cover.enable_state();
  // cover.disable_state();
  cover.enable_position();
  // cover.disable_position();
  cover.enable_tilt();
  // cover.disable_tilt();

  // Setup cover callbacks
  cover.on_open = &on_open;
  cover.on_close = &on_close;
  cover.on_stop = &on_stop;
  cover.on_position = &on_position;
  cover.on_tilt = &on_tilt;

  // WiFi setup
  WiFi.begin(WIFI_SSID, WIFI_PASS);
}

void loop() {}