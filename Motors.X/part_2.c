#include "BOARD.h"
#include "RC_Servo.h"
#include "LED.h"
#include "AD.h"
#include "pwm.h"
#include <stdio.h>

int main (void) {
    BOARD_Init();
    LED_Init();
    AD_Init();
    RC_Init();
    PWM_Init();
    
    PWM_SetFrequency(PWM_DEFAULT_FREQUENCY);
    
    LED_AddBanks(LED_BANK1);
    LED_AddBanks(LED_BANK2);
    LED_AddBanks(LED_BANK3);
    
    AD_AddPins(AD_PORTV5);
    
    PWM_AddPins(PWM_PORTZ06);
    
    
    while(1){
        unsigned int pot = AD_ReadADPin(AD_PORTV5);
        //unsigned int output = AD_ReadADPin(AD_PORTV3);

        unsigned int pwm_width = ((pot * (MAX_PWM - MIN_PWM)) / 1023) + MIN_PWM;
        //printf("input: %d\n", pwm_width);
        PWM_SetDutyCycle(PWM_PORTZ06, pwm_width);

        //unsigned int test = PWM_GetDutyCycle(PWM_PORTZ06);
        //printf("output: %d\n", test);
        
        unsigned char leds = (pot * 12) / 1023;
        unsigned int pattern = (1 << leds) - 1;
        LED_SetBank(LED_BANK1, (pattern & 0x0F));
        LED_SetBank(LED_BANK2, (pattern >> 4) & 0x0F);
        LED_SetBank(LED_BANK3, (pattern >> 8) & 0x0F);


    }
}