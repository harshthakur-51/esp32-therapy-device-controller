#include "controller.h"

namespace {
constexpr float kThermalCutoffC = 44.0F;
constexpr float kMinimumTemperatureC = 5.0F;
constexpr float kMaximumTemperatureC = 50.0F;
constexpr float kLowBatteryV = 3.35F;
constexpr float kHeatFloorC = 38.0F;
constexpr float kHeatCeilingC = 42.0F;
constexpr float kCoolFloorC = 18.0F;
constexpr float kCoolCeilingC = 22.0F;
}  // namespace

TherapyController::TherapyController()
    : mode_(DeviceMode::IDLE), faultLatched_(false), faultReason_("") {}

void TherapyController::requestMode(DeviceMode requested) {
  if (!faultLatched_) mode_ = requested;
}

void TherapyController::resetFault() {
  faultLatched_ = false;
  faultReason_ = "";
  mode_ = DeviceMode::IDLE;
}

void TherapyController::latchFault(const char* reason) {
  faultLatched_ = true;
  faultReason_ = reason;
  mode_ = DeviceMode::FAULT;
}

Outputs TherapyController::update(const Telemetry& telemetry) {
  Outputs outputs = {0, 0, 0};
  if (!telemetry.sensorValid || telemetry.skinTemperatureC < kMinimumTemperatureC || telemetry.skinTemperatureC > kMaximumTemperatureC) {
    latchFault("temperature sensor invalid");
  } else if (telemetry.skinTemperatureC >= kThermalCutoffC) {
    latchFault("thermal cutoff reached");
  } else if (telemetry.batteryVoltage < kLowBatteryV &&
             (mode_ == DeviceMode::HEAT || mode_ == DeviceMode::COOL || mode_ == DeviceMode::COMBINED)) {
    latchFault("battery too low for thermal mode");
  }
  if (faultLatched_) return outputs;
  if (mode_ == DeviceMode::HEAT || mode_ == DeviceMode::COMBINED) {
    outputs.heaterDuty = telemetry.skinTemperatureC < kHeatFloorC ? 190 : telemetry.skinTemperatureC < kHeatCeilingC ? 70 : 0;
  }
  if (mode_ == DeviceMode::COOL) {
    outputs.coolerDuty = telemetry.skinTemperatureC > kCoolCeilingC ? 190 : telemetry.skinTemperatureC > kCoolFloorC ? 70 : 0;
  }
  if (mode_ == DeviceMode::VIBRATION || mode_ == DeviceMode::COMBINED) outputs.vibrationDuty = 150;
  return outputs;
}

DeviceMode TherapyController::mode() const { return mode_; }
const char* TherapyController::faultReason() const { return faultReason_; }

const char* modeName(DeviceMode mode) {
  switch (mode) {
    case DeviceMode::IDLE: return "IDLE";
    case DeviceMode::HEAT: return "HEAT";
    case DeviceMode::COOL: return "COOL";
    case DeviceMode::VIBRATION: return "VIBRATION";
    case DeviceMode::COMBINED: return "COMBINED";
    case DeviceMode::FAULT: return "FAULT";
  }
  return "UNKNOWN";
}
