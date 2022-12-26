#include "DHT.h"
#include <MQ135.h>
#define PIN_MQ135 A2
const int DHTPIN = 7;
const int DHTTYPE = DHT11;
unsigned long sendDataPrevMillis = 0;
DHT dht(DHTPIN, DHTTYPE);

MQ135 mq135_sensor(PIN_MQ135);

float h, t;
int ppm;

void setup() {
  Serial.begin(9600);
  dht.begin();
}

void loop() {
  if (millis() - sendDataPrevMillis > 5000 || sendDataPrevMillis == 0) {
    sendDataPrevMillis = millis();
    dht11();
    mq135();
  }
}

void dht11() {
  h = dht.readHumidity();
  t = dht.readTemperature();
  // nhiet do c
  Serial.println(t);
  // do am
  Serial.println(h);
}

void mq135() {
  ppm = mq135_sensor.getCorrectedPPM(t, h);
  Serial.println(ppm);
}