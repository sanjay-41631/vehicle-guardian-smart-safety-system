# Component List

| Component | Purpose | Notes |
|---|---|---|
| ESP32 DevKit V1 | Main controller | USB for initial tests |
| HC-SR04 | Collision Guard distance | ECHO voltage divider required |
| IR obstacle sensor | Simulated Door Guard | HIGH = clear, LOW = object detected |
| MPU6050 | Acceleration/gyro sensing | I2C address `0x68` detected |
| NEO-6M GPS | GPS location | Outdoor sky view may be required |
| MAX98357A | I2S voice output | Speaker on SPK+ / SPK− |
| Speaker | Audio output | Check impedance and power rating |
| 2 × L298N | Motor drivers | ENA/ENB jumpers used |
| 4 × DC motors | Four-wheel drive | Test with wheels raised |
| Li-ion cells/holder | Motor power | Requires suitable protection/charging |
| Jumper wires | Connections | Verify polarity and common ground |
