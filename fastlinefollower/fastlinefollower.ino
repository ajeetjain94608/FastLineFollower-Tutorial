#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <EEPROM.h>

// --- Motor Driver Pins (unchanged) ---
#define MOTOR_R_PWM 13   // PWMB
#define MOTOR_R_IN1 14   // BIN1
#define MOTOR_R_IN2 12   // BIN2
#define MOTOR_STBY  27   // STBY
#define MOTOR_L_IN1 26   // AIN1
#define MOTOR_L_IN2 25   // AIN2
#define MOTOR_L_PWM 33   // PWMA

// --- MUX Pins ---
#define MUX_S0      18
#define MUX_S1      19
#define MUX_S2      17  // Changed from 23 to 17 (safe, previously used for BTN_MODE)
#define MUX_S3      5
#define MUX_SIG     34  // Analog input-only – OK

// --- Encoder Pins ---
#define ENCODER_L_A 32
#define ENCODER_L_B 16
#define ENCODER_R_A 35.


#define ENCODER_R_B 36

// --- OLED ---
#define OLED_SDA    21
#define OLED_SCL    22  // Changed from 22 to 17 (safe, previously used for BTN_MODE, now repurposed)
#define OLED_RST    -1  // Not connected

// --- Buttons (assigned to safe GPIOs) ---
#define BTN_START   4    // ✅ Safe, freed from RGB
#define BTN_CAL     15   // ✅ Use with external 10kΩ pull-down (boot pin)
#define BTN_MODE    23   // Changed from 17 to 23 (now using 23 for push button)
#define BTN_RESET   0    // ⚠️ Boot pin, use only with pull-down, unpressed during boot

// --- Buzzer ---
#define BUZZER_PIN  2    // ✅ Safe if unpressed at boot (usually OK for output)



// --- Constants ---
#define NUM_SENSORS 16
#define SENSOR_MAX  4095
#define SENSOR_MIN  0

#define BASE_SPEED  120   // Adjust for your motors (0-255)
#define MAX_SPEED   255

#define PID_KP      0.25
#define PID_KI      0.0
#define PID_KD      8.0

#define SPEED_KP    0.5
#define SPEED_KI    0.1
#define SPEED_KD    0.0
#define ACCEL_LIMIT 10 // Max change per loop

#define OLED_ADDR   0x3C
#define EEPROM_SIZE 128
#define EEPROM_ADDR_MIN 0
#define EEPROM_ADDR_MAX 64

// --- Globals ---
Adafruit_SSD1306 display(128, 64, &Wire, OLED_RST);

int sensorValues[NUM_SENSORS];
int sensorMin[NUM_SENSORS];
int sensorMax[NUM_SENSORS];

volatile long encoderCountL = 0, encoderCountR = 0;

bool running = false;
bool whiteLine = false; // false: black line, true: white line

// PID
float lastError = 0, integral = 0;

// Speed Control
long targetSpeedL = 0, targetSpeedR = 0;
float speedErrorL = 0, speedErrorR = 0;
float speedIntegralL = 0, speedIntegralR = 0;
float speedLastErrorL = 0, speedLastErrorR = 0;
long lastEncoderL = 0, lastEncoderR = 0;
unsigned long lastSpeedTime = 0;

// --- Function Prototypes ---
void readSensors();
int getLinePosition();
void calibrateSensors();
void setMotors(int left, int right);
void updateOLED();
void setRGB(uint8_t r, uint8_t g, uint8_t b);
void handleButtons();
void IRAM_ATTR encoderL_ISR();
void IRAM_ATTR encoderR_ISR();
void saveCalibration();
void loadCalibration();
void setTargetSpeeds(int left, int right);
void updateSpeedControl();

void setup() {
  // Serial
  Serial.begin(115200);

  // MUX pins
  pinMode(MUX_S0, OUTPUT);
  pinMode(MUX_S1, OUTPUT);
  pinMode(MUX_S2, OUTPUT);
  pinMode(MUX_S3, OUTPUT);
  pinMode(MUX_SIG, INPUT);

  // Motor pins
  pinMode(MOTOR_L_PWM, OUTPUT);
  pinMode(MOTOR_L_IN1, OUTPUT);
  pinMode(MOTOR_L_IN2, OUTPUT);
  pinMode(MOTOR_R_PWM, OUTPUT);
  pinMode(MOTOR_R_IN1, OUTPUT);
  pinMode(MOTOR_R_IN2, OUTPUT);

  // Encoder pins
  pinMode(ENCODER_L_A, INPUT_PULLUP);
  pinMode(ENCODER_L_B, INPUT_PULLUP);
  pinMode(ENCODER_R_A, INPUT_PULLUP);
  pinMode(ENCODER_R_B, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(ENCODER_L_A), encoderL_ISR, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENCODER_R_A), encoderR_ISR, CHANGE);

  // Buttons
  pinMode(BTN_START, INPUT_PULLUP);
  pinMode(BTN_CAL, INPUT_PULLUP);
  pinMode(BTN_MODE, INPUT_PULLUP);
  pinMode(BTN_RESET, INPUT_PULLUP);

  // OLED
  Wire.begin(OLED_SDA, OLED_SCL);
  display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR);
  display.clearDisplay();
  display.display();

  // EEPROM
  EEPROM.begin(EEPROM_SIZE);
  loadCalibration();

  // Initial sensor calibration
  calibrateSensors();

  setRGB(0, 0, 255); // Blue: Ready
  updateOLED();
}

