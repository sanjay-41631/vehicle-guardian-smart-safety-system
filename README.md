# Vehicle Guardian — Smart Vehicle Safety System

ESP32 student prototype for Collision Guard, simulated Door Guard, MPU6050 impact monitoring, NEO-6M GPS status, MAX98357A voice alerts, and four-wheel motor-driver hardware.

**Important:** This is a prepared project package based on the documented pin map. Firmware must be compiled and tested with the actual hardware. It is not automotive-grade and must not be used on public roads.

## Contents
- `firmware/VehicleGuardian/VehicleGuardian.ino` — starter integration sketch
- `firmware/VehicleGuardian/data/` — add your actual WAV recordings here
- `hardware/wiring-connections.md` — pin map and safety notes
- `hardware/component-list.md` — component inventory
- `hardware/wokwi-simulation.png` — illustrative layout image, not an executable Wokwi project
- `docs/project-overview.md` — overview and test plan

## Arduino libraries
Install TinyGPSPlus, Adafruit MPU6050, Adafruit Unified Sensor, and the audio library that provides `AudioFileSourceLittleFS.h`, `AudioGeneratorWAV.h`, and `AudioOutputI2S.h`.

## WAV files
Copy your recordings into the `data` folder with these exact names:
- `collision_guard_warning.wav`
- `door_guard_warning.wav`
- `accident_guard_warning.wav`
- `gps_status.wav`

The actual audio recordings were not available to include in this ZIP. Your previous LittleFS upload failed because storage was full. Do not format LittleFS unless you intentionally want to erase its contents. Short mono PCM WAV files can reduce storage needs.

## Safety
- Keep motor battery disconnected during initial USB sensor/audio tests.
- HC-SR04 ECHO must go through a voltage divider/level shifter before GPIO34.
- Use common ground and do not feed 5 V into ESP32 GPIOs.
- Do not power motors from ESP32 USB.
- Use suitable Li-ion cell protection and a compatible charger.

## GitHub
Create a repository named `vehicle-guardian-smart-safety-system`, then publish this folder using GitHub Desktop or Git.
