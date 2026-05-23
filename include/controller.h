#pragma once

#include <Arduino.h>

enum class DeviceMode {
  IDLE,
  HEAT,
  COOL,
  VIBRATION,
  COMBINED,
  FAULT
};

struct Telemetry {
  float skinTemperatureC;
  float batteryVoltage;
  bool sensorValid;
};

struct Outputs {
  uint8_t heaterDuty;
  uint8_t coolerDuty;
  uint8_t vibrationDuty;
};

class TherapyController {
 public:
  TherapyController();

  void requestMode(DeviceMode requested);
  void resetFault();
  Outputs update(const Telemetry& telemetry);
  DeviceMode mode() const;
  const char* faultReason() const;

 private:
  DeviceMode mode_;
  bool faultLatched_;
  const char* faultReason_;

  void latchFault(const char* reason);
};

const char* modeName(DeviceMode mode);
