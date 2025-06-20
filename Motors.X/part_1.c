#include "BOARD.h"
#include "RC_Servo.h"
#include "LED.h"
#include "AD.h"
#include <stdio.h>

int main (void) {
    BOARD_Init();
    LED_Init();
    AD_Init();
    RC_Init();
    
    LED_AddBanks(LED_BANK1);
    LED_AddBanks(LED_BANK2);
    LED_AddBanks(LED_BANK3);
    
    AD_AddPins(AD_PORTV5);
    RC_AddPins(RC_PORTV04);
    
    while(1){
        unsigned int pot = AD_ReadADPin(AD_PORTV5);
        //unsigned int output = AD_ReadADPin(AD_PORTV3);

        unsigned int pulse_width = ((pot * (MAXPULSE - MINPULSE)) / 1024) + MINPULSE;


        //printf("input: %d\n", pulse_width);

        RC_SetPulseTime(RC_PORTV04, pulse_width);

        short int get_time = RC_GetPulseTime(RC_PORTV04);
        //printf("output: %d\n", get_time);

        unsigned char leds = (pot * 12) / 1023;
        unsigned int pattern = (1 << leds) - 1;
        LED_SetBank(LED_BANK1, (pattern & 0x0F));
        LED_SetBank(LED_BANK2, (pattern >> 4) & 0x0F);
        LED_SetBank(LED_BANK3, (pattern >> 8) & 0x0F);


    }
}