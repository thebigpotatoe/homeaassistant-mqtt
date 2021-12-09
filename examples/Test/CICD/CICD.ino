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

// CI/CD test
// Used for checking the library compiles
// Used in github CI/CD

#define HA_MQTT_HOST_NAME "your_broker_hostname_or_ip"
#define HA_MQTT_PORT 1883
// #define HA_MQTT_PREFIX nullptr
// #define HA_MQTT_DEVICE_NAME nullptr

#include "Arduino.h"
#include "ESP8266WiFi.h"
#include "Homeassistant_MQTT.h"

Alarm alarm_test("lil_alarm", "code");
BinarySensor bin_test("lil_binsen", "lock");
Cover cover_test("lil_cover");
Fan fan_test("lil_fan");
Humidifier humidifier_test("lil_hum");
HVAC hvac_test("lil_hvac");
Light light_test("lil_light");
Lock lock_test("lil_lock");
Number number_test("lil_number");
Scene test_scene("lil_scene");
Select test_select("lil_select");
Sensor test_sensor("lil_sensor");
TagScanner test_scanner("lil_scanner");
Switch switch_test("lil_switch");
Tracker test_tracker("lil_tracker");
Trigger test_trigger("lil_trigger", Trigger::TriggerType::button_short_press, Trigger::TriggerSubType::button_1);
Vacuum test_vacuum("lil_vac");

unsigned long start_time;

void setup() {
    WiFi.begin("your_wifi_ssid", "your_password");
}

void loop() {
  if (millis() - start_time > 1000) {
    Serial.printf("%.2f %%\n", (float)ESP.getFreeHeap()/819.20);
    start_time = millis();
  }
}