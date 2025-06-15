#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
// Encoder pins only
#define ENCODER_L_A 32
#define ENCODER_L_B 16
#define ENCODER_R_A 35
#define ENCODER_R_B 36
// OLED pins
#define OLED_SDA    21
#define OLED_SCL    22
#define OLED_RST    -1
#define OLED_ADDR   0x3C
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RST);

volatile long encoderCountL = 0, encoderCountR = 0;

void IRAM_ATTR encoderL_ISR() {
  if (digitalRead(ENCODER_L_A) == digitalRead(ENCODER_L_B)) encoderCountL++;
  else encoderCountL--;
}
void IRAM_ATTR encoderR_ISR() {
  if (digitalRead(ENCODER_R_A) == digitalRead(ENCODER_R_B)) encoderCountR++;
  else encoderCountR--;
}

void setup() {
  pinMode(ENCODER_L_A, INPUT_PULLUP);
  pinMode(ENCODER_L_B, INPUT_PULLUP);
  pinMode(ENCODER_R_A, INPUT_PULLUP);
  pinMode(ENCODER_R_B, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(ENCODER_L_A), encoderL_ISR, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENCODER_R_A), encoderR_ISR, CHANGE);
  Serial.begin(115200);
  Wire.begin(OLED_SDA, OLED_SCL);
  display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR);
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("ENCODER TEST");
  display.display();
  delay(1000);
}

void loop() {
  Serial.print("EncL: "); Serial.print(encoderCountL);
  Serial.print("\tEncR: "); Serial.println(encoderCountR);
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("EncL: "); display.println(encoderCountL);
  display.print("EncR: "); display.println(encoderCountR);
  display.display();
  delay(100);
}
