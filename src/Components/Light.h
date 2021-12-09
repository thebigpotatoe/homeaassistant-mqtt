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

#ifndef MQTT_LIGHT_H
#define MQTT_LIGHT_H

#include "../Command/Command.h"
#include "../State/State.h"
#include "Component.h"

namespace HA_MQTT {
  class Light : public Component {
   public:
    // Enums
    enum class ColourMode {
      onoff,
      brightness,
      color_temp,
      hs,
      xy,
      rgb,
      rgbw,
      rgbww
    };

    // Structs
    struct Colour {
      int temp = -1;
      int r = -1;
      int g = -1;
      int b = -1;
      int c = -1;
      int w = -1;
      float x = -1;
      float y = -1;
      float h = -1;
      float s = -1;
    };
    // Typedefs
    typedef std::list<const char*> EffectsList;

    // Constructors
    Light(const char* _node_id) : Component("light", _node_id),
                                  state_state(this, "state"),
                                  state_command(this, "set") {
      // State config
      state_state.get_state_string = [&]() { return get_state_string(); };

      // Command config
      state_command.parse_command = [&](const char* _msg, size_t _len) { parse_json(_msg, [&](JsonObject& _json) { parse_state_command_json(_json); }); };
    }
    ~Light(){};

    // Config methods
    void create_config_message(JsonObject& _json) override {
      // Set the schema to JSON by default
      _json["schema"] = "json";

      // Use the outgoing and incoming state and command variables
      state_state.set_state_config(_json);
      state_command.set_command_config(_json);
      _json["optimistic"] = false;

      // Enable the brightness if used
      if (brightness_enabled) {
        _json["brightness"] = true;
        _json["brightness_scale"] = brightness_scale;
      }

      // Enable the colour mode if used
      if (colour_enabled) {
        _json["color_mode"] = true;
        JsonArray colour_mode = _json.createNestedArray("supported_color_modes");
        colour_mode[0] = get_colour_mode_string().c_str();
      }

      // Enable the effects if used
      if (effects_enabled) {
        _json["effect"] = true;
        JsonArray effect_list = _json.createNestedArray("effect_list");
        for (auto effect : effects) {
          effect_list.add(effect);
        }
      }

      // Set the flash lengths
      _json["min_mireds"] = min_mireds;
      _json["max_mireds"] = max_mireds;
    }

    // State methods
    std::string get_state_string() {
      return (state_state.get_state()) ? "{\"state\":\"ON\"}" : "{\"state\":\"OFF\"}";
    }

    // Command methods
    void parse_state_command_json(JsonObject& _json) {
      serializeJson(_json, Serial);
      Serial.printf("\n");
      if (!_json["state"].isNull()) {
        if (strcmp(_json["state"].as<const char*>(), "ON") == 0) {
          state_state.update_state(true);
        } else if (strcmp(_json["state"].as<const char*>(), "OFF") == 0) {
          state_state.update_state(false);
        }
      }
    }

    // Control methods
    void enable_brightness(uint8_t _brightness_scale = 255) {
      brightness_enabled = true;
      brightness_scale = _brightness_scale;
    }
    void enable_colour(ColourMode _mode) {
      colour_enabled = true;
      colour_mode = _mode;
    }
    void enable_effects(EffectsList _effects) {
      effects_enabled = true;
      effects = _effects;
    }
    void set_colour_range(uint8_t _min_mireds = 10, uint8_t _max_mireds = 2) {
      max_mireds = _max_mireds;
      min_mireds = _min_mireds;
    }
    void set_flash_times(uint8_t _flash_short_time = 2, uint8_t _flash_long_time = 10) {
      flash_long_time = _flash_long_time;
      flash_short_time = _flash_short_time;
    }
    std::string get_colour_mode_string() {
      switch (colour_mode) {
        case ColourMode::onoff:
          return "onoff";
        case ColourMode::brightness:
          return "brightness";
        case ColourMode::color_temp:
          return "color_temp";
        case ColourMode::hs:
          return "hs";
        case ColourMode::xy:
          return "xy";
        case ColourMode::rgb:
          return "rgb";
        case ColourMode::rgbw:
          return "rgbw";
        case ColourMode::rgbww:
          return "rgbww";
        default:
          return "unknown";
      }
    }

    // Send methods
    void get_json_message(JsonObject& _json) {
      // State
      _json["transition"] = transition;

      // Brightness
      if (brightness_enabled) {
        _json["brightness"] = brightness;
      }

      // Colour
      if (colour_enabled) {
        _json["color_mode"] = get_colour_mode_string().c_str();

        switch (colour_mode) {
          case ColourMode::color_temp:
            if (colour.temp > -1) _json["color_temp"] = colour.temp;
            break;
          case ColourMode::hs:
            if (colour.h > -1) _json["h"] = colour.h;
            if (colour.s > -1) _json["s"] = colour.s;
            break;
          case ColourMode::xy:
            if (colour.x > -1) _json["x"] = colour.x;
            if (colour.y > -1) _json["y"] = colour.y;
            break;
          case ColourMode::rgb:
            if (colour.r > -1) _json["r"] = colour.r;
            if (colour.g > -1) _json["g"] = colour.g;
            if (colour.b > -1) _json["b"] = colour.b;
            break;
          case ColourMode::rgbw:
            if (colour.r > -1) _json["r"] = colour.r;
            if (colour.g > -1) _json["g"] = colour.g;
            if (colour.b > -1) _json["b"] = colour.b;
            if (colour.b > -1) _json["w"] = colour.b;
            break;
          case ColourMode::rgbww:
            if (colour.r > -1) _json["r"] = colour.r;
            if (colour.g > -1) _json["g"] = colour.g;
            if (colour.b > -1) _json["b"] = colour.b;
            if (colour.b > -1) _json["c"] = colour.b;
            if (colour.b > -1) _json["w"] = colour.b;
            break;

          default:
            break;
        }
      }

      // Effects
      if (effects_enabled) {
        _json["effect"] = effect.c_str();
      }
    }

    // Loop
    void loop() override {
    }

   public:
    State<bool> state_state;
    Command state_command;
    uint32_t transition = 0;

    bool brightness_enabled = false;
    uint8_t brightness = 0;
    uint32_t brightness_scale = 255;

    bool colour_enabled = false;
    ColourMode colour_mode = ColourMode::onoff;
    Colour colour;
    uint32_t max_mireds = 0;
    uint32_t min_mireds = 0;

    bool effects_enabled = false;
    EffectsList effects;
    std::string effect;
    uint8_t flash_long_time = 10;
    uint8_t flash_short_time = 2;
  };

}  // namespace HA_MQTT

#endif