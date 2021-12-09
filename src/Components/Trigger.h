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

#ifndef MQTT_TRIGGER_H
#define MQTT_TRIGGER_H

#include "../Command/Command.h"
#include "../State/State.h"
#include "Component.h"

namespace HA_MQTT {

  class Trigger : public Component {
   public:
    enum class TriggerType {
      button_short_press,
      button_short_release,
      button_long_press,
      button_long_release,
      button_double_press,
      button_triple_press,
      button_quadruple_press,
      button_quintuple_press
    };
    enum class TriggerSubType {
      turn_on,
      turn_off,
      button_1,
      button_2,
      button_3,
      button_4,
      button_5,
      button_6
    };

    Trigger(const char* _node_id, TriggerType _trigger_type, TriggerSubType _trigger_subtype) : Component("device_automation", _node_id),
                                                                                                trigger_type(_trigger_type),
                                                                                                trigger_subtype(_trigger_subtype) {}
    ~Trigger() {}

    // To create the rest of the config message on startup
    void create_config_message(JsonObject& _json) override {
      // State config
      _json["topic"] = create_topic("action").c_str();

      // Trigger specific config
      _json["type"] = trigger_type_to_string(trigger_type).c_str();
      _json["subtype"] = trigger_sub_type_to_string(trigger_subtype).c_str();
      _json["payload"] = trigger_sub_type_to_string(trigger_subtype).c_str();
    }

    // Trigger methods
    std::string trigger_type_to_string(TriggerType _trigger_type) {
      switch (_trigger_type) {
        case TriggerType::button_short_press:
          return "button_short_press";
        case TriggerType::button_short_release:
          return "button_short_release";
        case TriggerType::button_long_press:
          return "button_long_press";
        case TriggerType::button_long_release:
          return "button_long_release";
        case TriggerType::button_double_press:
          return "button_double_press";
        case TriggerType::button_triple_press:
          return "button_triple_press";
        case TriggerType::button_quadruple_press:
          return "button_quadruple_press";
        case TriggerType::button_quintuple_press:
          return "button_quintuple_press";
        default:
          return "";
      }
    }
    std::string trigger_sub_type_to_string(TriggerSubType _trigger_sub_type) {
      switch (_trigger_sub_type) {
        case TriggerSubType::turn_on:
          return "turn_on";
        case TriggerSubType::turn_off:
          return "turn_off";
        case TriggerSubType::button_1:
          return "button_1";
        case TriggerSubType::button_2:
          return "button_2";
        case TriggerSubType::button_3:
          return "button_3";
        case TriggerSubType::button_4:
          return "button_4";
        case TriggerSubType::button_5:
          return "button_5";
        case TriggerSubType::button_6:
          return "button_6";
        default:
          return "";
      }
    }
    void trigger() {
      send_message("action", default_qos, default_retain, trigger_sub_type_to_string(trigger_subtype).c_str());
    }

    // Data
   protected:
    std::string trigger_topic;
    TriggerType trigger_type;
    TriggerSubType trigger_subtype;
  };

}  // namespace HA_MQTT

#endif