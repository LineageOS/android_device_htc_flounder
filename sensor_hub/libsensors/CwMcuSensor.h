/*
 * Copyright (C) 2008-2014 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef ANDROID_CWMCU_SENSOR_H
#define ANDROID_CWMCU_SENSOR_H

#include <errno.h>
#include <stdint.h>
#include <sys/cdefs.h>
#include <sys/types.h>

#include "InputEventReader.h"
#include "sensors.h"
#include "SensorBase.h"

/*****************************************************************************/

// Must match driver copy of .../linux/include/linux/CwMcuSensor.h file
typedef enum {
    CW_ACCELERATION                = 0,
    CW_MAGNETIC                    = 1,
    CW_GYRO                        = 2,
    CW_LIGHT                       = 3,
    CW_PRESSURE                    = 5,
    CW_ORIENTATION                 = 6,
    CW_ROTATIONVECTOR              = 7,
    CW_LINEARACCELERATION          = 8,
    CW_GRAVITY                     = 9,
    CW_MAGNETIC_UNCALIBRATED       = 16,
    CW_GYROSCOPE_UNCALIBRATED      = 17,
    CW_GAME_ROTATION_VECTOR        = 18,
    CW_GEOMAGNETIC_ROTATION_VECTOR = 19,
    CW_SIGNIFICANT_MOTION          = 20,
    CW_STEP_DETECTOR               = 21,
    CW_STEP_COUNTER                = 22,
    HTC_ANY_MOTION                 = 28,
    CW_SENSORS_ID_END // Be careful, do not exceed 31
} CW_SENSORS_ID;

#define        SAVE_PATH_ACC                                "/data/misc/AccOffset.txt"
#define        SAVE_PATH_MAG                                "/data/misc/cw_calibrator_mag.ini"
#define        SAVE_PATH_GYRO                                "/data/system/cw_calibrator_gyro.ini"

#define        BOOT_MODE_PATH                                "sys/class/htc_sensorhub/sensor_hub/boot_mode"

#define        numSensors        CW_SENSORS_ID_END

struct input_event;

class CwMcuSensor : public SensorBase {

        uint32_t mEnabled;
        InputEventCircularReader mInputReader;
        sensors_event_t mPendingEvents[numSensors];
        bool mHasPendingEventPressure;
        bool mHasPendingEventActivity;
        bool mHasPendingEventLight;
        bool mHasPendingEventMagnetic;
        bool mHasPendingEventOrientation;
        uint32_t mPendingMask;
        char input_sysfs_path[PATH_MAX];
        int input_sysfs_path_len;

        int setInitialState(int what);
        float indexToValue(size_t index) const;

public:
        CwMcuSensor();
        virtual ~CwMcuSensor();
        virtual int readEvents(sensors_event_t* data, int count);
        virtual bool hasPendingEvents() const;
        virtual int setDelay(int32_t handle, int64_t ns);
        virtual int setEnable(int32_t handle, int enabled);
        virtual int getEnable(int32_t handle);
        int find_sensor(int32_t handle);
        void cw_save_calibrator_file(int type, const char * path, int* str);
        int cw_read_calibrator_file(int type, const char * path, int* str);
        void processEvent(int code, float value);
        void calculate_rv_4th_element(int sensors_id);
};

/*****************************************************************************/

#endif  // ANDROID_CWMCU_SENSOR_H
