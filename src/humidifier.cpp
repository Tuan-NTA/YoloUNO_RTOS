#include <sensor.h>
#include <software_timer.h>

#define D7 10
#define D8 17

#define init 0
#define idle 1
#define active_red 2
#define active_yellow 3
#define active_green 4

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
            if (Is_Timer_Expired() == 0) 
                break;
            Set_Timer(0, 500);
            humidifier_status = active_yellow;
            digitalWrite(D7, HIGH);
            digitalWrite(D8,LOW);
            break;
        
        case active_yellow:
            if (Is_Timer_Expired() == 0) 
                break;
            Set_Timer(0, 300);
            humidifier_status = active_red;
            digitalWrite(D7, LOW);
            digitalWrite(D8,HIGH);
            break;
        
        case active_red:
            if (Is_Timer_Expired() == 0) 
                break;
            Set_Timer(0, 200);
            humidifier_status = idle;
            digitalWrite(D7, HIGH);
            digitalWrite(D8,HIGH);
            break;
    }
}
