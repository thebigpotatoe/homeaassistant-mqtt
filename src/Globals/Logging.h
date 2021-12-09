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

#ifndef HA_LOGGING_H
#define HA_LOGGING_H

#if defined(ESP8266)

#define HA_LOGGER(f_, ...) Serial.printf(PSTR(f_), ##__VA_ARGS__)

#include "Arduino.h"

class Logger {
 public:
  Logger() {
    if (!Serial) {
      Serial.begin(115200);
      Serial.println();
      Serial.println("Home Assistant MQTT logging has begun...");
    }
  }
};
Logger logger;

#endif

#endif