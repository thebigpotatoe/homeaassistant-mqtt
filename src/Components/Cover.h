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

#ifndef MQTT_COVER_H
#define MQTT_COVER_H

#include <sstream>
#include <string>

#include "../Command/Command.h"
#include "../State/State.h"
#include "Component.h"

namespace HA_MQTT {

  class Cover : public Component {
   public:
    // State enum
    enum class CoverState {
      closed,
      closing,
      open,
      opening,
      stopped
    };

    // Typdefs
    typedef std::function<void(CoverState)> StateCallback;
    typedef std::function<void(uint8_t)> PositionCallback;
    typedef std::function<void(uint8_t)> TiltCallback;

    // Constructor
    Cover(const char* _node_id) : Component("cover", _node_id),
                                  state(this, "state"),
                                  command(this, "set"),
                                  position(this, "pos_state", "position_topic"),
                                  pos_command(this, "pos_set", "set_position_topic"),
                                  tilt(this, "tilt_state", "tilt_status_topic"),
                                  tilt_command(this, "tilt_set", "tilt_command_topic") {
      // Setup states
      state.get_state_string = [&]() { return get_cover_state_string(); };
      position.get_state_string = [&]() { return get_position_string(); };
      tilt.get_state_string = [&]() { return get_tilt_string(); };

      // Setup commands
      command.add_valid_input("OPEN", &on_open);
      command.add_valid_input("CLOSE", &on_close);
      command.add_valid_input("STOP", &on_stop);
      pos_command.add_valid_input("INTEGER", &on_position);
      tilt_command.add_valid_input("INTEGER", &on_tilt);
    }
    ~Cover() {}

    // Control methods
    void enable_state() { enable_state_control = true; }
    void disable_state() { enable_state_control = false; }
    void enable_position() { enable_pos_control = true; }
    void disable_position() { enable_pos_control = false; }
    void enable_tilt() { enable_tilt_control = true; }
    void disable_tilt() { enable_tilt_control = false; }

    // Config Methods
    void create_config_message(JsonObject& _json) override {
      // Cover config
      if (enable_state_control) {
        state.set_state_config(_json);
        command.set_command_config(_json);
        _json["optimistic"] = cover_optimistic;
      }

      // Position config variables
      if (enable_pos_control) {
        position.set_state_config(_json);
        pos_command.set_command_config(_json);
        // _json["position_open"] = position_open;
        // _json["position_closed"] = position_closed;
      }

      // Tilt config variables
      if (enable_tilt_control) {
        tilt.set_state_config(_json);
        tilt_command.set_command_config(_json);
        // _json["tilt_optimistic"] = tilt_optimistic;
        // _json["tilt_min"] = tilt_min;
        // _json["tilt_max"] = tilt_max;
        // _json["tilt_open_value"] = tilt_open_value;
        // _json["tilt_close_value"] = tilt_close_value;
      }
    }

    // State methods
    std::string get_cover_state_string() {
      switch (state.get_state()) {
        case CoverState::closed:
          return "closed";
        case CoverState::closing:
          return "closing";
        case CoverState::open:
          return "open";
        case CoverState::opening:
          return "opening";
        case CoverState::stopped:
          return "stopped";
        default:
          return "";
      }
    }
    std::string get_position_string() {
      return to_string(position.get_state());
    }
    std::string get_tilt_string() {
      return to_string(tilt.get_state());
    }

    // Event Methods
    EventCallback on_open = [&](void*) { state.update_state(CoverState::open); };
    EventCallback on_close = [&](void*) { state.update_state(CoverState::closed); };
    EventCallback on_stop = [&](void*) { state.update_state(CoverState::stopped); };
    EventCallback on_position = [&](void* _num) { position.set_state(position.cast_to_type(_num)); };
    EventCallback on_tilt = [&](void* _num) { tilt.update_state(tilt.cast_to_type(_num)); };

    // Update methods
    void send_update_message(uint8_t _qos = 0, bool _retain = false) override {
      state.send_state_message(_qos, _retain);
      position.send_state_message(_qos, _retain);
      tilt.send_state_message(_qos, _retain);
    }

    // Loop for the component
    void loop() override {
      state.loop();
      position.loop();
      tilt.loop();
    }

    // Data
   public:
    // Cover data
    bool enable_state_control = true;
    State<CoverState> state;
    Command command;
    bool cover_optimistic = false;

    // Position data
    bool enable_pos_control = true;
    State<uint8_t> position;
    Command pos_command;
    uint8_t position_open = 100;
    uint8_t position_closed = 0;

    // tilt data
    bool enable_tilt_control = true;
    State<uint8_t> tilt;
    Command tilt_command;
    bool tilt_optimistic = false;
    uint8_t tilt_min = 0;
    uint8_t tilt_max = 100;
    uint8_t tilt_open_value = 0;
    uint8_t tilt_close_value = 100;
  };

}  // namespace HA_MQTT

#endif