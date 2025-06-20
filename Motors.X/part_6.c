#include "BOARD.h"
#include "RC_Servo.h"
#include "LED.h"
#include "AD.h"
#include "pwm.h"
#include "Stepper.h"
#include "IO_Ports.h"
#include <stdio.h>
#include <xc.h>

#define direction FORWARD

int main (void) {
    BOARD_Init();
    Stepper_Init();
    
    
    IO_PortsSetPortOutputs(PORTY, PIN8 | PIN9 | PIN11);
    
    //step
    //1 = stepping
    //0 = not stepping
    //IO_PortsWritePort(PORTY, PIN8);
    PORTY08_LAT = 1;
    //printf("writing to 8\n");
    
    
    //direction
    //1 = forward
    //0 = reverse
    //IO_PortsWritePort(PORTY, PIN9);
    PORTY09_LAT = direction;
    printf("writing to 9\n");
    
    
    //enable
    //1 = on
    //0 = off
    //IO_PortsWritePort(PORTY, PIN11);
    PORTY11_LAT = 1;
    printf("writing to 11\n");
    
    Stepper_SetRate(200);
    printf("starting steps\n");
    Stepper_InitSteps(direction, 400);
//    Stepper_InitSteps(REVERSE, 400);
    
    
}