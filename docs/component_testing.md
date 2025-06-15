# Component Testing Guide

This guide explains how to use the test sketches in `component_tests/` to verify each hardware component of your fast line follower robot.

## How to Use
- Open the `.ino` file for the component you want to test in the Arduino IDE or VS Code with Arduino extension.
- Upload the sketch to your ESP32.
- Observe the OLED and/or Serial Monitor for test results.

## Test Sketches

| Test File                | What it Tests         | How to Use / What to Expect                |
|-------------------------|----------------------|--------------------------------------------|
| test_motor.ino          | Motors               | Motors run forward/backward, status on OLED|
| test_encoder.ino        | Encoders             | Encoder counts shown on OLED & Serial      |
| test_oled.ino           | OLED Display         | Text appears on OLED                       |
| test_buttons.ino        | All 4 Buttons        | Button states on OLED & Serial             |
| test_buzzer.ino         | Buzzer               | Buzzer beeps, state on OLED                |
| test_mux.ino            | MUX channels         | Analog values on OLED & Serial             |
| test_all_in_one.ino     | All (menu-driven)    | Select and test any component via buttons  |

## Troubleshooting
- If a component does not respond, check wiring and pinout in `hardware/pinout_tables.md`.
- For I2C issues, try running only the OLED test and check pull-ups.
- For Serial output, use 115200 baud.

See also: `docs/troubleshooting.md` for more help.
