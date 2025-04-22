#include <sensor.h>
#include <software_timer.h>

#define D5 8
#define D6 9

#define init 0
#define active 1
#define temp_check 2

int cooler_status=init;

void task_cooler(){
    switch (cooler_status){
        case init:
            pinMode(D5, OUTPUT);
            pinMode(D6, OUTPUT);
            
            cooler_status=temp_check;
            break;
        case active:
            Set_Timer(500);
            digitalWrite(D5,HIGH);
            digitalWrite(D6,LOW);
            if(Is_Timer_Expired == 0){
                cooler_status= temp_check;
            }
        case temp_check:
            digitalWrite(D5,HIGH);
            digitalWrite(D6,HIGH);
            if(temperature>30)
                cooler_status=active;
            break;
    }
}
