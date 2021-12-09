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

#ifndef HA_MQTT_CLIENT_H
#define HA_MQTT_CLIENT_H

// Macro for prefix and device name
#ifndef HA_MQTT_PREFIX
#define HA_MQTT_PREFIX nullptr
#endif
#ifndef HA_MQTT_DEVICE_NAME
#define HA_MQTT_DEVICE_NAME nullptr
#endif
#ifndef HA_MQTT_RESET_ALL_COMPONENTS
#define HA_MQTT_RESET_ALL_COMPONENTS false
#endif

namespace HA_MQTT {
  class Client {
   public:
    // Typedefs
    typedef std::list<Component*> ComponentList;
    typedef std::function<void(void)> ScheduleCallback;
    typedef std::function<void(Client*)> WaitForCallback;

   public:
    // Constructors
    Client() {
      // Init the specifc parts of the class
      init();

      // Set the prefix if needed
      if (!prefix) set_prefix("homeassistant");

      // Set the device id to the mac address if needed
      if (!device_id) {
        std::string* buffer_id = new std::string(WiFi.macAddress().c_str());
        buffer_id->erase(std::remove(buffer_id->begin(), buffer_id->end(), ':'), buffer_id->end());
        set_device_id(buffer_id->c_str());
      }

#if defined(HA_MQTT_HOST_NAME) && defined(HA_MQTT_PORT)
      // Set the host and port from macros
      set_broker(HA_MQTT_HOST_NAME, HA_MQTT_PORT);
#endif

      // Schedule the loop
      loop();
    }
    ~Client() {}

   protected:
    // Logic methods
    void init();
    bool schedule(ScheduleCallback _callback);
    void wait_for(unsigned long _millis, WaitForCallback _callback);
    void loop() {
      // Iterate over each components loop method
      for (auto component : components) {
        component->loop();
      }

      // schedule the next loop
      if (!schedule([&]() { loop(); })) {
        wait_for(500, [&](Client* _this) { _this->loop(); });
      }
    }

   protected:
    // Wifi methods
    void onWiFiConnected() {
      HA_LOGGER("[HA MQTT Client] Broker sensed wifi has been connected\n");
      connect();
    }
    void onWiFiDisconnected() {
      HA_LOGGER("[HA MQTT Client] Broker sensed wifi has been disconnected\n");
      disconnect();
    }

   public:
    // Mqtt client methods
    std::string create_safe_string(const char* _input) {
      std::string input = std::string(_input);

      // Change any space to an underscore
      // std::replace(input.begin(), input.end(), ' ', '_');

      // Change any invalid char to nothing
      input.erase(std::remove_if(input.begin(), input.end(), (int (*)(int))std::isalnum), input.end());

      // Return the cleaned input
      return input;
    }
    bool set_broker(const char* _hostname, uint32_t _port) {
      if (_hostname) hostname = _hostname;
      if (_port) port = _port;
      HA_LOGGER("[HA MQTT Client] Broker set to: %s:%d\n", hostname, port);
      return true;
    }
    bool connect();
    bool disconnect();
    bool reconnect();

   protected:
    // MQTT event methods
    void on_connect() {
      HA_LOGGER("[HA MQTT Client] Connected to broker at %s:%d\n", hostname, port);
      if (clear_all || HA_MQTT_RESET_ALL_COMPONENTS) reset_all();
      send_all_configs();
      subscribe_to_all();
      wait_for(1000, [](Client* _this) {
        _this->send_all_available();
        _this->send_all_updates();
      });
    }
    void on_disconnect() {
      HA_LOGGER("[HA MQTT Client] Disconnected from broker at %s:%d\n", hostname, port);
    }
    void on_message(char* _topic, char* _payload, size_t _len) {
      if (_topic && _payload) {
        for (auto component : components) {
          Component::SubscriptionList* sub_list = component->get_subscriptions();
          for (auto subscription : *sub_list) {
            if (strcmp(_topic, std::get<0>(subscription).c_str()) == 0) {
              auto callback = std::get<2>(subscription);
              if (callback) (*callback)(_payload, _len);
            }
          }
        }
      }
    }
    void subscribe(const char* _topic, uint8_t _qos = 0);
    void publish(const char* _topic, uint8_t _qos, bool retain, const char* _msg);

