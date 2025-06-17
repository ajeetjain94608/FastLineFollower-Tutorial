// Created by Ajeet Jain
/*
  WiFi RC Car Example for ESP32
  - Controls motors via WiFi (web interface)
  - Uses same motor pinout as fastlinefollower.ino
  - Simple web page for forward, backward, left, right, stop
*/

#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Motor pins (from fastlinefollower.ino)
#define MOTOR_R_PWM 13
#define MOTOR_R_IN1 14
#define MOTOR_R_IN2 12
#define MOTOR_STBY  27
#define MOTOR_L_IN1 25
#define MOTOR_L_IN2 26
#define MOTOR_L_PWM 33

// OLED pins and config
#define OLED_SDA    21
#define OLED_SCL    22
#define OLED_RST    -1
#define OLED_ADDR   0x3C
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RST);

const char* ssid = "ESP32-RC-CAR-Ajeet-Jain";
const char* password = "12345678";

WebServer server(80);

int motorSpeed = 200; // Default speed

void showOledMessage(const char* msg) {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println(msg);
  display.setTextSize(1);
  display.setCursor(0, SCREEN_HEIGHT - 10);
  display.println("Ajeet Jain");
  display.display();
}

void showOledAnimation() {
  display.clearDisplay();
  for (int i = 0; i < SCREEN_WIDTH; i += 8) {
    display.clearDisplay();
    display.fillCircle(i, 32, 8, SSD1306_WHITE);
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, SCREEN_HEIGHT - 10);
    display.println("Ajeet Jain");
    display.display();
    delay(30);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, SCREEN_HEIGHT - 10);
  display.println("Ajeet Jain");
  display.display();
}

void setMotors(int left, int right) {
  digitalWrite(MOTOR_L_IN1, left > 0);
  digitalWrite(MOTOR_L_IN2, left < 0);
  digitalWrite(MOTOR_R_IN1, right > 0);
  digitalWrite(MOTOR_R_IN2, right < 0);
  ledcWrite(MOTOR_L_PWM, abs(left));
  ledcWrite(MOTOR_R_PWM, abs(right));
}

void handleRoot() {
  String html = R"rawliteral(
  <html><head><title>ESP32 RC Car</title>
  <style>
    body { font-family: sans-serif; text-align: center; }
    .dpad { display: inline-block; margin: 20px auto; }
    .row { display: flex; justify-content: center; }
    .dpad-btn {
      width: 70px; height: 70px; margin: 5px;
      font-size: 1.2em; border-radius: 15px; border: 2px solid #333;
      background: #eee; transition: background 0.2s;
      box-shadow: 2px 2px 8px #aaa;
      user-select: none; -webkit-user-select: none; -ms-user-select: none;
      -webkit-tap-highlight-color: transparent;
      outline: none;
    }
    .dpad-btn:active { background: #b3e5fc; }
    .slider { width: 200px; }
  </style>
  </head><body>
  <h2>WiFi RC Car Control</h2>
  <div class='dpad'>
    <div class='row'>
      <button class='dpad-btn' id='fwd'>▲</button>
    </div>
    <div class='row'>
      <button class='dpad-btn' id='left'>◀</button>
      <button class='dpad-btn' id='stop'>■</button>
      <button class='dpad-btn' id='right'>▶</button>
    </div>
    <div class='row'>
      <button class='dpad-btn' id='bwd'>▼</button>
    </div>
  </div>
  <form id='speedForm' action='/speed' method='POST'>
    Speed: <input class='slider' type='range' name='value' min='0' max='255' value='" + String(motorSpeed) + "' oninput='this.nextElementSibling.value=this.value'><output>" + String(motorSpeed) + "</output>
    <button type='submit'>Set Speed</button>
  </form>
  <br><form action='/oled_msg' method='POST'><input name='msg' value='Hello!'><button type='submit'>Show OLED Msg</button></form>
  <form action='/oled_anim' method='POST'><button type='submit'>Run OLED Animation</button></form>
  <script>
    function sendCmd(path) {
      fetch(path, {method: 'POST'});
    }
    function setupBtn(id, cmd) {
      const btn = document.getElementById(id);
      btn.addEventListener('mousedown', (e) => { e.preventDefault(); sendCmd(cmd); });
      btn.addEventListener('touchstart', (e) => { e.preventDefault(); sendCmd(cmd); });
      btn.addEventListener('mouseup', (e) => { e.preventDefault(); sendCmd('/stop'); });
      btn.addEventListener('mouseleave', (e) => { e.preventDefault(); sendCmd('/stop'); });
      btn.addEventListener('touchend', (e) => { e.preventDefault(); sendCmd('/stop'); });
      btn.addEventListener('selectstart', (e) => { e.preventDefault(); });
    }
    setupBtn('fwd', '/fwd');
    setupBtn('bwd', '/bwd');
    setupBtn('left', '/left');
    setupBtn('right', '/right');
    setupBtn('stop', '/stop');
  </script>
  </body></html>
  )rawliteral";
  server.send(200, "text/html", html);
}

void handleFwd()    { setMotors(motorSpeed, motorSpeed); handleRoot(); }
void handleBwd()    { setMotors(-motorSpeed, -motorSpeed); handleRoot(); }
void handleLeft()   { setMotors(-motorSpeed * 0.75, motorSpeed * 0.75); handleRoot(); }
void handleRight()  { setMotors(motorSpeed * 0.75, -motorSpeed * 0.75); handleRoot(); }
void handleStop()   { setMotors(0, 0); handleRoot(); }

void handleSpeed() {
  if (server.hasArg("value")) {
    motorSpeed = constrain(server.arg("value").toInt(), 0, 255);
  }
  handleRoot();
}

void handleOledMsg() {
  String msg = server.hasArg("msg") ? server.arg("msg") : "Hello!";
  showOledMessage(msg.c_str());
  handleRoot();
}

void handleOledAnim() {
  showOledAnimation();
  handleRoot();
}

void setup() {
  // Motor pins
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

  // OLED setup
  Wire.begin(OLED_SDA, OLED_SCL);
  display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR);
  display.setRotation(2);
  display.clearDisplay();
  display.display();
  showOledMessage("WiFi RC Car");

  // WiFi setup
  WiFi.softAP(ssid, password);
  IPAddress IP = WiFi.softAPIP();
  Serial.begin(115200);
  Serial.print("AP IP address: ");
  Serial.println(IP);

  // Web server routes
  server.on("/", HTTP_ANY, handleRoot);
  server.on("/fwd", HTTP_POST, handleFwd);
  server.on("/bwd", HTTP_POST, handleBwd);
  server.on("/left", HTTP_POST, handleLeft);
  server.on("/right", HTTP_POST, handleRight);
  server.on("/stop", HTTP_POST, handleStop);
  server.on("/speed", HTTP_POST, handleSpeed);
  server.on("/oled_msg", HTTP_POST, handleOledMsg);
  server.on("/oled_anim", HTTP_POST, handleOledAnim);
  server.begin();
}

void loop() {
  server.handleClient();
}
