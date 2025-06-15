#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Pin definitions (from fastlinefollower.ino)
#define MOTOR_R_PWM 13
#define MOTOR_R_IN1 14
#define MOTOR_R_IN2 12
#define MOTOR_STBY  27
#define MOTOR_L_IN1 26
#define MOTOR_L_IN2 25
#define MOTOR_L_PWM 33
#define ENCODER_L_A 32
#define ENCODER_L_B 16
#define ENCODER_R_A 35
#define ENCODER_R_B 36
#define MUX_S0 18
#define MUX_S1 19
#define MUX_S2 17
#define MUX_S3 5
#define MUX_SIG 34
#define BTN_START 4
#define BTN_CAL 15
#define BTN_MODE 23
#define BTN_RESET 0
#define BUZZER_PIN 2
#define OLED_SDA 21
#define OLED_SCL 22
#define OLED_RST -1
#define OLED_ADDR 0x3C
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define NUM_SENSORS 16
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RST);

// Encoder variables
volatile long encoderCountL = 0, encoderCountR = 0;
void IRAM_ATTR encoderL_ISR() {
  if (digitalRead(ENCODER_L_A) == digitalRead(ENCODER_L_B)) encoderCountL++;
  else encoderCountL--;
}
void IRAM_ATTR encoderR_ISR() {
  if (digitalRead(ENCODER_R_A) == digitalRead(ENCODER_R_B)) encoderCountR++;
  else encoderCountR--;
}

// State
int testMode = 0; // 0: Motor, 1: Encoder, 2: MUX, 3: Buttons, 4: Buzzer, 5: OLED
const char* testNames[] = {"Motor", "Encoder", "MUX", "Buttons", "Buzzer", "OLED"};
const int numTests = 6;

void setMotors(int left, int right) {
  digitalWrite(MOTOR_L_IN1, left > 0);
  digitalWrite(MOTOR_L_IN2, left < 0);
  digitalWrite(MOTOR_R_IN1, right > 0);
  digitalWrite(MOTOR_R_IN2, right < 0);
  ledcWrite(MOTOR_L_PWM, abs(left));
  ledcWrite(MOTOR_R_PWM, abs(right));
}

void beep() {
  digitalWrite(BUZZER_PIN, HIGH);
  delay(100);
  digitalWrite(BUZZER_PIN, LOW);
}

void setup() {
  // Pins
  pinMode(MOTOR_L_PWM, OUTPUT);
  pinMode(MOTOR_L_IN1, OUTPUT);
  pinMode(MOTOR_L_IN2, OUTPUT);
  pinMode(MOTOR_R_PWM, OUTPUT);
  pinMode(MOTOR_R_IN1, OUTPUT);
  pinMode(MOTOR_R_IN2, OUTPUT);
  pinMode(MOTOR_STBY, OUTPUT);
  digitalWrite(MOTOR_STBY, HIGH);
  ledcAttach(MOTOR_L_PWM, 20000, 8);
  ledcAttach(MOTOR_R_PWM, 20000, 8);
  pinMode(ENCODER_L_A, INPUT_PULLUP);
  pinMode(ENCODER_L_B, INPUT_PULLUP);
  pinMode(ENCODER_R_A, INPUT_PULLUP);
  pinMode(ENCODER_R_B, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(ENCODER_L_A), encoderL_ISR, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENCODER_R_A), encoderR_ISR, CHANGE);
  pinMode(MUX_S0, OUTPUT);
  pinMode(MUX_S1, OUTPUT);
  pinMode(MUX_S2, OUTPUT);
  pinMode(MUX_S3, OUTPUT);
  pinMode(MUX_SIG, INPUT);
  pinMode(BTN_START, INPUT_PULLUP);
  pinMode(BTN_CAL, INPUT_PULLUP);
  pinMode(BTN_MODE, INPUT_PULLUP);
  pinMode(BTN_RESET, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
  Wire.begin(OLED_SDA, OLED_SCL);
  display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR);
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("ALL TESTS");
  display.display();
  delay(1000);
  Serial.begin(115200);
}

void loop() {
  // --- Menu selection ---
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("Select Test:");
  for (int i = 0; i < numTests; i++) {
    if (i == testMode) display.print("> ");
    else display.print("  ");
    display.println(testNames[i]);
  }
  display.display();
  // BTN_MODE cycles test, BTN_START selects
  static int lastMode = HIGH, lastStart = HIGH;
  int curMode = digitalRead(BTN_MODE);
  int curStart = digitalRead(BTN_START);
  if (lastMode == HIGH && curMode == LOW) {
    testMode = (testMode + 1) % numTests;
    delay(200);
  }
  if (lastStart == HIGH && curStart == LOW) {
    runTest(testMode);
    delay(200);
  }
  lastMode = curMode;
  lastStart = curStart;
  delay(50);
}

