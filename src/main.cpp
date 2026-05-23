#include <Arduino.h>

#include "controller.h"

namespace {
constexpr uint8_t kHeaterPin = 25;
constexpr uint8_t kCoolerPin = 26;
constexpr uint8_t kVibrationPin = 27;
constexpr uint32_t kControlPeriodMs = 250;
TherapyController controller;
uint32_t lastUpdate = 0;

Telemetry readTelemetry() {
  // Replace these bring-up values with thermistor and BMS driver readings.
  return {36.5F, 3.92F, true};
}

void applyOutputs(const Outputs& outputs) {
  analogWrite(kHeaterPin, outputs.heaterDuty);
  analogWrite(kCoolerPin, outputs.coolerDuty);
  analogWrite(kVibrationPin, outputs.vibrationDuty);
}

void handleCommand(String command) {
  command.trim();
  command.toLowerCase();
  if (command == "heat") controller.requestMode(DeviceMode::HEAT);
  else if (command == "cool") controller.requestMode(DeviceMode::COOL);
  else if (command == "vibe") controller.requestMode(DeviceMode::VIBRATION);
  else if (command == "combined") controller.requestMode(DeviceMode::COMBINED);
  else if (command == "idle") controller.requestMode(DeviceMode::IDLE);
  else if (command == "reset") controller.resetFault();
}
}  // namespace

void setup() {
  Serial.begin(115200);
  pinMode(kHeaterPin, OUTPUT);
  pinMode(kCoolerPin, OUTPUT);
  pinMode(kVibrationPin, OUTPUT);
  Serial.println("Therapy controller ready. Commands: heat, cool, vibe, combined, idle, reset");
}

void loop() {
  if (Serial.available()) handleCommand(Serial.readStringUntil('\n'));
  if (millis() - lastUpdate < kControlPeriodMs) return;
  lastUpdate = millis();
  const Telemetry telemetry = readTelemetry();
  const Outputs outputs = controller.update(telemetry);
  applyOutputs(outputs);
  Serial.printf("mode=%s temp=%.1fC battery=%.2fV pwm=%u/%u/%u", modeName(controller.mode()), telemetry.skinTemperatureC,
                telemetry.batteryVoltage, outputs.heaterDuty, outputs.coolerDuty, outputs.vibrationDuty);
  if (controller.mode() == DeviceMode::FAULT) Serial.printf(" fault=%s", controller.faultReason());
  Serial.println();
}
