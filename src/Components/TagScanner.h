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

#ifndef MQTT_TAG_SCANNER_H
#define MQTT_TAG_SCANNER_H

#include "../Command/Command.h"
#include "../State/State.h"
#include "Component.h"

namespace HA_MQTT {

  class TagScanner : public Component {
   public:
    // Constructors
    TagScanner(const char* _node_id) : Component("tag", _node_id) {}
    ~TagScanner() {}

    // To create the rest of the config message on startup
    void create_config_message(JsonObject& _json) override {
      _json["topic"] = create_topic("scanned").c_str();
    }

    void send_tag(const char* _tag_id) {
      send_message("scanned", default_qos, default_retain, _tag_id);
    }
  };

}  // namespace HA_MQTT

#endif