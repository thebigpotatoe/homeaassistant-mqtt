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

#ifndef MQTT_FAN_H
#define MQTT_FAN_H

#define FAN_ON true
#define FAN_OFF false
#define OSCILLATE_ON true
#define OSCILLATE_OFF false

#include "../Command/Command.h"
#include "../State/State.h"
#include "Component.h"

namespace HA_MQTT {
  class Fan : public Component {
   public:
    // Constructor
    Fan(const char* _node_id) : Component("fan", _node_id),
                                state(this, "state"),
                                command(this, "set"),
                                oscillate(this, "oscillate_state", "oscillation_state_topic"),
                                oscillate_command(this, "oscillate_set", "oscillation_command_topic"),
                                percent(this, "percentage_state", "percentage_state_topic"),
                                percent_command(this, "percentage_set", "percentage_command_topic") {
      // Setup states
      state.get_state_string = [&]() { return get_fan_state_string(); };
      oscillate.get_state_string = [&]() { return get_oscillate_string(); };
      percent.get_state_string = [&]() { return get_percent_string(); };

      // Setup commands
      command.add_valid_input("ON", &on_fan_on);
      command.add_valid_input("OFF", &on_fan_off);
      oscillate_command.add_valid_input("oscillate_on", &on_oscillate_on);
      oscillate_command.add_valid_input("oscillate_off", &on_oscillate_off);
      percent_command.add_valid_input("INTEGER", &on_percent_change);
    }
    ~Fan() {}

    // Config Methods
    void create_config_message(JsonObject& _json) override {
      // Cover config
      state.set_state_config(_json);
      command.set_command_config(_json);
      _json["optimistic"] = state_optimistic;

      // Oscillate config variables
      oscillate.set_state_config(_json);
      oscillate_command.set_command_config(_json);

      // Percent config variables
      percent.set_state_config(_json);
      percent_command.set_command_config(_json);
    }

    // State methods
    std::string get_fan_state_string() {
      return (state.get_state()) ? std::string("ON") : std::string("OFF");
    }
    std::string get_oscillate_string() {
      return (oscillate.get_state()) ? "oscillate_on" : "oscillate_off";
    }
    std::string get_percent_string() {
      return to_string(percent.get_state());
    }

    // Event Methods
    EventCallback on_fan_on = [&](void*) { state.update_state(true); };
    EventCallback on_fan_off = [&](void*) { state.update_state(false); };
    EventCallback on_oscillate_on = [&](void*) { oscillate.update_state(true); };
    EventCallback on_oscillate_off = [&](void*) { oscillate.update_state(false); };
    EventCallback on_percent_change = [&](void* _num) { percent.update_state(*((uint8_t*)_num)); };

    // Update methods
    void send_update_message(uint8_t _qos = 0, bool _retain = false) override {
      state.send_state_message(_qos, _retain);
      oscillate.send_state_message(_qos, _retain);
      percent.send_state_message(_qos, _retain);
    }

    // Loop for the component
    void loop() override {
      state.loop();
      oscillate.loop();
      percent.loop();
    }

    // Data
   public:
    // Fan data
    State<bool> state;
    Command command;
    bool state_optimistic = false;

    // Oscillation data
    State<bool> oscillate;
    Command oscillate_command;

    // Percent data
    State<uint8_t> percent;
    Command percent_command;
  };

}  // namespace HA_MQTT

#endif