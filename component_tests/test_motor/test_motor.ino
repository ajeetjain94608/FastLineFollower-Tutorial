#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
// Motor pins only
#define MOTOR_R_PWM 13
#define MOTOR_R_IN1 14
#define MOTOR_R_IN2 12
#define MOTOR_STBY  27
#define MOTOR_L_IN1 26
#define MOTOR_L_IN2 25
#define MOTOR_L_PWM 33
// OLED pins
#define OLED_SDA    21
#define OLED_SCL    22
#define OLED_RST    -1
#define OLED_ADDR   0x3C
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RST);

void setMotors(int left, int right) {
  digitalWrite(MOTOR_L_IN1, left > 0);
  digitalWrite(MOTOR_L_IN2, left < 0);
  digitalWrite(MOTOR_R_IN1, right > 0);
  digitalWrite(MOTOR_R_IN2, right < 0);
  ledcWrite(MOTOR_L_PWM, abs(left));
  ledcWrite(MOTOR_R_PWM, abs(right));
}

void setup() {
  pinMode(MOTOR_L_PWM, OUTPUT);
  pinMode(MOTOR_L_IN1, OUTPUT);
  pinMode(MOTOR_L_IN2, OUTPUT);
  pinMode(MOTOR_R_PWM, OUTPUT);
  pinMode(MOTOR_R_IN1, OUTPUT);
  pinMode(MOTOR_R_IN2, OUTPUT);
  pinMode(MOTOR_STBY, OUTPUT);
  digitalWrite(MOTOR_STBY, HIGH);
  ledcAttach(MOTOR_L_PWM, 20000, 8); // New API
  ledcAttach(MOTOR_R_PWM, 20000, 8); // New API
  Wire.begin(OLED_SDA, OLED_SCL);
  display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR);
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("MOTOR TEST");
  display.display();
  delay(1000);
}

void loop() {
  setMotors(200, 200);
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("FWD");
  display.display();
  delay(1000);
  setMotors(-200, -200);
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("BWD");
  display.display();
  delay(1000);
  setMotors(0, 0);
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("STOP");
  display.display();
  delay(1000);
}
