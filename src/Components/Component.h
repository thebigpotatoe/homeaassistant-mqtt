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

#ifndef HA_COMPONENT_H
#define HA_COMPONENT_H

#include <functional>
#include <list>
#include <utility>

#include "ArduinoJson.h"
#include "ESP8266WiFi.h"
#include "Globals/Declarations.h"

namespace HA_MQTT {
  class Component {
    // Typedefs
   public:
    typedef std::function<void(const char* _endpoint, uint8_t _qos, bool _retain, const char* _message)> SendCallback;
    typedef std::function<std::string(void)> StringMessageCallback;
    typedef std::function<void(JsonObject&)> JsonMessageCallback;
    typedef std::function<void(const char*, size_t)> SubscriptionCallback;
    typedef std::tuple<std::string, uint8_t, SubscriptionCallback*> SubscritionData;
    typedef std::list<SubscritionData> SubscriptionList;
    typedef std::function<void(void*)> EventCallback;

    // Constructors
    Component(const char* _componet_type, const char* _node_id) : componet_type(_componet_type), node_id(_node_id) {
      add_component_globally(this);
    }
    ~Component() {}

    // Data methods
    const char* get_ha_prefix() {
      return ha_prefix;
    }
    const char* get_component_type() {
      return componet_type;
    }
    const char* get_node_id() {
      return node_id;
    }
    const char* get_object_id() {
      return object_id;
    }
    uint8_t get_default_qos() {
      return default_qos;
    }
    bool get_default_retain() {
      return default_retain;
    }
    std::string to_string(uint8_t _num) {
      char buf[1 + 8 * sizeof(unsigned int)];
      int len = sprintf(buf, "%u", _num);
      return (len != -1) ? std::string(buf) : "";
    }
    std::string to_string(float _num, int _decimals) {
      char buf[(1 + 1 + 1 + (_decimals - 1) + 1 + 1 + 4 + 1)];
      int len = snprintf(buf, sizeof buf, "%.*e", _decimals - 1, _num);
      return (len != -1) ? std::string(buf) : "";
    }

    // Topic methods
   public:
    bool set_prefix(const char* _prefix) {
      if (_prefix) {
        HA_LOGGER("[%s] Prefix set to: %s\n", node_id, _prefix);
        ha_prefix = _prefix;
        return true;
      }
      HA_LOGGER("[%s] Failed to set prefix\n", node_id);
      return false;
    }
    bool set_id(const char* _object_id) {
      if (_object_id) {
        HA_LOGGER("[%s] Object ID set to: %s\n", node_id, _object_id);
        object_id = _object_id;
        return true;
      }
      HA_LOGGER("[%s] Failed to set object ID\n", node_id);
      return false;
    }
    std::string create_safe_string(const char* _input) {
      auto filter = [](char c) { return (!std::isalnum(c) + (c == '_')); };
      std::string input = std::string(_input);
      std::replace(input.begin(), input.end(), ':', '_');
      std::replace(input.begin(), input.end(), ' ', '_');
      input.erase(std::remove_if(input.begin(), input.end(), filter), input.end());
      return input;
    }
    std::string create_unique_id() {
      if ((node_id)) {
        std::string safe_node_id = create_safe_string(node_id);
        std::string safe_mac = create_safe_string(WiFi.macAddress().c_str());
        return std::string(safe_node_id + "_" + safe_mac);
      } else {
        return std::string("default");
      }
    }
    std::string create_base_topic() {
      if (ha_prefix && componet_type && node_id && object_id) {
        std::string safe_ha_prefix = create_safe_string(ha_prefix);
        std::string safe_componet_type = std::string(componet_type);
        std::string safe_node_id = create_safe_string(node_id);
        std::string safe_object_id = create_safe_string(object_id);
        return std::string(safe_ha_prefix + "/" + safe_componet_type + "/" + safe_node_id + "/" + safe_object_id);
      } else {
        return std::string("");
      }
    }
    std::string create_topic(const char* _endpoint) {
      if (_endpoint) {
        std::string base_topic = create_base_topic();
        if (base_topic.length()) {
          return std::string(base_topic + "/" + _endpoint);
        }
      }
      return std::string("");
    }

