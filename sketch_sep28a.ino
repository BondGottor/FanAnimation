#include <TimerOne.h>

const int ledPin = 9;
const int BLADES = 4;

const float FLASH_FREQ_HZ = 47.0;
const int HOLD_FLASHES = 5;   // базовых вспышек до одного удлинённого периода

volatile unsigned int flashCounter = 0;
volatile bool nextIsLong = false;   // true = следующий период удлинённый

unsigned long basePeriod_us;
unsigned long longPeriod_us;

void flash() {
  digitalWrite(ledPin, HIGH);
  delayMicroseconds(50);
  digitalWrite(ledPin, LOW);

  if (nextIsLong) {
    // Только что прошла удлинённая вспышка — возвращаемся к базовому периоду
    Timer1.setPeriod(basePeriod_us);
    nextIsLong = false;
    flashCounter = 0;
    return;
  }

  flashCounter++;

  if (flashCounter >= HOLD_FLASHES) {
    // Следующий период будет удлинённым — ровно один раз
    Timer1.setPeriod(longPeriod_us);
    nextIsLong = true;
    flashCounter = 0;
  }
}

void setup() {
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);

  basePeriod_us = (unsigned long)(1000000.0 / FLASH_FREQ_HZ);
  unsigned long shift_us = basePeriod_us / (BLADES * 2); // 1/16 оборота
  longPeriod_us = basePeriod_us + shift_us;

  Timer1.initialize(basePeriod_us);
  Timer1.attachInterrupt(flash);
}

void loop() {
  static unsigned long lastPrint = 0;
  if (millis() - lastPrint > 500) {
    lastPrint = millis();
    noInterrupts();
    unsigned int cnt = flashCounter;
    bool nl = nextIsLong;
    interrupts();

    Serial.print("base = ");
    Serial.print(basePeriod_us);
    Serial.print(" us | long = ");
    Serial.print(longPeriod_us);
    Serial.print(" us | cnt = ");
    Serial.print(cnt);
    Serial.print("/");
    Serial.print(HOLD_FLASHES);
    Serial.print(" | nextLong = ");
    Serial.println(nl ? "yes" : "no");
  }
}