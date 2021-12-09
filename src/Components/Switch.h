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

#ifndef MQTT_SWITCH_H
#define MQTT_SWITCH_H

#define SWITCH_ON true
#define SWITCH_OFF false

#include "../Command/Command.h"
#include "../State/State.h"
#include "Component.h"

namespace HA_MQTT {
  class Switch : public Component {
   public:
    // Constructors
    Switch(const char* _node_id) : Component("switch", _node_id),
                                   state(this, "state"),
                                   command(this, "set") {
      // Setup the state
      state.get_state_string = [&]() { return (state.get_state()) ? std::string("ON") : std::string("OFF"); };

      // Setup command
      command.add_valid_input("ON", &on_switch_on);
      command.add_valid_input("OFF", &on_switch_off);
    }
    ~Switch() {}

    // To create the rest of the config message on startup
    void create_config_message(JsonObject& _json) override {
      // Get the default state config
      state.set_state_config(_json);

      // Get the default command config
      command.set_command_config(_json);
    }

    // Event Methods
    EventCallback on_switch_on = [&](void*) { state.update_state(true); };
    EventCallback on_switch_off = [&](void*) { state.update_state(false); };

    // Update methods
    void send_update_message(uint8_t _qos = 0, bool _retain = false) override {
      state.send_state_message(_qos, _retain);
    }

    // Loop for the component
    void loop() override {
      state.loop();
    }

    // Data
   public:
    State<bool> state;
    Command command;
  };
}  // namespace HA_MQTT

#endif