void loop() {
  handleButtons();

  if (running) {
    readSensors();
    int position = getLinePosition();
    float error = position - (NUM_SENSORS - 1) * 500 / 2; // Center is 7500 for 16 sensors

    // PID
    integral += error;
    float derivative = error - lastError;
    float correction = PID_KP * error + PID_KI * integral + PID_KD * derivative;
    lastError = error;

    // --- Lost line recovery ---
    bool lostLine = true;
    for (int i = 0; i < NUM_SENSORS; i++) {
      if (sensorValues[i] > 100) lostLine = false;
    }
    if (lostLine) {
      setMotors(0, 0);
      setRGB(255, 0, 0); // Red: Lost
      updateOLED();
      delay(100);
      return;
    }

    // --- Speed control ---
    int base = BASE_SPEED;
    int leftTarget = base + correction;
    int rightTarget = base - correction;
    leftTarget = constrain(leftTarget, 0, MAX_SPEED);
    rightTarget = constrain(rightTarget, 0, MAX_SPEED);
    setTargetSpeeds(leftTarget, rightTarget);
    updateSpeedControl();

    updateOLED();
    setRGB(0, 255, 0); // Green: Running
  } else {
    setMotors(0, 0);
    setRGB(0, 0, 255); // Blue: Idle
  }

  delay(10);
}

// --- Acceleration limiting and speed PID ---
void setTargetSpeeds(int left, int right) {
  // Limit acceleration
  if (abs(left - targetSpeedL) > ACCEL_LIMIT) {
    targetSpeedL += (left > targetSpeedL ? ACCEL_LIMIT : -ACCEL_LIMIT);
  } else {
    targetSpeedL = left;
  }
  if (abs(right - targetSpeedR) > ACCEL_LIMIT) {
    targetSpeedR += (right > targetSpeedR ? ACCEL_LIMIT : -ACCEL_LIMIT);
  } else {
    targetSpeedR = right;
  }
}

void updateSpeedControl() {
  unsigned long now = millis();
  if (now - lastSpeedTime < 20) return; // 50Hz
  lastSpeedTime = now;
  long encL = encoderCountL;
  long encR = encoderCountR;
  long speedL = encL - lastEncoderL;
  long speedR = encR - lastEncoderR;
  lastEncoderL = encL;
  lastEncoderR = encR;
  // Target speed in encoder ticks per cycle (tune for your robot)
  int targetTicksL = map(targetSpeedL, 0, 255, 0, 20);
  int targetTicksR = map(targetSpeedR, 0, 255, 0, 20);
  speedErrorL = targetTicksL - speedL;
  speedErrorR = targetTicksR - speedR;
  speedIntegralL += speedErrorL;
  speedIntegralR += speedErrorR;
  float speedDerivL = speedErrorL - speedLastErrorL;
  float speedDerivR = speedErrorR - speedLastErrorR;
  speedLastErrorL = speedErrorL;
  speedLastErrorR = speedErrorR;
  int pwmL = targetSpeedL + SPEED_KP * speedErrorL + SPEED_KI * speedIntegralL + SPEED_KD * speedDerivL;
  int pwmR = targetSpeedR + SPEED_KP * speedErrorR + SPEED_KI * speedIntegralR + SPEED_KD * speedDerivR;
  pwmL = constrain(pwmL, 0, MAX_SPEED);
  pwmR = constrain(pwmR, 0, MAX_SPEED);
  setMotors(pwmL, pwmR);
}

// --- Sensor Reading via MUX ---
void readSensors() {
  for (int i = 0; i < NUM_SENSORS; i++) {
    digitalWrite(MUX_S0, i & 0x01);
    digitalWrite(MUX_S1, (i >> 1) & 0x01);
    digitalWrite(MUX_S2, (i >> 2) & 0x01);
    digitalWrite(MUX_S3, (i >> 3) & 0x01);
    delayMicroseconds(5);
    int val = analogRead(MUX_SIG);
    // Normalize
    val = map(val, sensorMin[i], sensorMax[i], 0, 1000);
    val = constrain(val, 0, 1000);
    if (whiteLine) val = 1000 - val; // Invert for white line
    sensorValues[i] = val;
  }
}

// --- Get Line Position (Weighted Average) ---
int getLinePosition() {
  long avg = 0, sum = 0;
  for (int i = 0; i < NUM_SENSORS; i++) {
    int value = sensorValues[i];
    avg += (long)value * i * 500; // 0, 500, 1000, ..., 7500
    sum += value;
  }
  if (sum == 0) return (NUM_SENSORS - 1) * 500 / 2; // Center if no line detected
  return avg / sum;
}

