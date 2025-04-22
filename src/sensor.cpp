#include <DHT20.h>

DHT20 dht;

float temperature=0;
float humidity=0;



void readSensor(){
  switch (state){
    case init:
      Serial.begin(115200);
      Wire.begin(GPIO_NUM_11, GPIO_NUM_12);
      dht.begin();
    case active:
      dht.read();

      temperature = dht.getTemperature();
      humidity = dht.getHumidity();
  
      if (isnan(temperature) || isnan(humidity)) {
        Serial.println("Failed to read from DHT sensor!");
        return;
      }  
    }
}

void printTnH(){
    Serial.print("Humidity: ");
    Serial.print(humidity);
    Serial.print("%  Temperature: ");
    Serial.print(temperature);
    Serial.println("°C");
  }
  

