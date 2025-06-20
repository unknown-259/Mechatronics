#include "BOARD.h"
#include "RC_Servo.h"
#include "LED.h"
#include "AD.h"
#include "pwm.h"
#include "Stepper.h"
#include <stdio.h>

int main (void) {
    BOARD_Init();
    Stepper_Init();
    
    
    //Stepper_SetRate(10);
    Stepper_InitSteps(FORWARD, 200);
    //Stepper_InitSteps(REVERSE, 200);
    
}