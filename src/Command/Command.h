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

#ifndef HA_COMMAND_H
#define HA_COMMAND_H

#include <functional>
#include <utility>

#include "../Components/Component.h"
#include "ArduinoJson.h"

namespace HA_MQTT {
  class Command {
   public:
    // Typedefs
    typedef std::function<void(const char*, size_t)> CommandParseMethod;
    typedef std::function<void(void*)> EventCallback;
    typedef std::list<std::pair<const char*, EventCallback*>> EventCallbackList;

    // Constructors
    Command(Component* _parent, const char* _endpoint, const char* _topic_name = "command_topic") : parent(_parent),
                                                                                                    endpoint(_endpoint),
                                                                                                    topic_name(_topic_name) {
      // Setup the parse command
      parse_command = [&](const char* _msg, size_t _len) {
        if (_msg && _len > 0) {
          for (auto event : event_list) {
            if (event.first && event.second) {
              // Parse an integer if required
              if (strncmp(event.first, "INTEGER", 7) == 0) {
                char buffer[_len];
                memcpy(buffer, _msg, _len);
                uint8_t buffer_num = atoi(buffer);
                (*(event.second))((void*)&buffer_num);
              }

              // Else parse a float if required
              else if (strncmp(event.first, "FLOAT", 5) == 0) {
                char buffer[_len];
                memcpy(buffer, _msg, _len);
                float buffer_num = atof(buffer);
                (*(event.second))((void*)&buffer_num);
              }

              // Else pass the message object
              else if (strncmp(event.first, _msg, _len) == 0) {
                (*(event.second))((void*)_msg);
              }
            }
          }
        }
      };

      // Add subscription to parent
      if (parent && endpoint && topic_name)
        parent->add_subscription(endpoint, 0, &parse_command);
    };
    ~Command() {}

    // Valid inputs methods
    void add_valid_input(const char* _input, EventCallback* _callback) {
      if (_input && _callback) event_list.push_back(std::make_pair(_input, _callback));
    }

    // Parsing Method
    CommandParseMethod parse_command = nullptr;

    // Config Methods
    void set_command_config(JsonObject& _json) {
      if (topic_name && endpoint)
        _json[topic_name] = parent->create_topic(endpoint);
      else
        HA_LOGGER("[%s][%s] topic or endpoint is invalid for config\n", parent->get_node_id(), topic_name);
    }

    // Config data
   public:
    EventCallbackList event_list;

   protected:
    Component* parent;
    const char* endpoint = nullptr;
    const char* topic_name = "command_topic";
  };
}  // namespace HA_MQTT

#endif