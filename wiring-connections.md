# Wiring Connections

Verify every connection against your actual board before powering.

## HC-SR04
- VCC → module-rated supply (typically 5 V)
- GND → GND
- TRIG → GPIO23
- ECHO → GPIO34 through a voltage divider/level shifter; never connect 5 V ECHO directly.

## IR sensor — simulated Door Guard
- OUT → GPIO35
- Observed logic: HIGH = clear; LOW = object/hand detected.
- VCC/GND → module-rated supply and common GND.

## MPU6050
- VCC → 3.3 V; GND → GND
- SDA → GPIO21; SCL → GPIO22
- I2C scanner detected `0x68`.

## NEO-6M GPS
- GPS TX → ESP32 GPIO27 (UART RX)
- GPS RX ← ESP32 GPIO4 (UART TX; optional if only receiving)
- UART baud: 9600; power according to your module rating.

## MAX98357A
- VIN → suitable 5 V/VIN supply as previously tested
- GND → common GND
- BCLK → GPIO17; LRC/WS → GPIO16; DIN → GPIO5
- Speaker connects only to SPK+ and SPK−.

## L298N driver A
- IN1 → GPIO14; IN2 → GPIO13; IN3 → GPIO18; IN4 → GPIO19
- ENA/ENB jumpers installed.

## L298N driver B
- IN1 → GPIO25; IN2 → GPIO26; IN3 → GPIO32; IN4 → GPIO33
- ENA/ENB jumpers installed.

Project notes associate motors 1–2 with driver A and motors 3–4 with driver B. Confirm actual terminal mapping and polarity.

## Safety
Keep the motor battery disconnected during initial tests. Use a common ground. Never power motors from ESP32 USB. Do not mix lithium-ion cells of different condition/capacity in one pack; use suitable protection and a compatible charger.
