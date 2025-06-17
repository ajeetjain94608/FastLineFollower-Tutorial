// Created by Ajeet Jain
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define MUX_S0 18
#define MUX_S1 19
#define MUX_S2 17
#define MUX_S3 5
#define MUX_SIG 34
#define NUM_SENSORS 16
#define OLED_SDA 21
#define OLED_SCL 22
#define OLED_RST -1
#define OLED_ADDR 0x3C
Adafruit_SSD1306 display(128, 64, &Wire, OLED_RST);

void setup() {
  pinMode(MUX_S0, OUTPUT);
  pinMode(MUX_S1, OUTPUT);
  pinMode(MUX_S2, OUTPUT);
  pinMode(MUX_S3, OUTPUT);
  pinMode(MUX_SIG, INPUT);
  Serial.begin(115200);
  Wire.begin(OLED_SDA, OLED_SCL);
  display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR);
  display.setRotation(2);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("MUX TEST");
  display.display();
  delay(1000);
}

void showAjeetJain() {
  display.setTextSize(1);
  display.setCursor(0, 54);
  display.println("Ajeet Jain");
}

void loop() {
  display.clearDisplay();
  display.setCursor(0, 0);
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
  showAjeetJain();
  display.display();
  delay(500);
}