// --- EEPROM Calibration Save/Load ---
void saveCalibration() {
  for (int i = 0; i < NUM_SENSORS; i++) {
    EEPROM.writeUShort(EEPROM_ADDR_MIN + i * 2, sensorMin[i]);
    EEPROM.writeUShort(EEPROM_ADDR_MAX + i * 2, sensorMax[i]);
  }
  EEPROM.commit();
}
void loadCalibration() {
  for (int i = 0; i < NUM_SENSORS; i++) {
    sensorMin[i] = EEPROM.readUShort(EEPROM_ADDR_MIN + i * 2);
    sensorMax[i] = EEPROM.readUShort(EEPROM_ADDR_MAX + i * 2);
    if (sensorMin[i] == 0xFFFF || sensorMax[i] == 0xFFFF) {
      sensorMin[i] = SENSOR_MAX;
      sensorMax[i] = SENSOR_MIN;
    }
  }
}

// --- Calibration ---
void calibrateSensors() {
  setRGB(255, 255, 0); // Yellow: Calibrating
  for (int i = 0; i < NUM_SENSORS; i++) {
    sensorMin[i] = SENSOR_MAX;
    sensorMax[i] = SENSOR_MIN;
  }
  for (int t = 0; t < 400; t++) { // Sweep for 4 seconds
    for (int i = 0; i < NUM_SENSORS; i++) {
      digitalWrite(MUX_S0, i & 0x01);
      digitalWrite(MUX_S1, (i >> 1) & 0x01);
      digitalWrite(MUX_S2, (i >> 2) & 0x01);
      digitalWrite(MUX_S3, (i >> 3) & 0x01);
      delayMicroseconds(5);
      int val = analogRead(MUX_SIG);
      if (val < sensorMin[i]) sensorMin[i] = val;
      if (val > sensorMax[i]) sensorMax[i] = val;
    }
    delay(10);
  }
  saveCalibration();
  setRGB(0, 0, 255); // Blue: Done
}

// --- Motor Control ---
void setMotors(int left, int right) {
  // Left
  digitalWrite(MOTOR_L_IN1, left > 0);
  digitalWrite(MOTOR_L_IN2, left <= 0);
  ledcWrite(0, abs(left));
  // Right
  digitalWrite(MOTOR_R_IN1, right > 0);
  digitalWrite(MOTOR_R_IN2, right <= 0);
  ledcWrite(1, abs(right));
}

// --- OLED Display ---
void updateOLED() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print(running ? "RUN" : "IDLE");
  display.print(whiteLine ? " WL" : " BL");
  display.setCursor(0, 10);
  for (int i = 0; i < NUM_SENSORS; i++) {
    int h = map(sensorValues[i], 0, 1000, 0, 40);
    display.fillRect(i * 8, 60 - h, 6, h, SSD1306_WHITE);
  }
  display.display();
}

// --- RGB LED ---
// For common cathode: 0=off, 255=full brightness
// For common anode: 255=off, 0=full brightness (invert values)
void setRGB(uint8_t r, uint8_t g, uint8_t b) {
  // If your LED is common anode, uncomment the next 3 lines:
  // r = 255 - r;
  // g = 255 - g;
  // b = 255 - b;
  ledcAttachPin(RGB_R_PIN, 3); // Channel 3
  ledcAttachPin(RGB_G_PIN, 4); // Channel 4
  ledcAttachPin(RGB_B_PIN, 5); // Channel 5
  ledcWrite(3, r);
  ledcWrite(4, g);
  ledcWrite(5, b);
}

// --- Button Handling ---
void handleButtons() {
  static bool lastStart = HIGH, lastCal = HIGH, lastMode = HIGH, lastReset = HIGH;
  bool curStart = digitalRead(BTN_START);
  bool curCal = digitalRead(BTN_CAL);
  bool curMode = digitalRead(BTN_MODE);
  bool curReset = digitalRead(BTN_RESET);

  if (lastStart == HIGH && curStart == LOW) running = !running;
  if (lastCal == HIGH && curCal == LOW) calibrateSensors();
  if (lastMode == HIGH && curMode == LOW) whiteLine = !whiteLine;
  if (lastReset == HIGH && curReset == LOW) ESP.restart();

  lastStart = curStart;
  lastCal = curCal;
  lastMode = curMode;
  lastReset = curReset;
}

// --- Encoder ISRs ---
void IRAM_ATTR encoderL_ISR() {
  if (digitalRead(ENCODER_L_A) == digitalRead(ENCODER_L_B)) encoderCountL++;
  else encoderCountL--;
}
void IRAM_ATTR encoderR_ISR() {
  if (digitalRead(ENCODER_R_A) == digitalRead(ENCODER_R_B)) encoderCountR++;
  else encoderCountR--;
}