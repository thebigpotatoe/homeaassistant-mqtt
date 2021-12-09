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

#ifndef MQTT_NUMBER_H
#define MQTT_NUMBER_H

#include "../Command/Command.h"
#include "../State/State.h"
#include "Component.h"

namespace HA_MQTT {
  class Number : public Component {
   public:
    // Constructors
    Number(const char* _node_id, float _step = 1, float _min_number = 0, float _max_number = 0) : Component("number", _node_id),
                                                                                                  state(this, "state"),
                                                                                                  command(this, "set"),
                                                                                                  min_number(_min_number),
                                                                                                  max_number(_max_number),
                                                                                                  step(_step) {
      // Setup the state
      state.get_state_string = [&]() { return get_number_state_string(); };

      // Setup the command
      command.add_valid_input("FLOAT", &on_number_changed);
    }
    ~Number() {}

    // To create the rest of the config message on startup
    void create_config_message(JsonObject& _json) override {
      // Get the default state config
      state.set_state_config(_json);

      // Get the default command config
      command.set_command_config(_json);

      // Numebr specific configs
      if (min_number != 0 && max_number != 0) {
        _json["min"] = min_number;
        _json["max"] = max_number;
      }
      _json["step"] = step;
    }

    // State methods
    std::string get_number_state_string() {
      return to_string(state.get_state());
    }

    // Event Methods
    EventCallback on_number_changed = [&](void* _num) { state.update_state(*((float*)_num)); };

    // Update methods
    void send_update_message(uint8_t _qos = 0, bool _retain = false) override {
      state.send_state_message(_qos, _retain);
    }

    // Loop for the component
    void loop() override {
      state.loop();
    }

   public:
    // Data
    State<float> state;
    Command command;

   protected:
    // Private data
    float min_number = 0;
    float max_number = 0;
    float step = 1;
  };

}  // namespace HA_MQTT

#endif