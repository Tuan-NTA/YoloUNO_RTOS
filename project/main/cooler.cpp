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
        Set_Timer(1,500);
        digitalWrite(D5,HIGH);
        digitalWrite(D6,LOW);
        
        if(Is_Timer_Expired(1) == 0)
          cooler_status= temp_check;
      
        break;
      case temp_check:
        digitalWrite(D5,HIGH);
        digitalWrite(D6,HIGH);
        if(temperature>30)
          cooler_status=active;
        break;
    }
}
