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

// HVAC example

// WiFi Data
#define WIFI_SSID "your_wifi_ssid"
#define WIFI_PASS "your_password"

// MQTT Data
#define HA_MQTT_HOST_NAME "your_broker_hostname_or_ip"
#define HA_MQTT_PORT 1883
#define HA_MQTT_RESET_ALL_COMPONENTS true

// Includes
#include "Homeassistant_MQTT.h"

// HVAC object
#define FanPin 5
HVAC thermostat("test thermostat");

// Sketch Data
unsigned long loop_timer = 0;

// HVAC functions
void setup_thermostat() {
  pinMode(FanPin, OUTPUT);
}
void update_temperature(float _temp) {
  // Used to send the current room temperature back to home assistant
  thermostat.current_temperature = _temp;
}
void set_fan(HVAC::HVACFanMode _mode) {
  // HVAC::HVACFanMode _mode = thermostat.fan_mode.get_state();
  thermostat.fan_mode.update_state(_mode);

  switch (_mode) {
    case HVAC::HVACFanMode::automatic:
      // Set the fan used to automatic, user specific method
      break;
    case HVAC::HVACFanMode::low:
      // Set the fan used to low
      analogWrite(FanPin, 333);
      break;
    case HVAC::HVACFanMode::medium:
      // Set the fan used to medium
      analogWrite(FanPin, 666);
      break;
    case HVAC::HVACFanMode::high:
      // Set the fan used to high
      analogWrite(FanPin, 1024);
      break;
    default:
      break;
  }
}
void set_mode(HVAC::HVACMode _mode) {
  Serial.println((int)_mode);
  thermostat.mode = _mode;
  switch (_mode) {
    case HVAC::HVACMode::automatic:
      // Set the device to automatic mode
      break;
    case HVAC::HVACMode::off:
      // Set the device to off mode
      break;
    case HVAC::HVACMode::cool:
      // Set the device to cool mode
      break;
    case HVAC::HVACMode::heat:
      // Set the device to heat mode
      break;
    case HVAC::HVACMode::dry:
      // Set the device to dry mode
      break;
    case HVAC::HVACMode::fan_only:
      // Set the device to fan mode
      break;
    default:
      break;
  }
}
void set_swing(bool _state) {
  // Set the swing of the HVAC system as desired
  thermostat.swing_mode = _state;
}
void set_setpoint(float _setpoint) {
  // set the setpoint of the HVAC system
  thermostat.temperature = _setpoint;
}
void set_high(float _setpoint) {
  // set the high setpoint of the HVAC system
  thermostat.temperature_high = _setpoint;
}
void set_low(float _setpoint) {
  // set the low setpoint of the HVAC system
  thermostat.temperature_low = _setpoint;
}

// Callbacks
void on_aux_on(void* _data) {
  thermostat.aux_state = AUX_ON;
}
void on_aux_off(void* _data) {
  thermostat.aux_state = AUX_OFF;
}
void on_away_on(void* _data) {
  thermostat.away_state = AWAY_ON;
}
void on_away_off(void* _data) {
  thermostat.away_state = AWAY_OFF;
}
void on_fan_auto(void* _data) {
  set_fan(HVAC::HVACFanMode::automatic);
}
void on_fan_low(void* _data) {
  set_fan(HVAC::HVACFanMode::low);
}
void on_fan_medium(void* _data) {
  set_fan(HVAC::HVACFanMode::medium); 
}
void on_fan_high(void* _data) {
  set_fan(HVAC::HVACFanMode::high);
}
void on_mode_auto(void* _data) {
  set_mode(HVAC::HVACMode::automatic);
}
void on_mode_off(void* _data) {
  set_mode(HVAC::HVACMode::off);
}
void on_mode_cool(void* _data) {
  set_mode(HVAC::HVACMode::cool);
}
void on_mode_heat(void* _data) {
  set_mode(HVAC::HVACMode::heat);
}
void on_mode_dry(void* _data) {
  set_mode(HVAC::HVACMode::dry);
}
void on_mode_fan_only(void* _data) {
  set_mode(HVAC::HVACMode::fan_only);
}
void on_swing_on(void* _data) {
  set_swing(thermostat.swing_mode.cast_to_type(_data));
}
void on_swing_off(void* _data) {
  set_swing(thermostat.swing_mode.cast_to_type(_data));
}
void on_temperature_change(void* _data) {
  set_setpoint(thermostat.temperature.cast_to_type(_data));
}
void on_high_change(void* _data) {
  set_high(thermostat.temperature_high.cast_to_type(_data));
}
void on_low_change(void* _data) {
  set_low(thermostat.temperature_low.cast_to_type(_data));
}

void setup() {
  // Serial for debug
  // Serial.begin(115200);
  // Serial.println();

  // Actual output setup
  setup_thermostat();

  // Setuop the thermostat as required
  thermostat.set_unit("C");
  thermostat.set_temperature_precision(0.1);
  thermostat.set_initial_temperature(24);
  thermostat.set_min_temperature(18);
  thermostat.set_max_temperature(32);
  thermostat.set_step_size(1);
  // thermostat.enable_actions();
  // thermostat.enable_aux_heat();
  // thermostat.enable_fan_mode();
  // thermostat.enable_mode();
  // thermostat.enable_swing();
  // thermostat.enable_min_max_setpoints();

  // Setup HVAC callbacks
  thermostat.on_aux_on = &on_aux_on;
  thermostat.on_aux_off = &on_aux_off;
  thermostat.on_away_on = &on_away_on;
  thermostat.on_away_off = &on_away_off;
  thermostat.on_fan_auto = &on_fan_auto;
  thermostat.on_fan_low = &on_fan_low;
  thermostat.on_fan_medium = &on_fan_medium;
  thermostat.on_fan_high = &on_fan_high;
  thermostat.on_mode_auto = &on_mode_auto;
  thermostat.on_mode_off = &on_mode_off;
  thermostat.on_mode_cool = &on_mode_cool;
  thermostat.on_mode_heat = &on_mode_heat;
  thermostat.on_mode_dry = &on_mode_dry;
  thermostat.on_mode_fan_only = &on_mode_fan_only;
  thermostat.on_swing_on = &on_swing_on;
  thermostat.on_swing_off = &on_swing_off;
  thermostat.on_temperature_change = &on_temperature_change;
  thermostat.on_high_change = &on_high_change;
  thermostat.on_low_change = &on_low_change;

  // WiFi setup
  WiFi.begin(WIFI_SSID, WIFI_PASS);
}

void loop() {
  if (millis() - loop_timer > 2000) {
    // Falsify the temperature updating
    thermostat.current_temperature = thermostat.current_temperature + 1;
    if (thermostat.current_temperature > 32) thermostat.current_temperature = 18;

    loop_timer = millis();
  }
}