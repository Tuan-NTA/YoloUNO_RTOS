#include "Arduino.h"
#include <sensor.h>
#include <heater.h>
#include <software_timer.h>


int heater_status= init;
bool flag = true;

void temp_range(){
    
    if(temperature>18 && temperature<30){
        heater_status=safe;
    }
            
    if((temperature>10 && temperature<17) ||
        (temperature>31 && temperature<35)){
            heater_status=risky;
    }
            
            
    if(temperature<10||temperature>35){
        heater_status=danger;
    }

    flag=true;
    Set_Timer(3,500);
}

void task_heater(){
    switch (heater_status){
        case init:
            pinMode(D3, OUTPUT);
            pinMode(D4, OUTPUT);
            temp_range();
            break;

        case safe:
            if(flag){
                digitalWrite(D3,HIGH);
                digitalWrite(D4,LOW);
                flag=false;
            }
            
            
            if(Is_Timer_Expired(3)==1)
                temp_range();
            break;

        case risky:
            if(flag){
                digitalWrite(D3,LOW);
                digitalWrite(D4,HIGH);
                flag=false;
            }
            
            
            if(Is_Timer_Expired(3)==1)
                temp_range();
            break;

        case danger:
            if(flag){
                digitalWrite(D3,HIGH);
                digitalWrite(D4,HIGH);
                flag=false;
            }
            
            
            if(Is_Timer_Expired(3)==1)
                temp_range();
            break;
    }
}
