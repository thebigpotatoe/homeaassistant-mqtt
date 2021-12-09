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

// Vacuum example

// WiFi Data
#define WIFI_SSID "your_wifi_ssid"
#define WIFI_PASS "your_password"

// MQTT Data
#define HA_MQTT_HOST_NAME "your_broker_hostname_or_ip"
#define HA_MQTT_PORT 1883
#define HA_MQTT_RESET_ALL_COMPONENTS true

// Includes
#include "Homeassistant_MQTT.h"

// Vacuum object
Vacuum vacuum("test vacuum");

// Sketch Data
unsigned long loop_timer = 0;

// Vacuum functions
void start_cleaning() {
  // do what is needed here
  vacuum.state = Vacuum::VacState::cleaning;
}
void stop_cleaning() {
  // do what is needed here
  vacuum.state = Vacuum::VacState::idle;
}
void pause_cleaning() {
  // do what is needed here
  vacuum.state = Vacuum::VacState::paused;
}
void return_to_base() {
  // do what is needed here
  vacuum.state = Vacuum::VacState::returning;
}
void clean_spot() {
  // do what is needed here
  vacuum.state = Vacuum::VacState::cleaning;
}
void locate() {
  // do what is needed here
}
void change_fan_speed(Vacuum::FanSpeed _state) {
  vacuum.fan_speed = _state;
  switch (_state) {
    case Vacuum::FanSpeed::off:
      break;
    case Vacuum::FanSpeed::low:
      break;
    case Vacuum::FanSpeed::medium:
      break;
    case Vacuum::FanSpeed::high:
      break;
    case Vacuum::FanSpeed::max:
      break;
    default:
      break;
  }
}

// Callbacks
void on_start(void* _data) {
  start_cleaning();
}
void on_stop(void* _data) {
  stop_cleaning();
}
void on_pause(void* _data) {
  pause_cleaning();
}
void on_return_to_base(void* _data) {
  return_to_base();
}
void on_clean_spot(void* _data) {
  clean_spot();
}
void on_locate(void* _data) {
  locate();
}
void on_fan_off(void* _data) {
  change_fan_speed(Vacuum::FanSpeed::off);
}
void on_fan_low(void* _data) {
  change_fan_speed(Vacuum::FanSpeed::low);
}
void on_fan_medium(void* _data) {
  change_fan_speed(Vacuum::FanSpeed::medium);
}
void on_fan_high(void* _data) {
  change_fan_speed(Vacuum::FanSpeed::high);
}
void on_fan_max(void* _data) {
  change_fan_speed(Vacuum::FanSpeed::max);
}

void setup() {
  // Serial for debug
  // Serial.begin(115200);
  // Serial.println();

  // Setup callbacks
  vacuum.on_start = &on_start;
  vacuum.on_stop = &on_stop;
  vacuum.on_pause = &on_pause;
  vacuum.on_return_to_base = &on_return_to_base;
  vacuum.on_clean_spot = &on_clean_spot;
  vacuum.on_locate = &on_locate;
  vacuum.on_fan_off = &on_fan_off;
  vacuum.on_fan_low = &on_fan_low;
  vacuum.on_fan_medium = &on_fan_medium;
  vacuum.on_fan_high = &on_fan_high;
  vacuum.on_fan_max = &on_fan_max;

  // WiFi setup
  WiFi.begin(WIFI_SSID, WIFI_PASS);
}

void loop() {
  if (millis() - loop_timer > 2000) {
    // Update the battery every 2 seconds
    vacuum.battery = random(0, 100);
    loop_timer = millis();
  }
}