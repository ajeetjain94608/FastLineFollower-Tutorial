// Created by Ajeet Jain
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
// OLED pins only
#define OLED_SDA    21
#define OLED_SCL    22
#define OLED_RST    -1
#define OLED_ADDR   0x3C
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RST);

void showAjeetJain() {
  display.setTextSize(1);
  display.setCursor(0, SCREEN_HEIGHT - 10);
  display.println("Ajeet Jain");
}

void setup() {
  Wire.begin(OLED_SDA, OLED_SCL);
  display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR);
  display.setRotation(2);
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("OLED TEST");
  display.setTextSize(1);
  display.setCursor(0, 24);
  display.println("128x64 SSD1306");
  display.setCursor(0, 40);
  display.println("Hello, ESP32!");
  display.display();
  showAjeetJain();
}

void loop() {
  // Nothing to do, static test
}
