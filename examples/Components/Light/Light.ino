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

// Light example

// WiFi Data
#define WIFI_SSID "your_wifi_ssid"
#define WIFI_PASS "your_password"

// MQTT Data
#define HA_MQTT_HOST_NAME "your_broker_hostname_or_ip"
#define HA_MQTT_PORT 1883
#define HA_MQTT_RESET_ALL_COMPONENTS true

// Includes
#include "Homeassistant_MQTT.h"

// Alarm object
#define ALARM_CODE "my code"
Alarm alarm("test alarm", ALARM_CODE);

// Sketch Data
unsigned long loop_timer = 0;

// Callbacks
void disarm(void* _data) {
  alarm.state.update_state(Alarm::AlarmState::disarming);
  unlock();
  alarm.state.update_state(Alarm::AlarmState::disarmed);
}
void arm_away(void*) {
  alarm.state.update_state(Alarm::AlarmState::arming);
  lock();
  alarm.state.update_state(Alarm::AlarmState::armed_away);
}
void arm_home(void*) {
  alarm.state.update_state(Alarm::AlarmState::arming);
  lock();
  alarm.state.update_state(Alarm::AlarmState::armed_home);
}
void arm_night(void*) {
  alarm.state.update_state(Alarm::AlarmState::arming);
  lock();
  alarm.state.update_state(Alarm::AlarmState::armed_night);
}
void arm_vacation(void*) {
  alarm.state.update_state(Alarm::AlarmState::arming);
  lock();
  alarm.state.update_state(Alarm::AlarmState::armed_vacation);
}
void custom_bypass(void*) {
  alarm.state.update_state(Alarm::AlarmState::arming);
  lock();
  alarm.state.update_state(Alarm::AlarmState::armed_custom_bypass);
}

// Lock functions
void setup_lock() {
  pinMode(5, OUTPUT);
}
void lock() {
  digitalWrite(5, HIGH);
}
void unlock() {
  digitalWrite(5, LOW);
}

void setup() {
  // Serial for debug
  // Serial.begin(115200);
  // Serial.println();

  // Actual output setup
  setup_lock();

  // Setup alarm callbacks
  alarm.on_disarm = &disarm;
  alarm.on_arm_away = &arm_away;
  alarm.on_arm_home = &arm_home;
  alarm.on_arm_night = &arm_night;
  alarm.on_arm_vacation = &arm_vacation;
  alarm.on_custom_bypass = &custom_bypass;

  // WiFi setup
  WiFi.begin(WIFI_SSID, WIFI_PASS);
}

void loop() {}