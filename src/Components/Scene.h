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

#ifndef MQTT_SCENE_H
#define MQTT_SCENE_H

#include "../Command/Command.h"
#include "../State/State.h"
#include "Component.h"

namespace HA_MQTT {

  class Scene : public Component {
   public:
    // Constructors
    Scene(const char* _node_id) : Component("scene", _node_id),
                                  command(this, "set") {
      // Setup the command
      command.add_valid_input("ON", &on_input);
    }
    ~Scene() {}

    // To create the rest of the config message on startup
    void create_config_message(JsonObject& _json) override {
      // Get the default command config
      command.set_command_config(_json);
      _json["payload_on"] = "ON";
    }

    // Event Methods
    EventCallback on_input = [&](void*) {};

   public:
    // Data
    Command command;
  };

}  // namespace HA_MQTT

#endif