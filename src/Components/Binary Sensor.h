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

#ifndef HA_BINARY_SENSOR_H
#define HA_BINARY_SENSOR_H

#include "../Command/Command.h"
#include "../State/State.h"
#include "Component.h"

namespace HA_MQTT {
  class BinarySensor : public Component {
   public:
    //  Constructors
    BinarySensor(const char* _node_id, const char* _device_class, unsigned long _expire_after = 0) : Component("binary_sensor", _node_id),
                                                                                                     state(this, "state"),
                                                                                                     device_class(_device_class),
                                                                                                     expire_after(_expire_after) {
      state.get_state_string = [&]() { return (state.get_state()) ? std::string("ON") : std::string("OFF"); };
    }
    ~BinarySensor() {}

    // Config methods
    void create_config_message(JsonObject& _json) override {
      // State config
      state.set_state_config(_json);

      // Binary sensor specific config
      if (device_class) _json["device_class"] = device_class;
      if (expire_after) _json["expire_after "] = expire_after;
    }

    // Update methods
    void send_update_message(uint8_t _qos = 0, bool _retain = false) override {
      state.send_state_message(_qos, _retain);
    }

    // Loop method
    void loop() override {
      state.loop();
    }

    // Data
   public:
    State<bool> state;

   protected:
    const char* device_class = nullptr;
    unsigned long expire_after = 0;
  };

}  // namespace HA_MQTT

#endif