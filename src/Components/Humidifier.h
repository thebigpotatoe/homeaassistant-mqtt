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

#ifndef MQTT_Humidifier_H
#define MQTT_Humidifier_H

#define HUMIDIFIER_ON true
#define HUMIDIFIER_OFF false

#include "../Command/Command.h"
#include "../State/State.h"
#include "Component.h"

namespace HA_MQTT {
  class Humidifier : public Component {
   public:
    // Constructor
    Humidifier(const char* _node_id) : Component("humidifier", _node_id),
                                       state(this, "state"),
                                       command(this, "set"),
                                       target(this, "target", "target_humidity_state_topic"),
                                       target_command(this, "target_set", "target_humidity_command_topic") {
      // Setup states
      state.get_state_string = [&]() { return get_fan_state_string(); };
      target.get_state_string = [&]() { return get_percent_string(); };

      // Setup commands
      command.add_valid_input("ON", &on_humidifier_on);
      command.add_valid_input("OFF", &on_humidifier_off);
      target_command.add_valid_input("INTEGER", &on_target_update);
    }
    ~Humidifier() {}

    // Config Methods
    void create_config_message(JsonObject& _json) override {
      // Cover config
      state.set_state_config(_json);
      command.set_command_config(_json);
      _json["optimistic"] = humidifier_optimistic;

      // Percent config variables
      target.set_state_config(_json);
      target_command.set_command_config(_json);
    }

    // State methods
    std::string get_fan_state_string() {
      return (state.get_state()) ? std::string("ON") : std::string("OFF");
    }
    std::string get_percent_string() {
      return to_string(target.get_state());
    }

    // Event Methods
    EventCallback on_humidifier_on = [&](void*) { state.update_state(true); };
    EventCallback on_humidifier_off = [&](void*) { state.update_state(false); };
    EventCallback on_target_update = [&](void* _num) { target.update_state(*((uint8_t*)_num)); };

    // Update methods
    void send_update_message(uint8_t _qos = 0, bool _retain = false) override {
      state.send_state_message(_qos, _retain);
      target.send_state_message(_qos, _retain);
    }

    // Loop Method
    void loop() override {
      target.loop();
    }

   public:
    // Fan data
    State<bool> state;
    Command command;
    bool humidifier_optimistic = false;

    // Percent data
    State<uint8_t> target;
    Command target_command;
  };

}  // namespace HA_MQTT

#endif