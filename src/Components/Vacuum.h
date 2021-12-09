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

#ifndef MQTT_VACUUM_H
#define MQTT_VACUUM_H

#include "../Command/Command.h"
#include "../State/State.h"
#include "Component.h"

namespace HA_MQTT {
  class Vacuum : public Component {
   public:
    // Enums
    enum class VacState {
      cleaning,
      docked,
      paused,
      idle,
      returning,
      error
    };
    enum class FanSpeed {
      off,
      low,
      medium,
      high,
      max
    };

    // Typedefs
    typedef std::function<void(void)> StateCallback;

    // Constructors
    Vacuum(const char* _node_id) : Component("vacuum", _node_id),
                                   state(this, "state"),
                                   fan_speed(this, "state"),
                                   battery(this, "state"),
                                   command(this, "set"),
                                   fan_speed_command(this, "set_fan_speed", "set_fan_speed_topic"),
                                   custom_command(this, "set_custom_command", "send_command_topic") {
      // Setup the states
      state.get_state_string = [&]() { return get_sensor_state_string(); };
      fan_speed.get_state_string = [&]() { return get_fan_speed_string(); };
      battery.get_state_string = [&]() { return get_battery_string(); };
      
      // Setup the commands
      command.add_valid_input("start", &on_start);
      command.add_valid_input("stop", &on_stop);
      command.add_valid_input("pause", &on_pause);
      command.add_valid_input("return_to_base", &on_return_to_base);
      command.add_valid_input("locate", &on_locate);
      command.add_valid_input("clean_spot", &on_clean_spot);
      fan_speed_command.add_valid_input("off", &on_fan_off);
      fan_speed_command.add_valid_input("low", &on_fan_low);
      fan_speed_command.add_valid_input("medium", &on_fan_medium);
      fan_speed_command.add_valid_input("high", &on_fan_high);
      fan_speed_command.add_valid_input("max", &on_fan_max);
    }
    ~Vacuum() {}

    // To create the rest of the config message on startup
    void create_config_message(JsonObject& _json) override {
      // State config
      state.set_state_config(_json);

      // Commands config
      command.set_command_config(_json);
      fan_speed_command.set_command_config(_json);
      custom_command.set_command_config(_json);

      // Vacuum specific config
      _json["schema"] = "state";

      // Add supported features
      JsonArray supported_features = _json.createNestedArray("supported_features");
      supported_features.add("start");
      supported_features.add("pause");
      supported_features.add("stop");
      supported_features.add("return_home");
      supported_features.add("battery");
      // supported_features.add("status");
      supported_features.add("locate");
      supported_features.add("clean_spot");
      supported_features.add("fan_speed");
      // supported_features.add("send_command");

      // Add supported fan speeds
      JsonArray fan_speeds = _json.createNestedArray("fan_speed_list");
      fan_speeds.add("off");
      fan_speeds.add("low");
      fan_speeds.add("medium");
      fan_speeds.add("high");
      fan_speeds.add("max");
    }

    // State methods
    std::string get_sensor_state_string() {
      switch (state.get_state()) {
        case VacState::cleaning:
          return "cleaning";
        case VacState::docked:
          return "docked";
        case VacState::paused:
          return "paused";
        case VacState::idle:
          return "idle";
        case VacState::returning:
          return "returning";
        case VacState::error:
          return "error";
        default:
          return "";
      }
    }
    std::string get_fan_speed_string() {
      switch (fan_speed.get_state()) {
        case FanSpeed::off:
          return "off";
        case FanSpeed::low:
          return "low";
        case FanSpeed::medium:
          return "medium";
        case FanSpeed::high:
          return "high";
        case FanSpeed::max:
          return "max";
        default:
          return "";
      }
    }
    std::string get_battery_string() {
      return to_string(battery.get_state());
    }
    void send_state_message(uint8_t _qos = 0, bool _retain = false) {
      std::string msg = std::string("{\"battery_level\": BATTERY_STRING, \"state\": \"STATE_STRING\", \"fan_speed\": \"FAN_SPEED\", \"status\" : \"test\"}");
      msg.replace(msg.find("BATTERY_STRING"), 14, battery.get_state_string());
      msg.replace(msg.find("STATE_STRING"), 12, state.get_state_string());
      msg.replace(msg.find("FAN_SPEED"), 9, fan_speed.get_state_string());
      state.send_state_message(_qos, _retain, msg.c_str());
    }

    // Event Methods
    EventCallback on_start = [&](void*) { state.update_state(VacState::cleaning); };
    EventCallback on_stop = [&](void*) { state.update_state(VacState::idle); };
    EventCallback on_pause = [&](void*) { state.update_state(VacState::paused); };
    EventCallback on_return_to_base = [&](void*) { state.update_state(VacState::returning); };
    EventCallback on_clean_spot = [&](void*) { state.update_state(VacState::cleaning); };
    EventCallback on_locate = [&](void*) {};

    EventCallback on_fan_off = [&](void*) { fan_speed.update_state(FanSpeed::off); };
    EventCallback on_fan_low = [&](void*) { fan_speed.update_state(FanSpeed::low); };
    EventCallback on_fan_medium = [&](void*) { fan_speed.update_state(FanSpeed::medium); };
    EventCallback on_fan_high = [&](void*) { fan_speed.update_state(FanSpeed::high); };
    EventCallback on_fan_max = [&](void*) { fan_speed.update_state(FanSpeed::max); };

    // Update methods
    void send_update_message(uint8_t _qos = 0, bool _retain = false) override {
      send_state_message(_qos, _retain);
    }

    // Loop for the component
    void loop() override {
      state.loop();
      fan_speed.loop();
      battery.loop();
    }

   public:
    // Data
    State<VacState> state;
    State<FanSpeed> fan_speed;
    State<uint8_t> battery;
    Command command;
    Command fan_speed_command;
    Command custom_command;
  };

}  // namespace HA_MQTT

#endif