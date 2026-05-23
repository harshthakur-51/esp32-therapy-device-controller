# ESP32 Multimodal Therapy Device Controller

Firmware starter for a cosmetic therapy device that coordinates resistive heating,
Peltier cooling, and ERM vibration while applying independent safety cutoffs.

This public portfolio implementation documents and demonstrates the embedded
control structure without publishing any production hardware design or regulated
device firmware.

## Control modes

| Mode | Actuation target |
| --- | --- |
| `IDLE` | Outputs disabled |
| `HEAT` | Maintain 38-42 C with PWM heater control |
| `COOL` | Maintain 18-22 C with Peltier control |
| `VIBRATION` | Adjustable ERM PWM actuation |
| `COMBINED` | Thermal therapy plus vibration |
| `FAULT` | Latched actuator shutdown |

## Safety behavior

- Sensor bounds check prevents actuation after a disconnected or implausible probe.
- Hardware-independent thermal cutoff latches a `FAULT` above 44 C.
- Low-battery threshold disables high-current thermal modes.
- Faults require an explicit reset before outputs can be re-enabled.

## Build

The starter is structured as a PlatformIO Arduino project:

```bash
pio run
pio device monitor
```

The firmware uses serial commands to make hardware bring-up straightforward:

```text
heat
cool
vibe
combined
idle
reset
```

Replace the demonstration sensor methods in `src/main.cpp` with the board's
thermistor/ADC and battery-monitoring drivers during hardware integration.
