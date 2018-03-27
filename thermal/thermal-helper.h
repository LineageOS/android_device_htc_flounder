#ifndef __THERMAL_HELPER_H__
#define __THERMAL_HELPER_H__

#include <android/hardware/thermal/1.1/IThermal.h>
#include <hardware/thermal.h>

namespace android {
namespace hardware {
namespace thermal {
namespace V1_1 {
namespace implementation {

using ::android::hardware::thermal::V1_0::CpuUsage;
using ::android::hardware::thermal::V1_0::Temperature;

constexpr const char *kCpuUsageFile = "/proc/stat";
constexpr const char *kCpuOnlineFileFormat = "/sys/devices/system/cpu/cpu%d/online";
constexpr const char *kTemperatureFileFormat = "/sys/class/thermal/thermal_zone%d/temp";

constexpr unsigned int kBatterySensorNum = 2;
constexpr unsigned int kCpuSensorNum = 3;
constexpr unsigned int kGpuSensorNum = 4;

constexpr unsigned int kCpuNum = 2;
constexpr unsigned int kTemperatureNum = 4;

constexpr const char *kBatteryLabel = "battery";
constexpr const char *kCpuLabel[kCpuNum] = {"CPU0", "CPU1"};
constexpr const char *kGpuLabel = "GPU";

constexpr unsigned int kBatteryShutdownThreshold = 68;

ssize_t fillTemperatures(hidl_vec<Temperature> *temperatures);
ssize_t fillCpuUsages(hidl_vec<CpuUsage> *cpuUsages);

}  // namespace implementation
}  // namespace V1_1
}  // namespace thermal
}  // namespace hardware
}  // namespace android

#endif //__THERMAL_HELPER_H__