   public:
    // Home assistant data methods
    const char* get_prefix() {
      return prefix;
    }
    const char* get_device_id() {
      return device_id;
    }
    void set_prefix(const char* _prefix) {
      if (_prefix) {
        HA_LOGGER("[HA MQTT Client] Setting prefix to: %s\n", _prefix);
        prefix = _prefix;
        for (auto component : components) {
          component->set_prefix(prefix);
        }
      } else {
        HA_LOGGER("[HA MQTT Client] Failed to set prefix\n");
      }
    }
    void set_device_id(const char* _id) {
      if (_id) {
        HA_LOGGER("[HA MQTT Client] Setting device id to: %s\n", _id);
        device_id = _id;
        for (auto component : components) {
          component->set_id(device_id);
        }
      } else {
        HA_LOGGER("[HA MQTT Client] Failed to set device id\n");
      }
    }

   public:
    // Component methods
    bool add_component(Component* _component) {
      if (_component) {
        HA_LOGGER("[HA MQTT Client] Adding component: %s\n", _component->get_node_id());
        _component->set_prefix(prefix);
        _component->set_id(device_id);
        _component->set_send_callback(std::bind(&Client::send_message, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
        components.push_back(_component);
        return true;
      }
      HA_LOGGER("[HA MQTT Client] Failed to add component");

      return false;
    }
    bool remove_component(Component* _component) {
      if (_component) {
        HA_LOGGER("[HA MQTT Client] Removing component: %s\n", _component->get_node_id());
        components.remove(_component);
        // unsubscribe from all
        return true;
      }
      HA_LOGGER("[HA MQTT Client] Failed to remove component\n");
      return false;
    }
    void reset_all_components() {
      clear_all = true;
    }
    bool reset_all() {
      clear_all = false;
      for (auto component : components) {
        HA_LOGGER("[HA MQTT Client] Clearing config for: %s\n", component->get_node_id());
        component->clear_config();
      }
      return true;
    }
    bool subscribe_to_all() {
      for (auto component : components) {
        auto subscriptions = component->get_subscriptions();
        if (subscriptions->size() > 0) {
          for (auto subscription : *subscriptions) {
            const char* topic = std::get<0>(subscription).c_str();
            uint8_t qos = std::get<1>(subscription);
            if (topic) {
              HA_LOGGER("[HA MQTT Client] Subscribing to: %s\n", topic);
              subscribe(topic, qos);
            } else {
              HA_LOGGER("[HA MQTT Client] Failed to subscribe to topic\n");
            }
          }
        } else {
          HA_LOGGER("[HA MQTT Client] %s has no subscriptions\n", component->get_node_id());
        }
      }

      return true;
    }
    bool send_all_configs() {
      for (auto component : components) {
        HA_LOGGER("[HA MQTT Client] Sending config for: %s\n", component->get_node_id());
        component->send_config_message();
      }
      return true;
    }
    bool send_all_available() {
      for (auto component : components) {
        HA_LOGGER("[HA MQTT Client] Sending availability message for: %s\n", component->get_node_id());
        component->send_available_message();
      }
      return true;
    }
    bool send_all_updates() {
      for (auto component : components) {
        HA_LOGGER("[HA MQTT Client] Sending update for: %s\n", component->get_node_id());
        component->send_update_message();
      }
      return true;
    }
    void send_message(const char* _topic, uint8_t _qos, bool _retain, const char* _message) {
      if (_topic && _message) {
        // HA_LOGGER("Sending message:\n\ttopic: %s\n\tmessage: %s\n", _topic, _message);
        publish(_topic, _qos, _retain, _message);
      }
    }

   protected:
    // Mqtt client data
    const char* hostname;
    uint32_t port = 0;

    // Home assistant data
    bool clear_all = false;
    const char* prefix = HA_MQTT_PREFIX;
    const char* device_id = HA_MQTT_DEVICE_NAME;

    // Componet data
    ComponentList components;
  };

}  // namespace HA_MQTT

#if defined(ESP8266)

#include <Ticker.h>

#include "AsyncMqttClient.h"
#include "Client.h"
#include "ESP8266WiFi.h"

namespace HA_MQTT {
  // Logic data
  Ticker loop_watchdog;
  Ticker event_timer;

