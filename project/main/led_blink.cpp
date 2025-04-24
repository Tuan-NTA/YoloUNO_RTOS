#include "Arduino.h"
#include "software_timer.h"
#include "led_blink.h"

#define led GPIO_NUM_48

#define init 1
#define LED_on 2
#define LED_off 3

int led_status=init;

void taskLedBlinky(){
    switch (led_status){
        case init:
            pinMode(led, OUTPUT);
            led_status = LED_on;
            digitalWrite(led, HIGH);
            break;
    
        case LED_on:
            if(Is_Timer_Expired(0)==1){
                led_status = LED_off;
                break;
            }
            Set_Timer(0, 100);
            digitalWrite(led, HIGH);
            break;
    
        case LED_off:
            if(Is_Timer_Expired(0)==1){
                led_status = LED_off;
                break;
            }
            Set_Timer(0, 100);
            digitalWrite(led, LOW);
            break;
    }
}

