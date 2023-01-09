#include <Arduino.h>
#include <avr/sleep.h>

#define PIN_TRIGGER  2
#define PIN_LED      8

#define LIGHT_TIME   5 // in s
#define lmillis() ((long)millis())

bool isLedLit;
byte tiltStatus = LOW, lastTiltStatus = LOW;
int ledTimer = 0;
long lastAction = 0;

void isrAwake(void)
{
    detachInterrupt(0);
}

void enterSleepMode(void)
{
    attachInterrupt(digitalPinToInterrupt(PIN_TRIGGER), isrAwake, LOW);
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    sleep_enable();
    sleep_mode();
    sleep_disable();
}

void lightLED()
{
    digitalWrite(PIN_LED, HIGH);
    isLedLit = true;
    ledTimer = LIGHT_TIME;
}

void setup()
{
    pinMode(PIN_TRIGGER, INPUT_PULLUP);
    pinMode(PIN_LED, OUTPUT);

    pinMode(13, OUTPUT);
    digitalWrite(13, LOW);

    lightLED();
    lastAction = lmillis();
}

void loop()
{
    if (isLedLit && ledTimer <= 0) {
        digitalWrite(PIN_LED, LOW);
        isLedLit = false;
        enterSleepMode();
    }

    tiltStatus = digitalRead(PIN_TRIGGER);
    if (tiltStatus != lastTiltStatus && ledTimer < LIGHT_TIME) {
        lastTiltStatus = tiltStatus;
        lightLED();
    }

    if (ledTimer > 0 && lmillis() - lastAction >= 0) {
        lastAction = lmillis() + 1000;
        ledTimer--;
    }
}