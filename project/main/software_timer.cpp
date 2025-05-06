#include "software_timer.h"

int timer_counter[5];
int timer_flag[5];

void Set_Timer(int index, int value) {
    timer_counter[index] = value;
    timer_flag[index] = 0;
}

int Is_Timer_Expired(int index) {
    if (timer_flag[index] == 1) {
        timer_flag[index] = 0;
        return 1;
    }
    return 0;
} 

void Timer_Run() {
    for (int i = 0; i < 5; i++) {
        if (timer_counter[i] > 0) {
            timer_counter[i]--;
            if (timer_counter[i] == 0) {
                timer_flag[i] = 1;
            }
        }
    }
}