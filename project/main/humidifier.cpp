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
            if(humidity<40){
                Set_Timer(4, 500);
                humidifier_status= active_green;
                digitalWrite(D7, HIGH);
                digitalWrite(D8, LOW);
            }
                
            break;

        case active_green:
            if (Is_Timer_Expired(4) == 0){
                break;
            } 
            Set_Timer(4, 300);
            humidifier_status = active_yellow;
            digitalWrite(D7, LOW);
            digitalWrite(D8, HIGH);
            break;
        
        case active_yellow:
            if (Is_Timer_Expired(4) == 0){
                break;
            } 
            Set_Timer(4, 200);
            humidifier_status = active_red;
            digitalWrite(D7, HIGH);
            digitalWrite(D8, HIGH);
            break;
        
        case active_red:
            if (Is_Timer_Expired(4) == 0){
                break;
            } 
            humidifier_status = idle;
            digitalWrite(D7, LOW);
            digitalWrite(D8, LOW);
            break;
    }
}
