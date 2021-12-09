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

#pragma once

#ifndef MQTT_HVAC_H
#define MQTT_HVAC_H

#define AUX_ON true
#define AUX_OFF false

#define AWAY_ON true
#define AWAY_OFF false

#include "../Command/Command.h"
#include "../State/State.h"
#include "Component.h"

namespace HA_MQTT {
  class HVAC : public Component {
   public:
    // Enums
    enum class HVACAction {
      off,
      heating,
      cooling,
      drying,
      idle,
      fan
    };

    enum class HVACFanMode {
      automatic,
      low,
      medium,
      high
    };

    enum class HVACMode {
      automatic,
      off,
      cool,
      heat,
      dry,
      fan_only
    };

    // Constructors
    HVAC(const char* _node_id) : Component("climate", _node_id),
                                 action_state(this, "action_state", "action_topic"),
                                 aux_state(this, "aux_state", "aux_state_topic"),
                                 aux_command(this, "aux_set", "aux_command_topic"),
                                 away_state(this, "away_state", "away_mode_state_topic"),
                                 away_command(this, "away_set", "away_mode_command_topic"),
                                 current_temperature(this, "ct_state", "current_temperature_topic"),
                                 fan_mode(this, "fan_state", "fan_mode_state_topic"),
                                 fan_mode_command(this, "fan_set", "fan_mode_command_topic"),
                                 mode(this, "mode_state", "mode_state_topic"),
                                 mode_command(this, "mode_set", "mode_command_topic"),
                                 swing_mode(this, "swing_state", "swing_mode_state_topic"),
                                 swing_mode_command(this, "swing_set", "swing_mode_command_topic"),
                                 temperature(this, "temp_state", "temperature_state_topic"),
                                 temperature_command(this, "temp_set", "temperature_command_topic"),
                                 temperature_high(this, "temp_high_state", "temperature_high_state_topic"),
                                 temperature_high_command(this, "temp_high_set", "temperature_high_command_topic"),
                                 temperature_low(this, "temp_low_state", "temperature_low_state_topic"),
                                 temperature_low_command(this, "temp_low_set", "temperature_low_command_topic") {
      // Setup states
      action_state.get_state_string = [&]() { return get_action_string(); };
      aux_state.get_state_string = [&]() { return get_aux_state_string(); };
      away_state.get_state_string = [&]() { return get_away_state_string(); };
      current_temperature.get_state_string = [&]() { return get_current_temperature_string(); };
      fan_mode.get_state_string = [&]() { return get_fan_mode_string(); };
      mode.get_state_string = [&]() { return get_mode_string(); };
      swing_mode.get_state_string = [&]() { return get_swing_mode_string(); };
      temperature.get_state_string = [&]() { return get_temperature_string(); };
      temperature_high.get_state_string = [&]() { return get_high_temperature_string(); };
      temperature_low.get_state_string = [&]() { return get_low_temperature_string(); };

      // Setup commands
      aux_command.add_valid_input("ON", &on_aux_on);
      aux_command.add_valid_input("OFF", &on_aux_off);

      away_command.add_valid_input("ON", &on_aux_on);
      away_command.add_valid_input("OFF", &on_aux_off);

      fan_mode_command.add_valid_input("auto", &on_fan_auto);
      fan_mode_command.add_valid_input("low", &on_fan_low);
      fan_mode_command.add_valid_input("medium", &on_fan_medium);
      fan_mode_command.add_valid_input("high", &on_fan_high);

      mode_command.add_valid_input("auto", &on_mode_auto);
      mode_command.add_valid_input("off", &on_mode_off);
      mode_command.add_valid_input("cool", &on_mode_cool);
      mode_command.add_valid_input("heat", &on_mode_heat);
      mode_command.add_valid_input("dry", &on_mode_dry);
      mode_command.add_valid_input("fan_only", &on_mode_fan_only);

      swing_mode_command.add_valid_input("on", &on_swing_on);
      swing_mode_command.add_valid_input("off", &on_swing_off);

      temperature_command.add_valid_input("FLOAT", &on_temperature_change);
      temperature_high_command.add_valid_input("FLOAT", &on_high_change);
      temperature_low_command.add_valid_input("FLOAT", &on_low_change);
    }
    ~HVAC(){};

