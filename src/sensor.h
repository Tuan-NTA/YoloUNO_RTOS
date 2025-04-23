#ifndef SENSOR_H
#define SENSOR_H

#include <DHT20.h>
#include <Wire.h>

extern DHT20 dht;

extern float temperature;
extern float humidity;

void readSensor();
void printTnH();

#endif