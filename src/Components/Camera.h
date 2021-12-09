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

#ifndef MQTT_CAMERA_H
#define MQTT_CAMERA_H

#include "../Command/Command.h"
#include "../State/State.h"
#include "Component.h"

namespace HA_MQTT {
  class Camera : public Component {
   public:
    typedef std::function<void(bool)> StateCallback;

    // Constructor
    Camera(const char* _node_id) : Component("camera", _node_id),
                                   state(this, "state") {
      state.get_state_string = [&]() { return (""); };
    }
    ~Camera() {}

    // Config Methods
    void create_config_message(JsonObject& _json) override {
      // State config
      state.set_state_config(_json);
    }

    // Update methods
    void send_update_message(uint8_t _qos = 0, bool _retain = false) override {
      state.send_state_message(_qos, _retain);
    }

    // Loop Method
    void loop() override {
      state.loop();
    }

   public:
    State<uint8_t*> state;
  };
}  // namespace HA_MQTT

#endif