    // Config Methods
    void create_config_message(JsonObject& _json) override {
      if (enable_action_flag) {
        action_state.set_state_config(_json);
      }

      if (enable_aux_heat_flag) {
        aux_state.set_state_config(_json);
        aux_command.set_command_config(_json);
      }

      if (enable_away_flag) {
        away_state.set_state_config(_json);
        away_command.set_command_config(_json);
      }

      current_temperature.set_state_config(_json);

      if (enable_fan_mode_flag) {
        fan_mode.set_state_config(_json);
        fan_mode_command.set_command_config(_json);
      }

      mode.set_state_config(_json);
      mode_command.set_command_config(_json);

      if (enable_swing_mode_flag) {
        swing_mode.set_state_config(_json);
        swing_mode_command.set_command_config(_json);
      }

      temperature.set_state_config(_json);
      temperature_command.set_command_config(_json);

      if (enable_min_max_setpoint_flag) {
        temperature_high.set_state_config(_json);
        temperature_high_command.set_command_config(_json);
        temperature_low.set_state_config(_json);
        temperature_low_command.set_command_config(_json);
      }

      _json["initial"] = initial_temperature;
      _json["max_temp"] = max_set_temperature;
      _json["min_temp"] = min_set_temperature;
      _json["precision"] = temperature_precision;
      _json["temperature_unit"] = temperature_uint;
      _json["temp_step"] = temperature_step;
    }

    // State methods
    std::string get_action_string() {
      switch (action_state.get_state()) {
        case HVACAction::off:
          return "off";
        case HVACAction::heating:
          return "heating";
        case HVACAction::cooling:
          return "cooling";
        case HVACAction::drying:
          return "drying";
        case HVACAction::fan:
          return "fan";
        case HVACAction::idle:
          return "idle";
        default:
          return "unkown";
      }
    }
    std::string get_aux_state_string() {
      return (aux_state.get_state()) ? "ON" : "OFF";
    }
    std::string get_away_state_string() {
      return (away_state.get_state()) ? "ON" : "OFF";
    }
    std::string get_current_temperature_string() {
      return to_string(current_temperature.get_state());
    }
    std::string get_fan_mode_string() {
      switch (fan_mode.get_state()) {
        case HVACFanMode::automatic:
          return "auto";
        case HVACFanMode::low:
          return "low";
        case HVACFanMode::medium:
          return "medium";
        case HVACFanMode::high:
          return "high";
        default:
          return "unknown";
      }
    }
    std::string get_mode_string() {
      switch (mode.get_state()) {
        case HVACMode::automatic:
          return "auto";
        case HVACMode::off:
          return "off";
        case HVACMode::cool:
          return "cool";
        case HVACMode::heat:
          return "heat";
        case HVACMode::dry:
          return "dry";
        case HVACMode::fan_only:
          return "fan_only";
        default:
          return "unknown";
      }
    }
    std::string get_swing_mode_string() {
      return (swing_mode.get_state()) ? "on" : "off";
    }
    std::string get_temperature_string() {
      return to_string(temperature.get_state(), 2);
    }
    std::string get_high_temperature_string() {
      return to_string(temperature_high.get_state(), 2);
    }
    std::string get_low_temperature_string() {
      return to_string(temperature_low.get_state(), 2);
    }

    // Event Methods
    EventCallback on_aux_on = [&](void*) { aux_state.update_state(true); };
    EventCallback on_aux_off = [&](void*) { aux_state.update_state(false); };

    EventCallback on_away_on = [&](void*) { away_state.update_state(true); };
    EventCallback on_away_off = [&](void*) { away_state.update_state(false); };

    EventCallback on_fan_auto = [&](void*) { fan_mode.update_state(HVACFanMode::automatic); };
    EventCallback on_fan_low = [&](void*) { fan_mode.update_state(HVACFanMode::low); };
    EventCallback on_fan_medium = [&](void*) { fan_mode.update_state(HVACFanMode::medium); };
    EventCallback on_fan_high = [&](void*) { fan_mode.update_state(HVACFanMode::high); };

