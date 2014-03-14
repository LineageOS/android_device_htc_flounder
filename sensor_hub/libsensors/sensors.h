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

#ifndef ANDROID_SENSORS_H
#define ANDROID_SENSORS_H

#include <errno.h>
#include <stdint.h>
#include <sys/cdefs.h>
#include <sys/types.h>

#include <linux/input.h>

#include <hardware/hardware.h>
#include <hardware/sensors.h>

__BEGIN_DECLS

/*****************************************************************************/

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

#define ID_A                                        0//CW_ACCELERATION
#define ID_M                                        1//CW_MAGNETIC
#define ID_GY                                       2//CW_GYRO
#define ID_L                                        3//CW_LIGHT
#define ID_PS                                       4//CW_PRESSURE
#define ID_O                                        5//CW_ORIENTATION
#define ID_RV                                       6//CW_ROTATIONVECTOR
#define ID_LA                                       7//CW_LINEARACCELERATION
#define ID_G                                        8//CW_GRAVITY

#define ID_CW_MAGNETIC_UNCALIBRATED                 9//CW_MAGNETIC_UNCALIBRATED
#define ID_CW_GYROSCOPE_UNCALIBRATED               10//CW_GYROSCOPE_UNCALIBRATED
#define ID_CW_GAME_ROTATION_VECTOR                 11//CW_GAME_ROTATION_VECTOR
#define ID_CW_GEOMAGNETIC_ROTATION_VECTOR          12//CW_GEOMAGNETIC_ROTATION_VECTOR
#define ID_CW_SIGNIFICANT_MOTION                   13//CW_SIGNIFICANT_MOTION
#define ID_CW_STEP_DETECTOR                        14//CW_STEP_DETECTOR
#define ID_CW_STEP_COUNTER                         15//CW_STEP_COUNTER
#define ID_Any_Motion                              16//HTC_ANY_MOTION

/*****************************************************************************/

// The SENSORS Module
#define EVENT_TYPE_LIGHT           ABS_MISC

enum ABS_status {
        ABS_ACC_X = 0x01,
        ABS_ACC_Y = 0x02,
        ABS_ACC_Z = 0x03,
        ABS_MAG_X = 0x04,
        ABS_MAG_Y = 0x05,
        ABS_MAG_Z = 0x06,
        ABS_GYRO_X = 0x07,
        ABS_GYRO_Y = 0x08,
        ABS_GYRO_Z = 0x09,
        ABS_MAG_ACCURACY = 0x0A,
        ABS_ORI_ACCURACY = 0x0B,
        ABS_LIGHT_Z = 0x0C,
        ABS_GEOMAGNETIC_ROTATION_VECTOR_X = 0x0D,
        ABS_GEOMAGNETIC_ROTATION_VECTOR_Y = 0x0E,
        ABS_GEOMAGNETIC_ROTATION_VECTOR_Z = 0x0F,
        ABS_PRESSURE_X = 0x10,
        ABS_PRESSURE_Y = 0x11,
        ABS_PRESSURE_Z = 0x12,
        ABS_ORI_X = 0x13,
        ABS_ORI_Y = 0x14,
        ABS_ORI_Z = 0x15,
        ABS_ROT_X = 0x16,
        ABS_ROT_Y = 0x17,
        ABS_ROT_Z = 0x18,
        ABS_LIN_X = 0x1A, // Jump over ABS_DISTANCE
        ABS_LIN_Y = 0x1B,
        ABS_LIN_Z = 0x1C,
        ABS_GRA_X = 0x1D,
        ABS_GRA_Y = 0x1E,
        ABS_GRA_Z = 0x1F,
        ABS_STEP_DETECTOR = 0x23,
        ABS_STEP_COUNTER = 0x24,
        ABS_MAGNETIC_UNCALIBRATED_X = 0x30, // Jump oveer ABS_MT_SLOT, Sensor HAL cannot receive
        ABS_MAGNETIC_UNCALIBRATED_Y = 0x31,
        ABS_MAGNETIC_UNCALIBRATED_Z = 0x32,
        ABS_MAGNETIC_UNCALIBRATED_BIAS_X = 0x3F, // Jump over ABS_MT_WIDTH_MINOR, this cause little white point, and touch fails
        ABS_MAGNETIC_UNCALIBRATED_BIAS_Y = 0x34,
        ABS_MAGNETIC_UNCALIBRATED_BIAS_Z = 0x35,
        ABS_GYROSCOPE_UNCALIBRATED_X = 0x36,
        ABS_GYROSCOPE_UNCALIBRATED_Y = 0x37,
        ABS_GYROSCOPE_UNCALIBRATED_Z = 0x38,
        ABS_GYROSCOPE_UNCALIBRATED_BIAS_X = 0x39,
        ABS_GYROSCOPE_UNCALIBRATED_BIAS_Y = 0x3A,
        ABS_GYROSCOPE_UNCALIBRATED_BIAS_Z = 0x3B,
        ABS_GAME_ROTATION_VECTOR_X = 0x3C,
        ABS_GAME_ROTATION_VECTOR_Y = 0x3D,
        ABS_GAME_ROTATION_VECTOR_Z = 0x3E,
};

#define CONVERT_A        0.01f
#define CONVERT_M        0.01f
#define CONVERT_GYRO     0.01f
#define CONVERT_PS       1.0f
#define CONVERT_O        0.1f
#define CONVERT_ALL      0.01f
#define CONVERT_PRESSURE 100
#define CONVERT_RV       10000

#define RANGE_A                     (2*GRAVITY_EARTH)
#define RESOLUTION_A                (RANGE_A/(256*1))

#define SENSOR_STATE_MASK           (0x7FFF)

/*****************************************************************************/

__END_DECLS

#endif  // ANDROID_SENSORS_H
