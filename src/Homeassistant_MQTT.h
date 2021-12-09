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

// Definintions
#define ARDUINOJSON_ENABLE_STD_STRING 1

// Declare the global data
#include "Globals/Declarations.h"

// Include logging
#include "Globals/Logging.h"

// Include all the main files
#include "Components/Component.h"
#include "State/State.h"
#include "Command/Command.h"
#include "Client/Client.h"

// Include all the components
#include "Components/Alarm.h"
#include "Components/Binary Sensor.h"
#include "Components/Camera.h"
#include "Components/Cover.h"
#include "Components/Fan.h"
#include "Components/Humidifier.h"
#include "Components/HVAC.h"
#include "Components/Light.h"
#include "Components/Lock.h"
#include "Components/Number.h"
#include "Components/Scene.h"
#include "Components/Select.h"
#include "Components/Sensor.h"
#include "Components/Switch.h"
#include "Components/TagScanner.h"
#include "Components/Tracker.h"
#include "Components/Trigger.h"
#include "Components/Vacuum.h"

// Define the global data
#include "Globals/Definitions.h"

// Automatically include the HA_MQTT namespace
#ifndef HA_MQTT_DISABLE_NAMESPACE
using namespace HA_MQTT;
#endif