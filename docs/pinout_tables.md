# ESP32 Pinout Table for Fast Line Follower

| Component      | Signal         | ESP32 Pin |
|---------------|---------------|-----------|
| Motor Right   | PWM           | 13        |
| Motor Right   | IN1           | 14        |
| Motor Right   | IN2           | 12        |
| Motor Left    | PWM           | 33        |
| Motor Left    | IN1           | 26        |
| Motor Left    | IN2           | 25        |
| Motor Driver  | STBY          | 27        |
| Encoder Left  | A             | 32        |
| Encoder Left  | B             | 16        |
| Encoder Right | A             | 35        |
| Encoder Right | B             | 36        |
| MUX           | S0            | 18        |
| MUX           | S1            | 19        |
| MUX           | S2            | 17        |
| MUX           | S3            | 5         |
| MUX           | SIG           | 34        |
| OLED (I2C)    | SDA           | 21        |
| OLED (I2C)    | SCL           | 22        |
| Buttons       | START         | 4         |
| Buttons       | CAL           | 15        |
| Buttons       | MODE          | 23        |
| Buttons       | RESET         | 0         |
| Buzzer        | Signal        | 2         |
| (Add more as needed) |         |           |

**Note:** Avoid pressing buttons on GPIO0 or GPIO15 during boot. See code comments for safe usage.
