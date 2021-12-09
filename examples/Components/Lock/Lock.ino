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

// Lock example

// WiFi Data
#define WIFI_SSID "your_wifi_ssid"
#define WIFI_PASS "your_password"

// MQTT Data
#define HA_MQTT_HOST_NAME "your_broker_hostname_or_ip"
#define HA_MQTT_PORT 1883
#define HA_MQTT_RESET_ALL_COMPONENTS true

// Includes
#include "Homeassistant_MQTT.h"

// Lock object
Lock lock("test lock");

// Sketch Data
unsigned long loop_timer = 0;

// Callbacks
void on_lock(void* _data) {
  lock.state.update_state(Lock::LockState::locking);
  handle_lock();
}
void on_unlock(void*) {
  lock.state.update_state(Lock::LockState::unlocking);
  handle_unlock();
}

// Lock functions
void setup_lock() {
  pinMode(5, OUTPUT);
}
void handle_lock() {
  digitalWrite(5, HIGH);
  lock.state.update_state(Lock::LockState::locked);
}
void handle_unlock() {
  digitalWrite(5, LOW);
  lock.state.update_state(Lock::LockState::unlocked);
}

void setup() {
  // Serial for debug
  // Serial.begin(115200);
  // Serial.println();

  // Actual output setup
  setup_lock();

  // Setup callbacks
  lock.on_lock = &on_lock;
  lock.on_unlock = &on_unlock;

  // WiFi setup
  WiFi.begin(WIFI_SSID, WIFI_PASS);
}

void loop() {}