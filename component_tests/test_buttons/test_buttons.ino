// Created by Ajeet Jain
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
// Button pins only
#define BTN_START   4
#define BTN_CAL     15
#define BTN_MODE    23
#define BTN_RESET   0
// OLED pins
#define OLED_SDA    21
#define OLED_SCL    22
#define OLED_RST    -1
#define OLED_ADDR   0x3C
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RST);

void setup() {
  pinMode(BTN_START, INPUT_PULLUP);
  pinMode(BTN_CAL, INPUT_PULLUP);
  pinMode(BTN_MODE, INPUT_PULLUP);
  pinMode(BTN_RESET, INPUT_PULLUP);
  Serial.begin(115200);
  Wire.begin(OLED_SDA, OLED_SCL);
  display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR);
  display.setRotation(2);
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("BTN TEST");
  display.display();
  delay(1000);
}

void showAjeetJain() {
  display.setTextSize(1);
  display.setCursor(0, SCREEN_HEIGHT - 10);
  display.println("Ajeet Jain");
}

void loop() {
  Serial.print("START: "); Serial.print(digitalRead(BTN_START) == LOW ? "PRESSED" : "RELEASED");
  Serial.print("\tCAL: "); Serial.print(digitalRead(BTN_CAL) == LOW ? "PRESSED" : "RELEASED");
  Serial.print("\tMODE: "); Serial.print(digitalRead(BTN_MODE) == LOW ? "PRESSED" : "RELEASED");
  Serial.print("\tRESET: "); Serial.println(digitalRead(BTN_RESET) == LOW ? "PRESSED" : "RELEASED");
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("START: "); display.println(digitalRead(BTN_START) == LOW ? "PRESSED" : "RELEASED");
  display.print("CAL:   "); display.println(digitalRead(BTN_CAL) == LOW ? "PRESSED" : "RELEASED");
  display.print("MODE:  "); display.println(digitalRead(BTN_MODE) == LOW ? "PRESSED" : "RELEASED");
  display.print("RESET: "); display.println(digitalRead(BTN_RESET) == LOW ? "PRESSED" : "RELEASED");
  showAjeetJain();
  display.display();
  delay(100);
}