  // WiFi data
  WiFiEventHandler stationConnectedHandler;
  WiFiEventHandler stationDisconnectedHandler;

  // MQTT Client
  AsyncMqttClient client;

  void Client::init() {
    // Set event handlers for wifi connection
    stationConnectedHandler = WiFi.onStationModeGotIP([&](const WiFiEventStationModeGotIP&) { onWiFiConnected(); });
    stationDisconnectedHandler = WiFi.onStationModeDisconnected([&](const WiFiEventStationModeDisconnected&) { onWiFiDisconnected(); });

    // Setup the MQTT client
    client.onConnect(std::bind(&Client::on_connect, this));
    client.onDisconnect([&](AsyncMqttClientDisconnectReason) { on_disconnect(); });
    client.onMessage([&](char* _topic, char* _payload, AsyncMqttClientMessageProperties, size_t _len, size_t, size_t) { on_message(_topic, _payload, _len); });
  }
  bool Client::schedule(ScheduleCallback _callback) {
    return schedule_function(_callback);
  }
  void Client::wait_for(unsigned long _millis, WaitForCallback _callback) {
    event_timer.once_ms<Client*>(
        1000, [&](Client* _this) {
          _this->send_all_available();
          _this->send_all_updates();
        },
        this);
  }

  bool Client::connect() {
    if (hostname && port > 0) {
      // Debug
      HA_LOGGER("[HA MQTT Client] Connecting to broker at %s:%d\n", hostname, port);

      //   Get the hostname
      IPAddress hostIp;
      if (hostIp.fromString(hostname)) {
        client.setServer(hostIp, port);
      } else {
        client.setServer(hostname, port);
      }

      // Connect to the broker
      client.connect();

      // return true
      return true;
    }
    HA_LOGGER("[HA MQTT Client] Failed to start broker connection since hostname or port not set\n");
    return false;
  }
  bool Client::disconnect() {
    HA_LOGGER("[HA MQTT Client] Disconnecting from broker at %s:%d\n", hostname, port);
    client.disconnect();
    return true;
  }
  bool Client::reconnect() {
    return disconnect() & connect();
  }

  void Client::subscribe(const char* _topic, uint8_t _qos) {
    if (_topic) client.subscribe(_topic, _qos);
  }
  void Client::publish(const char* _topic, uint8_t _qos, bool _retain, const char* _message) {
    client.publish(_topic, _qos, _retain, _message);
  }
}  // namespace HA_MQTT

#endif

#if defined(ESP32)

#include <WiFi.h>

#include "AsyncMqttClient.h"

namespace HA_MQTT {
  typedef std::function<void(void)> esp32Event;
  typedef std::list<esp32Event> esp32EventStack;
  esp32EventStack aether_events_stack;
  TaskHandle_t aetherTaskHandle = NULL;

  void loop_init() {
    // Start a new thread on the second core of the esp
    xTaskCreatePinnedToCore(aetherEsp32Loop, "aetherloop", 10240, NULL, AETHER_ESP32_LOOP_PRIORITY, &aetherTaskHandle, CONFIG_ARDUINO_RUNNING_CORE);
    esp_task_wdt_add(aetherTaskHandle);
  }
  loop_init();

