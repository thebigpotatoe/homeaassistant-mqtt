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

#ifndef MQTT_SELECT_H
#define MQTT_SELECT_H

#include "../Command/Command.h"
#include "../State/State.h"
#include "Component.h"

namespace HA_MQTT {
  class Select : public Component {
   public:
    // Typedefs
    typedef std::list<std::pair<const char*, EventCallback>> OptionsList;

    // Constructors
    Select(const char* _node_id) : Component("select", _node_id),
                                   state(this, "state"),
                                   command(this, "set") {
      // Setup the state
      state.get_state_string = [&]() { return get_select_state_string(); };

      // Setup the command
      command.parse_command = [&](const char* _msg, size_t _len) { parse_select_command_message(_msg, _len); };
    }
    ~Select() {}

    // To create the rest of the config message on startup
    void create_config_message(JsonObject& _json) override {
      // Get the default state config
      state.set_state_config(_json);

      // Get the default command config
      command.set_command_config(_json);

      // Options specific config
      JsonArray options_array = _json.createNestedArray("options");
      for (auto option : options) {
        if (option.first) options_array.add(option.first);
      }
    }

    // State methods
    std::string get_select_state_string() {
      return (state.get_state()) ? std::string(state.get_state()) : "";
    }

    // Command methods
    void parse_select_command_message(const char* _msg, size_t _len) {
      if (_msg) {
        for (auto option : options) {
          if (option.first && option.second && strncmp(_msg, option.first, _len) == 0) {
            (option.second)((void*)option.first);
          }
        }
      }
    }

    // Select methods
    void add_option(const char* _option, EventCallback _callback) {
      if (_option && _callback) options.push_back(std::make_pair(_option, _callback));
    }

    // Update methods
    void send_update_message(uint8_t _qos = 0, bool _retain = false) override {
      // state.send_state_message(_qos, _retain);
    }

    // Loop for the component
    void loop() override {
      state.loop();
    }

   public:
    // Data
    State<const char*> state;
    Command command;
    OptionsList options;
  };

}  // namespace HA_MQTT

#endif