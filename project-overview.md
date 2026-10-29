# Project Overview

Vehicle Guardian is a tabletop student prototype that demonstrates local vehicle-safety alerts and status sensing.

## Planned features
1. Collision Guard — HC-SR04 measures obstacle distance.
2. Door Guard — IR detects an object near a simulated door; no physical servo door is used in this version.
3. Accident Guard — MPU6050 acceleration magnitude is checked against a preliminary threshold that must be calibrated.
4. GPS — NEO-6M location fix is parsed with TinyGPSPlus.
5. Voice alerts — MAX98357A plays WAV files stored in LittleFS.
6. Four-wheel drive hardware — two L298N boards are mapped to four DC motors. This starter sketch holds motor outputs LOW; joystick/ESP-NOW drive integration is not included.

## Suggested testing order
1. Confirm MPU6050 address `0x68`.
2. Test HC-SR04 readings.
3. Confirm IR HIGH/LOW logic.
4. Test GPS outdoors with Serial Monitor.
5. Test one short WAV file.
6. Integrate alerts one at a time.
7. Check battery protection and test motor drivers with wheels raised.

## Limitations
This is not automotive-grade. GPS may not fix indoors. Impact thresholds require calibration. LoRa vehicle-to-vehicle communication was a simulation concept, not part of the documented physical wiring. `wokwi-simulation.png` is illustrative, not a runnable Wokwi project.
