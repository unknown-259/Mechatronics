/*
Author:Neili Hu
Date:May.19.2025
version:0.2
*/
#include <stdlib.h>
#include <stdio.h>
#include "personal_config.h"
#include "pwm.h"
#include "RC_servo.h"
#include "timers.h"
#include "AD.h"
#include "IO_Ports.h"
#include <time.h>
#include "Stepper.h"
#include <xc.h>

static uint8_t led_rec = 0;
static int j = 0;
int last3 = 1;//at begining position shall be in 1
void delay(int set){
    for (j=0;j<set;j++);
}

void init_config(void){
    //IO_PortsSetPortOutputs(PORTY, PIN6);
    IO_PortsSetPortOutputs(PORTW, PIN7);
    IO_PortsSetPortOutputs(PORTZ, PIN3|PIN4|PIN5|PIN11|PIN7|PIN8|PIN9);

    PWM_Init();
    PWM_AddPins(DC_PIN_1|STEPPER_EN|STEPPER_DIR|STEPPER_PULSE);
    PWM_SetFrequency(400);//fixed since here
    PWM_SetDutyCycle(DC_PIN_1, 0);
    PWM_SetDutyCycle(STEPPER_EN, 0);
    PWM_SetDutyCycle(STEPPER_DIR, 0);
    PWM_SetDutyCycle(STEPPER_PULSE, 0);
    Stepper_Init();
    
    RC_Init();
    RC_AddPins(SERVO_PIN_1|SERVO_PIN_2);
    RC_SetPulseTime(SERVO_PIN_1, 1000);
    RC_SetPulseTime(SERVO_PIN_2, SERVO_STOP);
    
    AD_Init();
    AD_AddPins(IR_PIN_1);
    AD_AddPins(IR_PIN_2);
    AD_AddPins(IR_PIN_3);
    AD_AddPins(IR_PIN_4);
    IO_PortsSetPortInputs(PORTX, PIN5|PIN6);
}

void led_show(int port, uint16_t effect){
    if (effect == on){
        if (port == LED_1){
            IO_PortsSetPortBits(PORTZ, PIN3);
        } else if (port == LED_2){
            IO_PortsSetPortBits(PORTZ, PIN4);
        } else if (port == LED_3){
            IO_PortsSetPortBits(PORTZ, PIN5);
        } else if (port == LED_4){
            IO_PortsSetPortBits(PORTZ, PIN11);
        } else {
            printf("wrong led_show on\n");
        }
    } else if (effect == off){
        if (port == LED_1){
            IO_PortsClearPortBits(PORTZ, PIN3);
        } else if (port == LED_2){
            IO_PortsClearPortBits(PORTZ, PIN4);
        } else if (port == LED_3){
            IO_PortsClearPortBits(PORTZ, PIN5);
        } else if (port == LED_4){
            IO_PortsClearPortBits(PORTZ, PIN11);
        } else {
            printf("wrong led_show off\n");
        }
    }
}
void led_record(uint16_t effect){
    if (effect == add){
        if (led_rec == 0){
            IO_PortsSetPortBits(PORTZ, PIN7);
            led_rec = 1;
        } else if (led_rec == 1){
            IO_PortsSetPortBits(PORTZ, PIN8);
            led_rec = 2;
        } else if (led_rec == 2){
            IO_PortsSetPortBits(PORTZ, PIN9);
            led_rec = 3;
        }
    } else if (effect == mute){
        IO_PortsClearPortBits(PORTZ, PIN7|PIN8|PIN9);
        led_rec = 0;
    }
}
void stepper_control(uint16_t effect, uint16_t dir, int frequency, int steps){
    int temp =-1;

    if (dir == clocks){
        PWM_SetDutyCycle(STEPPER_DIR, 0);
    } else if (dir == counter_clocks){
        PWM_SetDutyCycle(STEPPER_DIR, 1000);
    }
    
    temp = (effect == off)? 0:50;
    PWM_SetDutyCycle(STEPPER_PULSE, temp);
    Stepper_InitSteps(FORWARD, steps);
    if (dir == clocks){
        for(j=0;j<3550*steps;j++);
    } else{
        for(j=0;j<10000*steps;j++);//directly delay for each step
    }
}

void stepper_off(void){
    PWM_SetDutyCycle(STEPPER_PULSE, 0);//or set to 1000
}

void dc_control(uint16_t effect, uint8_t mode){
    if (effect == off){
        PWM_SetDutyCycle(DC_PIN_1, 0);
    } else {
        if (mode == 1){
            PWM_SetDutyCycle(DC_PIN_1, 800);
        } else if (mode == 2){
            PWM_SetDutyCycle(DC_PIN_1, 800);
        } else if (mode == 3){
            PWM_SetDutyCycle(DC_PIN_1, 900);
        } else if (mode == 4){
            PWM_SetDutyCycle(DC_PIN_1, 900);
        }
    }
    
//    if (effect == on){
//        IO_PortsSetPortBits(PORTY, PIN6);
//    } else if (effect == off){
//        IO_PortsClearPortBits(PORTY, PIN6);
//    }
}

