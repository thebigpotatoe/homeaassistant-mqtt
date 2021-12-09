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

#ifndef MQTT_SENSOR_H
#define MQTT_SENSOR_H

#include "../Command/Command.h"
#include "../State/State.h"
#include "Component.h"

namespace HA_MQTT {
  class Sensor : public Component {
   public:
    Sensor(const char* _node_id, const char* _unit = nullptr, const char* _class = "measurement", unsigned long _expire_after = 0) : Component("sensor", _node_id),
                                                                                                                                     state(this, "state"),
                                                                                                                                     state_class(_class),
                                                                                                                                     unit(_unit),
                                                                                                                                     expire_after(_expire_after) {
      // Setup the state
      state.get_state_string = [&]() { return get_sensor_state_string(); };
    }
    ~Sensor() {}

    // To create the rest of the config message on startup
    void create_config_message(JsonObject& _json) override {
      // Get the default state config
      state.set_state_config(_json);

      // Sensor specific configs
      if (state_class) _json["state_class"] = state_class;
      if (unit) _json["unit_of_measurement"] = unit;
      if (expire_after) _json["expire_after"] = expire_after;
    }

    // State methods
    std::string get_sensor_state_string() {
      return to_string(state.get_state());
    }

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
    State<float> state;
    const char* state_class = "measurement";
    const char* unit = nullptr;
    unsigned long expire_after = 0;
  };

}  // namespace HA_MQTT

#endif