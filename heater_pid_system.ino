#include <Wire.h>
#include <U8x8lib.h>
#include <math.h>
#include <PID_v1.h>

// ================= OLED =================
// Arduino Uno: SDA = A4, SCL = A5
U8X8_SSD1306_128X64_NONAME_HW_I2C u8x8(U8X8_PIN_NONE);

// ================= PINS =================
const byte SAVE_BTN   = 2;   // D2 = Save / Fire
const byte INC_BTN    = 4;   // D4 = Increment
const byte CURSOR_BTN = 7;   // D7 = Cursor

const byte heaterPin = 5;    // D5 = Heater PWM
const byte tempPin   = A0;   // A0 = Present temperature thermistor
const byte THERM_PIN = A1;   // A1 = Interfacing resistance voltage

// ================= SETPOINT VARIABLES =================
int permanentTemp = 90;
int tempVariable  = 90;

// 0 = Hundreds, 1 = Tens, 2 = Ones
byte cursorPosition = 1;

// ================= BUTTON VARIABLES =================
bool lastSave   = HIGH;
bool lastInc    = HIGH;
bool lastCursor = HIGH;

unsigned long lastSaveTime   = 0;
unsigned long lastIncTime    = 0;
unsigned long lastCursorTime = 0;

const unsigned long debounceDelay = 200;

// ================= PID VARIABLES =================
double Setpoint = 90.0;
double Input;
double Output;

double Kp = 19;
double Ki = 0.6;
double Kd = 3.5;

PID myPID(&Input, &Output, &Setpoint, Kp, Ki, Kd, DIRECT);

// ================= THERMISTOR PARAMETERS =================
const float seriesResistor = 10000.0;
const float nominalResistance = 100000.0;
const float nominalTemp = 25.0;
const float beta = 3900.0;

// ================= READINGS =================
float presentTemperature = 0;
float thermistorResistance = 0;

// ================= TIMING =================
unsigned long lastDisplayTime = 0;
const unsigned long displayDelay = 500;

void setup() {
  pinMode(SAVE_BTN, INPUT_PULLUP);
  pinMode(INC_BTN, INPUT_PULLUP);
  pinMode(CURSOR_BTN, INPUT_PULLUP);

  pinMode(heaterPin, OUTPUT);
  analogWrite(heaterPin, 0);

  u8x8.begin();

  lastSave   = digitalRead(SAVE_BTN);
  lastInc    = digitalRead(INC_BTN);
  lastCursor = digitalRead(CURSOR_BTN);

  Setpoint = permanentTemp;

  myPID.SetMode(AUTOMATIC);
  myPID.SetOutputLimits(0, 255);

  updateDisplay();
}

void loop() {
  presentTemperature = readTemperature();

  readResistance();

  Input = presentTemperature;
  Setpoint = permanentTemp;

  myPID.Compute();
  analogWrite(heaterPin, (int)Output);

  // D7 = CURSOR
  if (buttonPressed(CURSOR_BTN, lastCursor, lastCursorTime)) {
    cursorPosition++;

    if (cursorPosition > 2) {
      cursorPosition = 0;
    }

    updateDisplay();
  }

  // D4 = INCREMENT
  if (buttonPressed(INC_BTN, lastInc, lastIncTime)) {
    int h = tempVariable / 100;
    int t = (tempVariable / 10) % 10;
    int o = tempVariable % 10;

    if (cursorPosition == 0) {
      h = (h + 1) % 3;     // hundreds: 0,1,2
    }
    else if (cursorPosition == 1) {
      t = (t + 1) % 10;
    }
    else {
      o = (o + 1) % 10;
    }

    tempVariable = h * 100 + t * 10 + o;

    updateDisplay();
  }

  // D2 = SAVE / FIRE
  if (buttonPressed(SAVE_BTN, lastSave, lastSaveTime)) {
    permanentTemp = tempVariable;
    Setpoint = permanentTemp;

    updateDisplay();
  }

  if (millis() - lastDisplayTime >= displayDelay) {
    lastDisplayTime = millis();
    updateDisplay();
  }
}

float readTemperature() {
  long total = 0;

  for (int i = 0; i < 20; i++) {
    total += analogRead(tempPin);
  }

  float adcValue = total / 20.0;

  if (adcValue <= 0) {
    adcValue = 1;
  }

  if (adcValue >= 1023) {
    adcValue = 1022;
  }

  float resistance = seriesResistor / ((1023.0 / adcValue) - 1.0);

  float steinhart;
  steinhart = resistance / nominalResistance;
  steinhart = log(steinhart);
  steinhart = steinhart / beta;
  steinhart = steinhart + (1.0 / (nominalTemp + 273.15));
  steinhart = 1.0 / steinhart;
  steinhart = steinhart - 273.15;

  return steinhart;
}

void readResistance() {
  long adcSum = 0;

  for (int i = 0; i < 100; i++) {
    adcSum += analogRead(THERM_PIN);
  }

  float adcValue = adcSum / 100.0;

  if (adcValue <= 0) {
    adcValue = 1;
  }

  if (adcValue >= 1023) {
    adcValue = 1022;
  }

  float voltage = adcValue * (5.0 / 1023.0);

  // Your interfacing formula
  thermistorResistance = voltage * (100000.0 + (47000.0 / 2.0)) / 5.0;
}

bool buttonPressed(byte pin, bool &lastState, unsigned long &lastTime) {
  bool currentState = digitalRead(pin);

  if (lastState == HIGH && currentState == LOW) {
    if (millis() - lastTime > debounceDelay) {
      lastTime = millis();
      lastState = currentState;
      return true;
    }
  }

  lastState = currentState;
  return false;
}

void updateDisplay() {
  int h = tempVariable / 100;
  int t = (tempVariable / 10) % 10;
  int o = tempVariable % 10;

  u8x8.clearDisplay();

  // Big font for main readings
  u8x8.setFont(u8x8_font_8x13_1x2_r);

  // Present temperature
  u8x8.setCursor(0, 0);
  u8x8.print(F("  T:"));
  u8x8.print((int)round(presentTemperature));
  u8x8.print(F("C"));

  // Resistance
  u8x8.setCursor(0, 2);
  u8x8.print(F("  R:"));
  u8x8.print((long)thermistorResistance);

  // Saved setpoint
  u8x8.setCursor(0, 4);
  u8x8.print(F("  SP:"));
  print3Digit(permanentTemp);
  u8x8.print(F("C"));

  // Small font for variable and cursor
  u8x8.setFont(u8x8_font_chroma48medium8_r);

  // Editable variable with 2 spaces at start
  u8x8.setCursor(2, 6);
  u8x8.print(F("V:"));
  u8x8.print(h);
  u8x8.print(t);
  u8x8.print(o);
  u8x8.print(F("C"));

  // Cursor below selected digit
  u8x8.setCursor(4 + cursorPosition, 7);
  u8x8.print(F("^"));
}

void print3Digit(int value) {
  int h = value / 100;
  int t = (value / 10) % 10;
  int o = value % 10;

  u8x8.print(h);
  u8x8.print(t);
  u8x8.print(o);
}