void servo_control(unsigned short int pin, unsigned short int num){
    RC_SetPulseTime(pin, num);
}

void seven_seg_control(uint16_t effect){
    if (effect == off){
        IO_PortsSetPortBits(PORTW, PIN7);
    } else if (effect == on){
        IO_PortsClearPortBits(PORTW, PIN7);
    }
}

void shutdown(void){
    //close leds
    led_show(LED_1, off);
    led_show(LED_2, off);
    led_show(LED_3, off);
    led_show(LED_4, off);
    led_record(mute);
    
    //close dc motor
    dc_control(off, 0);
    
    //close seven segments display
    seven_seg_control(off);

    //close stepper
    stepper_off();

    //close servo pins
    RC_End();

    //AD pins do not need to be terminated.
    
    //close timer(s)
    for (uint8_t k =0; k<5; k++){
        ES_Timer_StopTimer(k);
    }
}

void recover(void){
    RC_Init();
    RC_AddPins(SERVO_PIN_1|SERVO_PIN_2);
    RC_SetPulseTime(SERVO_PIN_2, 1500);
    RC_SetPulseTime(SERVO_PIN_1, SERVO_STOP);
    
}

uint8_t tunnel_gen(void){
    static uint8_t last = 0;
    static uint8_t ok = 0;
    srand(time(NULL));
    while(ok==last){
        ok = rand()%4+1;
    }
    if (ok == 3){
        ok = 4;
    }
    last = ok;
    return ok;
}

void led_num_show(uint8_t num, uint16_t effect){
    if (num == 1){
        led_show(LED_1, effect);
    } else if (num == 2){
        led_show(LED_2, effect);
    } else if (num == 3){
        led_show(LED_3, effect);
    } else if (num == 4){
        led_show(LED_4, effect);
    }
}

void actuator_control(uint8_t num){
    static last3 = 1;//at begining position shall be in 1
    if (num == 1){
        servo_control(SERVO_PIN_2, SERVO_POS_1);
        if (last3 == 2){
            stepper_control(on, counter_clocks, 400, 29);
        } else if (last3 == 3){
            stepper_control(on, counter_clocks, 400, 29);
        } else if (last3 == 4){
            stepper_control(on, counter_clocks, 400, 29);
        } 
    } else if (num == 2){
        servo_control(SERVO_PIN_2, SERVO_POS_2);
        if (last3 == 1){
            stepper_control(on, clocks, 400, 28);
        } else if (last3 == 5){
            stepper_control(on, clocks, 400, 28);
        }
    } else if (num == 3){
        servo_control(SERVO_PIN_2, SERVO_POS_3);
        if (last3 == 1){
            stepper_control(on, clocks, 400, 28);
        } else if (last3 == 5){
            stepper_control(on, clocks, 400, 28);
        }
    } else if (num == 4){
        servo_control(SERVO_PIN_2, SERVO_POS_4);
        if (last3 == 1){
            stepper_control(on, clocks, 400, 28);
        } else if (last3 == 5){
            stepper_control(on, clocks, 400, 28);
        }
    } else if (num == 5){
        servo_control(SERVO_PIN_2, 1500);
        if (last3 == 2){
            stepper_control(on, counter_clocks, 400, 29);
        } else if (last3 == 3){
            stepper_control(on, counter_clocks, 400, 29);
        } else if (last3 == 4){
            stepper_control(on, counter_clocks, 400, 29);
        } 
    }
    for(j=0;j<200000;j++);//delay for servo and it test
    last3 = num;
}


void last3_change(void){
    last3=1;
}
void shake(uint16_t effect){
    if (effect == on){
        dc_control(on, DC_POS_4);
        for(j=0;j<1000;j++){
            stepper_control(on, clocks, 400, 4);
            stepper_control(on, counter_clocks, 400, 4);
        }
        //shaking step motor;
    } else if (effect == off){
        dc_control(off, 0);
        stepper_control(off, clocks, 1, 0);
    }
}

void set_58_timer(void){
    ES_Timer_InitTimer(timer_58, 58000);
}
void set_shaking_timer(void){
    ES_Timer_InitTimer(shake_timer, 3000);
}
void set_end_of_game_timer(void){
    ES_Timer_InitTimer(end_of_game_timer, 2300);
}
void set_watch_dog(void){
    ES_Timer_InitTimer(watching_dog_timer, 30000);
}
void set_raise_timer(void){
    ES_Timer_InitTimer(raise_timer, 10000);
}
void set_protection_timer(void){
    ES_Timer_InitTimer(protection_timer, 100);
}