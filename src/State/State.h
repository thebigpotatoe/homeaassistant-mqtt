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

#include <functional>
#include <utility>

#include "../Components/Component.h"
#include "ArduinoJson.h"

namespace HA_MQTT {
  template <class T>
  class State {
   public:
    // Typedefs
    typedef std::function<void(T)> StateCallback;
    typedef std::function<std::string(void)> GetStateStringMethod;

    // Constructors
    State(Component* _parent, const char* _endpoint, const char* _topic_name = "state_topic") : parent(_parent),
                                                                                                endpoint(_endpoint),
                                                                                                topic_name(_topic_name) {
    }
    ~State() {}

    // Get Methods
    T get_state() { return *value; }
    T cast_to_type(void* _data) {
      return *((T*)_data);
    }
    GetStateStringMethod get_state_string = nullptr;

    // Set Methods
    void set_state_variable(T* _value) {
      if (_value) value = _value;
    }
    void set_state(T _value) {  // Make sure the state variable is set
      if (value) {
        // Set the state and previous state
        *value = p_value = _value;

        // Logging
        if (get_state_string) HA_LOGGER("[%s][%s] set to %s\n", parent->get_node_id(), topic_name, get_state_string().c_str());

        // Send the state message using the parent
        send_state_message(parent->get_default_qos(), parent->get_default_retain());

        // Call the event callback if it is set
        if (callback) callback(*value);
      }
    }
    void update_state(T _value) {
      // Make sure the state variable is set
      if (value) {
        // Check for a state change or if the forace was passed
        if (*value != _value) {
          // Use set state to update to the new value
          set_state(_value);
        }
      }
    }

    // Config Methods
    void set_state_config(JsonObject& _json) {
      if (topic_name && endpoint)
        _json[topic_name] = parent->create_topic(endpoint);
      else
        HA_LOGGER("[%s][%s] topic or endpoint is invalid for config\n", parent->get_node_id(), topic_name);
    }

    // Send Methods
    void send_state_message(uint8_t _qos = 0, bool _retain = false, const char* custom_payload = nullptr) {
      if (endpoint && get_state_string) {
        if (custom_payload) {
          parent->send_message(endpoint, _qos, _retain, custom_payload);
        } else {
          parent->send_message(endpoint, _qos, _retain, get_state_string().c_str());
        }
      } else if (!get_state_string) {
        HA_LOGGER("[%s][%s] No to string method defined\n", parent->get_node_id(), topic_name);
      }
    }

    // Loop method
    void loop() {
      if (value && *value != p_value) set_state(*value);
    }

    // Events
    void on_state_change(StateCallback _callback) {
      if (_callback)
        callback = _callback;
      else
        HA_LOGGER("[%s][%s] Event callback is invalid\n", parent->get_node_id(), topic_name);
    }

    // Operator Overrides
    operator T() {
      return (*value);
    }
    bool operator()(const T _value) {
      return update_state(_value);
    }
    void operator=(const T _value) {
      update_state(_value);
    }

    // Config data
   protected:
    Component* parent;
    const char* endpoint;
    const char* topic_name;
    T* value = &c_value;
    T c_value;
    T p_value;
    StateCallback callback;
  };  // namespace HA_MQTT
}  // namespace HA_MQTT