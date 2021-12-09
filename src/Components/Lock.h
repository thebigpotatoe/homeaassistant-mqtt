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

#ifndef MQTT_LOCK_H
#define MQTT_LOCK_H

#include "../Command/Command.h"
#include "../State/State.h"
#include "Component.h"

namespace HA_MQTT {
  class Lock : public Component {
   public:
    // Enum states
    enum class LockState {
      locked,
      locking,
      unlocked,
      unlocking,
    };

    // Constructors
    Lock(const char* _node_id) : Component("lock", _node_id),
                                 state(this, "state"),
                                 command(this, "set") {
      // Setup the state
      state.get_state_string = [&]() { return get_lock_state_string(); };

      // Setup the command
      command.add_valid_input("LOCK", &on_lock);
      command.add_valid_input("UNLOCK", &on_unlock);
    }
    ~Lock() {}

    // To create the rest of the config message on startup
    void create_config_message(JsonObject& _json) override {
      // Get the default state config
      state.set_state_config(_json);

      // Get the default command config
      command.set_command_config(_json);
    }

    // State methods
    std::string get_lock_state_string() {
      switch (state.get_state()) {
        case LockState::locked:
          return "LOCKED";
        case LockState::locking:
          return "LOCKING";
        case LockState::unlocked:
          return "UNLOCKED";
        case LockState::unlocking:
          return "UNLOCKING";
        default:
          return "UNKNOWN";
      }
    }

    // Event Methods
    EventCallback on_lock = [&](void*) { state.update_state(LockState::locking); };
    EventCallback on_unlock = [&](void*) { state.update_state(LockState::unlocking); };

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
    State<LockState> state;
    Command command;
  };

}  // namespace HA_MQTT

#endif