# Troubleshooting Guide

## Common Issues

### 1. Nothing appears on OLED
- Check SDA/SCL wiring and pin numbers (see `hardware/pinout_tables.md`).
- Ensure OLED is powered (3.3V and GND).
- Try running only `test_oled.ino`.
- Use I2C scanner if needed.

### 2. Motors do not run
- Check motor driver wiring and power.
- Confirm correct pins in code.
- Try `test_motor.ino`.

### 3. Encoder counts do not change
- Check encoder wiring and power.
- Confirm correct pins in code.
- Try `test_encoder.ino`.

### 4. Buttons not detected
- Check button wiring (use pull-ups or pull-downs as needed).
- Confirm correct pins in code.
- Try `test_buttons.ino`.

### 5. Buzzer does not beep
- Check buzzer wiring and pin.
- Try `test_buzzer.ino`.

### 6. MUX readings are wrong
- Check MUX select lines and SIG wiring.
- Confirm correct pins in code.
- Try `test_mux.ino`.

## General Tips
- Double-check all pin assignments in `hardware/pinout_tables.md`.
- Use Serial Monitor for extra debug info.
- Power cycle the ESP32 if it becomes unresponsive.
