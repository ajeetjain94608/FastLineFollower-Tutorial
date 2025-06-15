# Fast Line Follower Robot Tutorial

This repository contains all code, pinouts, and test sketches for building and debugging a fast line follower robot using an ESP32 WROOM (38-pin) board.

## Features
- Modular, well-documented code for each hardware component
- Individual test sketches for motors, encoders, OLED, buttons, buzzer, and MUX
- All-in-one interactive test sketch
- Clean pinout and wiring documentation

## Folder Structure
```
fastlinefollower/         # Main robot code
component_tests/          # Individual and all-in-one test sketches
hardware/                 # Pinout tables and wiring diagrams
  └── wiring_diagrams/    # (Add your Fritzing or hand-drawn diagrams here)
docs/                     # Extra documentation (troubleshooting, test usage)
```

## Quick Start
1. Open any `.ino` file in the `component_tests` folder to test a specific component.
2. Use `test_all_in_one.ino` for an interactive menu to test all components from one sketch.
3. See `hardware/pinout_tables.md` for ESP32 pin assignments.
4. See `docs/component_testing.md` for test instructions and expected results.

## Hardware Used
- ESP32 WROOM (38-pin)
- Motor driver, encoders, MUX, OLED (I2C), push buttons, buzzer, RGB LED, etc.

## License
MIT
