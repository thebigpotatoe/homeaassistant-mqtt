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

// Number example

// WiFi Data
#define WIFI_SSID "your_wifi_ssid"
#define WIFI_PASS "your_password"

// MQTT Data
#define HA_MQTT_HOST_NAME "your_broker_hostname_or_ip"
#define HA_MQTT_PORT 1883
#define HA_MQTT_RESET_ALL_COMPONENTS true

// Includes
#include "Homeassistant_MQTT.h"

// Number object
#define OPTIONAL_MIN_NUMBER 0 
#define OPTIONAL_MAX_NUMBER 100
#define OPTIONAL_STEP_VALUE 1
Number number("test number", OPTIONAL_STEP_VALUE, OPTIONAL_MIN_NUMBER, OPTIONAL_MAX_NUMBER);

// Sketch Data
unsigned long loop_timer = 0;

// Callbacks
void on_update(void* _data) {
  // This is the default, but change it to suit your application
  number.state.update_state(number.state.cast_to_type(_data));
}

void setup() {
  // Serial for debug
  // Serial.begin(115200);
  // Serial.println();

  // Setup callback
  number.on_number_changed = &on_update;

  // WiFi setup
  WiFi.begin(WIFI_SSID, WIFI_PASS);
}

void loop() {}