void runTest(int mode) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("Testing: ");
  display.println(testNames[mode]);
  display.display();
  delay(500);
  if (mode == 0) { // Motor
    setMotors(200, 200);
    display.clearDisplay(); display.setCursor(0,0); display.println("Motor FWD"); display.display(); delay(1000);
    setMotors(-200, -200);
    display.clearDisplay(); display.setCursor(0,0); display.println("Motor BWD"); display.display(); delay(1000);
    setMotors(0, 0);
    display.clearDisplay(); display.setCursor(0,0); display.println("Motor STOP"); display.display(); delay(1000);
  } else if (mode == 1) { // Encoder
    unsigned long t0 = millis();
    long l0 = encoderCountL, r0 = encoderCountR;
    while (digitalRead(BTN_START) == HIGH) {
      display.clearDisplay();
      display.setCursor(0,0);
      display.print("EncL: "); display.println(encoderCountL);
      display.print("EncR: "); display.println(encoderCountR);
      display.display();
      Serial.print("EncL: "); Serial.print(encoderCountL);
      Serial.print("\tEncR: "); Serial.println(encoderCountR);
      delay(100);
      if (millis() - t0 > 5000) break;
    }
  } else if (mode == 2) { // MUX
    for (int k=0; k<10; k++) {
      display.clearDisplay();
      display.setCursor(0,0);
      for (int i = 0; i < NUM_SENSORS; i++) {
        digitalWrite(MUX_S0, i & 0x01);
        digitalWrite(MUX_S1, (i >> 1) & 0x01);
        digitalWrite(MUX_S2, (i >> 2) & 0x01);
        digitalWrite(MUX_S3, (i >> 3) & 0x01);
        delayMicroseconds(5);
        int val = analogRead(MUX_SIG);
        Serial.print("CH"); Serial.print(i); Serial.print(": "); Serial.print(val); Serial.print("\t");
        display.print(i); display.print(":"); display.print(val); display.print(" ");
        if ((i+1)%4==0) display.println();
      }
      Serial.println();
      display.display();
      delay(500);
    }
  } else if (mode == 3) { // Buttons
    for (int k=0; k<50; k++) {
      display.clearDisplay();
      display.setCursor(0,0);
      display.print("START: "); display.println(digitalRead(BTN_START) == LOW ? "PRESSED" : "RELEASED");
      display.print("CAL:   "); display.println(digitalRead(BTN_CAL) == LOW ? "PRESSED" : "RELEASED");
      display.print("MODE:  "); display.println(digitalRead(BTN_MODE) == LOW ? "PRESSED" : "RELEASED");
      display.print("RESET: "); display.println(digitalRead(BTN_RESET) == LOW ? "PRESSED" : "RELEASED");
      display.display();
      Serial.print("START: "); Serial.print(digitalRead(BTN_START) == LOW ? "PRESSED" : "RELEASED");
      Serial.print("\tCAL: "); Serial.print(digitalRead(BTN_CAL) == LOW ? "PRESSED" : "RELEASED");
      Serial.print("\tMODE: "); Serial.print(digitalRead(BTN_MODE) == LOW ? "PRESSED" : "RELEASED");
      Serial.print("\tRESET: "); Serial.println(digitalRead(BTN_RESET) == LOW ? "PRESSED" : "RELEASED");
      delay(100);
    }
  } else if (mode == 4) { // Buzzer
    for (int k=0; k<5; k++) {
      digitalWrite(BUZZER_PIN, HIGH);
      display.clearDisplay(); display.setCursor(0,0); display.println("BUZZER ON"); display.display(); delay(200);
      digitalWrite(BUZZER_PIN, LOW);
      display.clearDisplay(); display.setCursor(0,0); display.println("BUZZER OFF"); display.display(); delay(300);
    }
  } else if (mode == 5) { // OLED
    display.clearDisplay();
    display.setTextSize(2);
    display.setCursor(0, 0);
    display.println("OLED TEST");
    display.setTextSize(1);
    display.setCursor(0, 24);
    display.println("128x64 SSD1306");
    display.setCursor(0, 40);
    display.println("Hello, ESP32!");
    display.display();
    delay(2000);
  }
  display.clearDisplay();
  display.setCursor(0,0);
  display.println("Done. Press MODE");
  display.display();
  // Wait for MODE to return to menu
  while (digitalRead(BTN_MODE) == HIGH) delay(10);
  delay(300);
}