  void aetherEsp32Loop(void* _null) {
    for (;;) {
      if (esp32ArduinoStarted && aether_events_stack.size()) {
        // HA_LOGGER("executing %d events\n", aether_events_stack.size());

        // Copy the current stack into a temp list
        esp32EventStack currentStack{std::move(aether_events_stack)};

        // Clear the old list
        aether_events_stack.clear();

        // Iterate the temp list which allows additions to the stack to execute next run
        for (auto event : currentStack) {
          event();
        }

        // HA_LOGGER("Done executing events\n");
      } else if (!esp32ArduinoStarted) {
        // Wait for the main Arduino loop of the ESP32 core to start before starting the Aether Loop
        esp32ArduinoStarted = (loopTaskHandle != NULL) ? true : false;
      }

      // Trigger watchdog
      esp_task_wdt_reset();
    }
  }
  bool esp32_schedule_function(esp32Event _event) {
    if (_event) {
      aether_events_stack.push_back(_event);
    }
    return true;
  }

  void Client::init() {
    // Start a new thread on the second core of the esp
    xTaskCreatePinnedToCore(aetherEsp32Loop, "aetherloop", 10240, NULL, AETHER_ESP32_LOOP_PRIORITY, &aetherTaskHandle, CONFIG_ARDUINO_RUNNING_CORE);
    esp_task_wdt_add(aetherTaskHandle);

    // Set event handlers for wifi connection
    WiFi.onEvent([=](WiFiEvent_t event, WiFiEventInfo_t info) { onWiFiConnected(); }, WiFiEvent_t::SYSTEM_EVENT_STA_GOT_IP);
    WiFi.onEvent([=](WiFiEvent_t event, WiFiEventInfo_t info) { onWiFiDisconnected(); }, WiFiEvent_t::SYSTEM_EVENT_STA_DISCONNECTED);

    // Setup the MQTT client
    client.onConnect(std::bind(&Client::on_connect, this));
    client.onDisconnect(std::bind(&Client::on_disconnect, this, std::placeholders::_1));
    client.onMessage([&](char* _topic, char* _payload, AsyncMqttClientMessageProperties, size_t _len, size_t, size_t) { on_message(_topic, _payload, _len); });
  }
  void Client::schedule(LogicCallback _callback) {
    if (!HA_MQTT_SCHEDULE_FUNCTION(_callback)) {
      HA_LOGGER("[HA MQTT Client] Failed to schedule loop, trying again...\n");
      // loop_watchdog.once_ms<Client*>(
      //     500, [](Client* _this) { _callback }, this);
    }
  }
  void Client::wait_for(unsigned long _millis, LogicCallback _callback) {
    // event_timer.once_ms<Client*>(
    //     1000, [&](Client* _this) {
    //       _this->send_all_available();
    //       _this->send_all_updates();
    //     },
    //     this);
  }

  bool Client::connect() {
    if (hostname && port > 0) {
      // Debug
      HA_LOGGER("[HA MQTT Client] Connecting to broker at %s:%d\n", hostname, port);

      //   Get the hostname
      IPAddress hostIp;
      if (hostIp.fromString(hostname)) {
        client.setServer(hostIp, port);
      } else {
        client.setServer(hostname, port);
      }

      // Connect to the broker
      client.connect();

      // return true
      return true;
    }
    HA_LOGGER("[HA MQTT Client] Failed to start broker connection since hostname or port not set\n");
    return false;
  }
  bool Client::disconnect() {
    HA_LOGGER("[HA MQTT Client] Disconnecting from broker at %s:%d\n", hostname, port);
    client.disconnect();
    return true;
  }
  bool Client::reconnect() {
    return disconnect() & connect();
  }

  void Client::subscribe(const char* _topic, uint8_t _qos = 0) {
    if (_topic) client.subscribe(_topic, _qos);
  }
  void Client::publish(const char* _topic, uint8_t _qos, bool _retain, const char* _message) {
    client.publish(_topic, _qos, _retain, _message);
  }
}  // namespace HA_MQTT

#endif

#endif