#include "Arduino.h"
#include "DHT20.h"
#include "sensor.h"
#include "software_timer.h"

DHT20 dht;

float temperature=0;
float humidity=0;

int sensor_status= init;

void readSensor(){
  switch (sensor_status){
    case init:
      Serial.begin(115200);
      Wire.begin(GPIO_NUM_11, GPIO_NUM_12);
      dht.begin();
      sensor_status= active;
      break;

    case active:
      dht.read();
      temperature = dht.getTemperature();
      humidity = dht.getHumidity();

      printTnH();

      if (isnan(temperature) || isnan(humidity)) {
        Serial.println("Failed to read from DHT sensor!");
        break;
      }  
      sensor_status=idle;
      Set_Timer(1,500);
      break;
      
    case idle:
      if(Is_Timer_Expired(1) == 0)
        break;
      sensor_status=active;
    }
}

void printTnH(){
    Serial.print("Humidity: ");
    Serial.print(humidity);
    Serial.print("%  Temperature: ");
    Serial.print(temperature);
    Serial.println("°C");
}
  

