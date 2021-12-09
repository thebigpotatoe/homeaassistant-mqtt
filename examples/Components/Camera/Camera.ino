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

// Camera example

// WiFi Data
#define WIFI_SSID "your_wifi_ssid"
#define WIFI_PASS "your_password"

// MQTT Data
#define HA_MQTT_HOST_NAME "your_broker_hostname_or_ip"
#define HA_MQTT_PORT 1883
#define HA_MQTT_RESET_ALL_COMPONENTS true

// Includes
#include "Homeassistant_MQTT.h"

// Camera object
Camera camera("test camera");
uint8_t* camera_image[100];

void setup() {
  // Serial for debug
  // Serial.begin(115200);
  // Serial.println();

  // Set the camera state variable to the image variable
  camera.state.set_state_variable(camera_image);

  // WiFi setup
  WiFi.begin(WIFI_SSID, WIFI_PASS);
}

void loop() {}