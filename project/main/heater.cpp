#include "Arduino.h"
#include <sensor.h>
#include <heater.h>
#include <software_timer.h>


int heater_status= init;
bool flag = false;

void temp_range(){
    if(temperature>18 && temperature<30)
            heater_status=safe;

    if((temperature>10 && temperature<17) ||
        (temperature>31 && temperature<35))
            heater_status=risky;
            
    if(temperature<10||temperature>35)
            heater_status=danger;
}

void task_heater(){
    if(flag)
        temp_range();

    switch (heater_status){
        case init:
            pinMode(D3, OUTPUT);
            pinMode(D4, OUTPUT);
            flag= true;
            break;
        case safe:
            digitalWrite(D3,HIGH);
            digitalWrite(D4,LOW);
            break;
        case risky:
            digitalWrite(D3,LOW);
            digitalWrite(D4,HIGH);
            break;
        case danger:
            digitalWrite(D3,HIGH);
            digitalWrite(D4,HIGH);
            break;
    }
}
