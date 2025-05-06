#include "Arduino.h"
#include "sensor.h"
#include "software_timer.h"
#include "cooler.h"

int cooler_status=init;

void task_cooler(){
    switch (cooler_status){
      case init:
        pinMode(D5, OUTPUT);
        pinMode(D6, OUTPUT);
        
        cooler_status=temp_check;
        break;

      case active:
        if(Is_Timer_Expired(2) == 0){
          break;
        }
        
        cooler_status= temp_check;
        digitalWrite(D5,HIGH);
        digitalWrite(D6,HIGH);
        break;

      case temp_check:
        if(temperature>30){
          cooler_status=active;
          Set_Timer(2,500);
          digitalWrite(D5,HIGH);
          digitalWrite(D6,LOW);
        }
        break;
    }
}
