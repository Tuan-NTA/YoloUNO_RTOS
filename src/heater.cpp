#include <sensor.h>

#define D3 6
#define D4 7

#define init 0
#define safe 1
#define risky 2
#define danger 3

int heater_status= init;

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
    switch (heater_status){
        case init:
            pinMode(D3, OUTPUT);
            pinMode(D4, OUTPUT);

            temp_range();
            break;
        case safe:
            digitalWrite(D3,HIGH);
            digitalWrite(D4,LOW);

            temp_range();
            break;
        case risky:
            digitalWrite(D3,LOW);
            digitalWrite(D4,HIGH);

            temp_range();
            break;
        case danger:
            digitalWrite(D3,HIGH);
            digitalWrite(D4,HIGH);
            
            temp_range();
            break;
    }
}
