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

#ifndef MQTT_ALARM_H
#define MQTT_ALARM_H

#include "../Command/Command.h"
#include "../State/State.h"
#include "Component.h"

namespace HA_MQTT {
  class Alarm : public Component {
   public:
    // Enums
    enum class AlarmState {
      disarmed,
      armed_home,
      armed_away,
      armed_night,
      armed_vacation,
      armed_custom_bypass,
      pending,
      triggered,
      arming,
      disarming
    };

    // Constructors
    Alarm(const char* _node_id, const char* _code = nullptr) : Component("alarm_control_panel", _node_id),
                                                               code(_code),
                                                               state(this, "state"),
                                                               command(this, "set") {
      // Setup the state
      state.get_state_string = [&]() { return get_alarm_state_string(); };

      // Setup the command
      command.add_valid_input("DISARM", &on_disarm);
      command.add_valid_input("ARM_AWAY", &on_arm_away);
      command.add_valid_input("ARM_HOME", &on_arm_home);
      command.add_valid_input("ARM_NIGHT", &on_arm_night);
      command.add_valid_input("ARM_VACATION", &on_arm_vacation);
      command.add_valid_input("ARM_CUSTOM_BYPASS", &on_custom_bypass);
    }
    ~Alarm() {}

    // Config Methods
    void create_config_message(JsonObject& _json) override {
      state.set_state_config(_json);
      command.set_command_config(_json);
      if (code) _json["code"] = code;
    }

    // State methods
    std::string get_alarm_state_string() {
      switch (state.get_state()) {
        case AlarmState::disarmed:
          return "disarmed";
        case AlarmState::armed_home:
          return "armed_home";
        case AlarmState::armed_away:
          return "armed_away";
        case AlarmState::armed_night:
          return "armed_night";
        case AlarmState::armed_vacation:
          return "armed_vacation";
        case AlarmState::armed_custom_bypass:
          return "armed_custom_bypass";
        case AlarmState::pending:
          return "pending";
        case AlarmState::triggered:
          return "triggered";
        case AlarmState::arming:
          return "arming";
        case AlarmState::disarming:
          return "disarming";
        default:
          return "";
      }
    }

    // Event Methods
    EventCallback on_disarm = [&](void*) { state.update_state(AlarmState::disarmed); };
    EventCallback on_arm_away = [&](void*) { state.update_state(AlarmState::armed_away); };
    EventCallback on_arm_home = [&](void*) { state.update_state(AlarmState::armed_home); };
    EventCallback on_arm_night = [&](void*) { state.update_state(AlarmState::armed_night); };
    EventCallback on_arm_vacation = [&](void*) { state.update_state(AlarmState::armed_vacation); };
    EventCallback on_custom_bypass = [&](void*) { state.update_state(AlarmState::armed_custom_bypass); };

    // Update methods
    void send_update_message(uint8_t _qos = 0, bool _retain = false) override {
      state.send_state_message(_qos, _retain);
    }

    // Loop Method
    void loop() override {
      state.loop();
    }

    // Data
   protected:
    const char* code = nullptr;

   public:
    State<AlarmState> state;
    Command command;
  };

}  // namespace HA_MQTT

#endif