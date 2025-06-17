// Created by Ajeet Jain
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
// Buzzer pin only
#define BUZZER_PIN 2
// OLED pins
#define OLED_SDA    21
#define OLED_SCL    22
#define OLED_RST    -1
#define OLED_ADDR   0x3C
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RST);

void setup() {
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
  Wire.begin(OLED_SDA, OLED_SCL);
  display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR);
  display.setRotation(2);
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("BUZZER TEST");
  showAjeetJain();
  display.display();
  delay(1000);
}

void loop() {
  digitalWrite(BUZZER_PIN, HIGH);
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("BUZZER ON");
  showAjeetJain();
  display.display();
  delay(100);
  digitalWrite(BUZZER_PIN, LOW);
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("BUZZER OFF");
  showAjeetJain();
  display.display();
  delay(900);
}

void showAjeetJain() {
  display.setTextSize(1);
  display.setCursor(0, SCREEN_HEIGHT - 10);
  display.println("Ajeet Jain");
}
