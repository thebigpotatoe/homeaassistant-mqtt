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

// Binary sensor example
// Supported types are:
// - None: Generic on/off. This is the default and doesn’t need to be set.
// - battery: on means low, off means normal
// - battery_charging: on means charging, off means not charging
// - cold: on means cold, off means normal
// - connectivity: on means connected, off means disconnected
// - door: on means open, off means closed
// - garage_door: on means open, off means closed
// - gas: on means gas detected, off means no gas (clear)
// - heat: on means hot, off means normal
// - light: on means light detected, off means no light
// - lock: on means open (unlocked), off means closed (locked)
// - moisture: on means moisture detected (wet), off means no moisture (dry)
// - motion: on means motion detected, off means no motion (clear)
// - moving: on means moving, off means not moving (stopped)
// - occupancy: on means occupied, off means not occupied (clear)
// - opening: on means open, off means closed
// - plug: on means device is plugged in, off means device is unplugged
// - power: on means power detected, off means no power
// - presence: on means home, off means away
// - problem: on means problem detected, off means no problem (OK)
// - running: on means running, off means not running
// - safety: on means unsafe, off means safe
// - smoke: on means smoke detected, off means no smoke (clear)
// - sound: on means sound detected, off means no sound (clear)
// - tamper: on means tampering detected, off means no tampering (clear)
// - update: on means update available, off means up-to-date
// - vibration: on means vibration detected, off means no vibration (clear)
// - window: on means open, off means closed

// WiFi Data
#define WIFI_SSID "your_wifi_ssid"
#define WIFI_PASS "your_password"

// MQTT Data
#define HA_MQTT_HOST_NAME "your_broker_hostname_or_ip"
#define HA_MQTT_PORT 1883
#define HA_MQTT_RESET_ALL_COMPONENTS true

// Includes
#include "Homeassistant_MQTT.h"

// Binary sensor objects
BinarySensor door("test door", "door");
BinarySensor light("test light", "light");
BinarySensor lock("test lock", "lock");
BinarySensor motion("test motion", "motion");
BinarySensor window("test window", "window");

// Sketch Data
unsigned long loop_timer = 0;
bool state_variable = false;

void setup() {
  // Serial for debug
  // Serial.begin(115200);
  // Serial.println();

  // Component setup
  window.state.set_state_variable(&state_variable);

  // WiFi setup
  WiFi.begin(WIFI_SSID, WIFI_PASS);
}

void loop() {
  if (millis() - loop_timer > 2000) {
    // Toggle the state of some sensors to show its working
    door.state.update_state(!door.state.get_state());
    light.state.update_state(!light.state.get_state());
    lock.state.update_state(!lock.state.get_state());
    motion.state.update_state(!motion.state.get_state());

    // Toggle the state of an external variable to show its working
    state_variable = !state_variable;

    loop_timer = millis();
  }
}