    EventCallback on_mode_auto = [&](void*) { mode.update_state(HVACMode::automatic); };
    EventCallback on_mode_off = [&](void*) { mode.update_state(HVACMode::off); };
    EventCallback on_mode_cool = [&](void*) { mode.update_state(HVACMode::cool); };
    EventCallback on_mode_heat = [&](void*) { mode.update_state(HVACMode::heat); };
    EventCallback on_mode_dry = [&](void*) { mode.update_state(HVACMode::dry); };
    EventCallback on_mode_fan_only = [&](void*) { mode.update_state(HVACMode::fan_only); };

    EventCallback on_swing_on = [&](void*) { swing_mode.update_state(true); };
    EventCallback on_swing_off = [&](void*) { swing_mode.update_state(false); };

    EventCallback on_temperature_change = [&](void* _num) { temperature.update_state(*((float*)_num)); };
    EventCallback on_high_change = [&](void* _num) { temperature_high.update_state(*((float*)_num)); };
    EventCallback on_low_change = [&](void* _num) { temperature_low.update_state(*((float*)_num)); };

    // Control methods
    void set_initial_temperature(float _temp) { initial_temperature = _temp; }
    void set_min_temperature(float _temp) { min_set_temperature = _temp; }
    void set_max_temperature(float _temp) { max_set_temperature = _temp; }
    void set_temperature_precision(float _precision) {
      if (_precision == 0.1)
        temperature_precision = 0.1;
      else if (_precision == 0.5)
        temperature_precision = 0.5;
      else
        temperature_precision = 1.0;
    }
    void set_unit(const char* _unit) { temperature_uint = _unit; }
    void set_step_size(float _step) { temperature_step = _step; }
    void enable_actions() { enable_action_flag = true; }
    void enable_aux_heat() { enable_aux_heat_flag = true; }
    void enable_fan_mode() { enable_fan_mode_flag = true; }
    void enable_swing() { enable_swing_mode_flag = true; }
    void enable_min_max_setpoints() { enable_min_max_setpoint_flag = true; }

    // Update methods
    void send_update_message(uint8_t _qos = 0, bool _retain = false) override {
      aux_state.send_state_message(_qos, _retain);
      away_state.send_state_message(_qos, _retain);
      current_temperature.send_state_message(_qos, _retain);
      fan_mode.send_state_message(_qos, _retain);
      mode.send_state_message(_qos, _retain);
      swing_mode.send_state_message(_qos, _retain);
      temperature.send_state_message(_qos, _retain);
      temperature_high.send_state_message(_qos, _retain);
      temperature_low.send_state_message(_qos, _retain);
    }

    // Loop Method
    void loop() override {
      aux_state.loop();
      away_state.loop();
      current_temperature.loop();
      fan_mode.loop();
      mode.loop();
      swing_mode.loop();
      temperature.loop();
      temperature_high.loop();
      temperature_low.loop();
    }

   public:
    // Action data
    bool enable_action_flag = false;
    State<HVACAction> action_state;

    // Aux heat data
    bool enable_aux_heat_flag = false;
    State<bool> aux_state;
    Command aux_command;

    // Away mode data
    bool enable_away_flag = false;
    State<bool> away_state;
    Command away_command;

    // Current temperature data
    State<float> current_temperature;

    // Fan mode data
    bool enable_fan_mode_flag = false;
    State<HVACFanMode> fan_mode;
    Command fan_mode_command;

    // Mode data
    State<HVACMode> mode;
    Command mode_command;

    // Power data
    // Command power_command;

    // Swing mode data
    bool enable_swing_mode_flag = false;
    State<bool> swing_mode;
    Command swing_mode_command;

    // Temperature data
    bool enable_min_max_setpoint_flag = false;
    State<float> temperature;
    Command temperature_command;

    // Temperature high data
    State<float> temperature_high;
    Command temperature_high_command;

    // Temperature low data
    State<float> temperature_low;
    Command temperature_low_command;

   protected:
    float initial_temperature = 24;
    float min_set_temperature = 12;
    float max_set_temperature = 35;
    float temperature_precision = 0.1;
    const char* temperature_uint = "C";
    float temperature_step = 1;
  };

}  // namespace HA_MQTT

#endif