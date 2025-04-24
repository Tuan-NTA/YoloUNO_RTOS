#include "Arduino.h"
#include <sensor.h>
#include <software_timer.h>
#include <humidifier.h>


int humidifier_status = init;


void task_humidifier(){
    switch (humidifier_status){
        case init:
            pinMode(D7, OUTPUT);
            pinMode(D8, OUTPUT);

            humidifier_status= idle;
            break;

        case idle:
            if(humidity<40)
                humidifier_status= active_green;
            break;

        case active_green:
            if (Is_Timer_Expired(1) == 1){
                humidifier_status = active_yellow;
                break;
            } 
            Set_Timer(1, 500);
            digitalWrite(D7, HIGH);
            digitalWrite(D8,LOW);
            break;
        
        case active_yellow:
            if (Is_Timer_Expired(1) == 1){
                humidifier_status = active_red;
                break;
            } 
            Set_Timer(1, 300);
            digitalWrite(D7, LOW);
            digitalWrite(D8,HIGH);
            break;
        
        case active_red:
            if (Is_Timer_Expired(1) == 1){
                humidifier_status = idle;
                break;
            } 
            Set_Timer(1, 200);
            
            digitalWrite(D7, HIGH);
            digitalWrite(D8,HIGH);
            break;
    }
}