    // Subscription methods
   public:
    SubscriptionList* get_subscriptions() {
      return &subscriptions;
    }
    bool parse_json(const char* _msg, JsonMessageCallback _callback) {
      // Parse the contents of the message
      DynamicJsonDocument json(ESP.getMaxFreeBlockSize() * 0.75);
      DeserializationError error = deserializeJson(json, _msg);
      json.shrinkToFit();
      JsonObject msg = json.as<JsonObject>();

      // Check the error to proceed
      switch (error.code()) {
        case DeserializationError::Ok:
          _callback(msg);
          return true;
          break;
        case DeserializationError::InvalidInput:
          HA_LOGGER("[%s] Failed to parse JSON: Invalid input\n", node_id);
          return false;
          break;
        case DeserializationError::NoMemory:
          HA_LOGGER("[%s] Failed to parse JSON: No Memory\n", node_id);
          return false;
          break;
        default:
          HA_LOGGER("[%s] Failed to parse JSON: Unknown Error\n", node_id);
          return false;
          break;
      }
    }
    void add_subscription(const char* _endpoint, uint8_t _qos, SubscriptionCallback* _callback) {
      if (ha_prefix && componet_type && node_id && object_id && _endpoint && _callback) {
        std::string topic = create_topic(_endpoint);
        if (topic.length()) {
          HA_LOGGER("[%s] Adding subscription for %s\n", node_id, topic.c_str());
          subscriptions.push_back(std::make_tuple(topic, _qos, _callback));
          return;
        }
      }
      HA_LOGGER("[%s] Failed to create subscription for endpoint\n", node_id);
    }

    // Publish methods
   public:
    bool set_send_callback(SendCallback _send_callback) {
      if (_send_callback) {
        send_callback = _send_callback;
        return true;
      }
      HA_LOGGER("[%s] Failed to set send callback\n", node_id);
      return false;
    }
    void send_message(const char* _endpoint, uint8_t _qos, bool _retain, StringMessageCallback _callback) {
      if (_endpoint && _callback) {
        std::string msg = _callback();
        send_message(_endpoint, constrain(_qos, 0, 2), _retain, msg.c_str());
      }
    }
    void send_message(const char* _endpoint, uint8_t _qos, bool _retain, JsonMessageCallback _callback) {
      if (_endpoint && _callback) {
        // Create the json message
        DynamicJsonDocument json(ESP.getMaxFreeBlockSize() * 0.75);
        JsonObject msg_object = json.to<JsonObject>();
        _callback(msg_object);
        json.shrinkToFit();

        // Send the message
        send_message(_endpoint, constrain(_qos, 0, 2), _retain, msg_object);
      }
    }
    void send_message(const char* _endpoint, uint8_t _qos, bool _retain, JsonObject& _message) {
      if (_endpoint) {
        size_t len = measureJson(_message) + 1;
        char buffer[len];
        serializeJson(_message, buffer, len);
        send_message(_endpoint, constrain(_qos, 0, 2), _retain, buffer);
      }
    }
    void send_message(const char* _endpoint, uint8_t _qos, bool _retain, const char* _message) {
      if (send_callback && _endpoint && _message) send_callback(create_topic(_endpoint).c_str(), constrain(_qos, 0, 2), _retain, _message);
    }

    // Config methods
   public:
    void clear_config() {
      send_message("config", 0, false, "");
    }
    void send_config_message(uint8_t _qos = 0, bool _retain = false) {
      send_message("config", constrain(_qos, 0, 2), _retain, std::bind(&Component::setup_config_message, this, std::placeholders::_1));
    }

   protected:
    void setup_config_message(JsonObject& _json) {
      // Mqtt Setup
      // _json["qos"] = default_qos;
      // _json["retain"] = default_retain;

      // Home assistant setup
      if (node_id) _json["name"] = node_id;
      _json["uniq_id"] = create_unique_id();

      // Availability messaging
      _json["avty_t"] = create_topic("available");
      _json["pl_avail"] = "online";
      _json["pl_not_avail"] = "offline";

      // Device info
      if (strcmp(componet_type, "scene") != 0) {
        JsonObject device_info = _json.createNestedObject("device");
        JsonArray device_connections = device_info.createNestedArray("identifiers");
        device_connections.add(std::string(WiFi.macAddress().c_str()));
        device_info["manufacturer"] = "Espressif";
        device_info["model"] = "ESP8266";
      }

      // Component specific setup
      create_config_message(_json);
    }
    virtual void create_config_message(JsonObject& _json) = 0;

    // Availability methods
   public:
    void send_available_message(uint8_t _qos = 0, bool _retain = false) {
      send_message("available", constrain(_qos, 0, 2), _retain, "online");
    }
    void send_unavailable_message(uint8_t _qos = 0, bool _retain = false) {
      send_message("available", constrain(_qos, 0, 2), _retain, "offline");
    }

    // Update Methods
    virtual void send_update_message(uint8_t _qos = 0, bool _retain = false) {}

    // Loop method
   public:
    virtual void loop(){};

    // Topic data
   protected:
    const char* ha_prefix = nullptr;
    const char* componet_type = nullptr;
    const char* node_id = nullptr;
    const char* object_id = nullptr;

    // Subscription data
   protected:
    SubscriptionList subscriptions;

    // Send data
   protected:
    SendCallback send_callback = nullptr;

    // Config data
   protected:
    uint8_t default_qos = 0;
    bool default_retain = false;
    bool enabled_by_default = true;
  };
}  // namespace HA_MQTT

#endif