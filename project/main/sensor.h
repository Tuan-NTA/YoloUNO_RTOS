#ifndef SENSOR_H
#define SENSOR_H

#include <DHT20.h>
#include <Wire.h>

#define init 0
#define active 1
#define idle 2

extern DHT20 dht;

extern float temperature;
extern float humidity;

void readSensor();
void printTnH();

#endif