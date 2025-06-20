/*
Author:Neili Hu
Date:May.15.2025
version:0.1
*/
#ifndef PERSONAL_CONFIG_H
#define PERSONAL_CONFIG_H

#include <stdlib.h>
#include "ES_Configure.h"
#include "ES_Events.h"
#include "AD.h"
#include "pwm.h"
#include "RC_Servo.h"
#include "IO_Ports.h"

enum{
    on,
    off,
    high,
    low,
    clocks,
    counter_clocks,
    add,
    mute,
};

enum{
    timer_58 = 0,
    shake_timer = 1,
    end_of_game_timer = 2,
    watching_dog_timer = 3,
    raise_timer = 4,
    protection_timer = 5,
};

#define comment
//#define IR2_TEST
//#define LED_TEST
//#define SERVO_TEST

#define LITTLE 20000
#define BIT 200000
#define MORE_BIT 1000000
//-----------------------------------------------------------
//input pins
#define IR_PIN_1 AD_PORTV5
#define IR_PIN_2 AD_PORTV6
#define IR_PIN_3 AD_PORTV8
#define IR_PIN_4 AD_PORTW8

#define TG_PIN_1 PORTX05_BIT

#define PW_PIN_1 PORTX06_BIT
//-----------------------------------------------------------
//output pins
#define LED_1 1//PORTZ03_LAT+1
#define LED_2 2//PORTZ04_LAT+2
#define LED_3 3//PORTZ05_LAT+4
#define LED_4 4//PORTZ11_LAT+8
#define LED_5 5//PORTZ07_LAT+16
#define LED_6 6//PORTZ08_LAT+32
#define LED_7 7//PORTZ09_LAT+64

#define SEVEN_SEG PORTW07_LAT

#define SERVO_PIN_2 RC_PORTX04
#define SERVO_POS_1 300
#define SERVO_POS_2 1900//
#define SERVO_POS_3 1200
#define SERVO_POS_4 500//
//-------------------------------------------------
#define SERVO_PIN_1 RC_PORTX03
#define SERVO_LOAD 900
#define SERVO_STOP 1210
#define SERVO_BRIDGE 1500


#define DC_PIN_1 PWM_PORTX11        
#define DC_POS_1 1
#define DC_POS_2 2
#define DC_POS_3 4
#define DC_POS_4 8

#define STEPPER_EN PWM_PORTY10      // useless now, very sad
#define STEPPER_DIR PWM_PORTY12     // ON/OFF
#define STEPPER_PULSE PWM_PORTY04


void init_config(void);
//------------------------low level functions-----------------------------------------
/*
 * effect: add or mute. add will auto add led 5->6->7 and no more. 
 * mute will clean 3 leds. add 1 led each time
*/
void led_record(uint16_t effect);

/*port: LED_X(1/2/3/4)
 *effect: on/off only
 *set a specific led each time
 */
void led_show(int port, uint16_t effect);

/*
 *effect: on/off. If off other setting doesn't matter
 * dir; clocks/counter_clocks
 * frequency: fixed at 400
 * steps: how much step
 * caution: this shall run in blocking mode, nothing need to be done after 
 * call it. if recall step, original step count will be wiped out and directly 
 * run new step count.
 */
void stepper_control(uint16_t effect, uint16_t dir, int frequency, int step);
void stepper_off(void);

/*
 *effect: on/off
 * mode: DC_POS_X
 */
void dc_control(uint16_t effect, uint8_t mode);

/*
 *pin:SERVO_PIN_1/SERVO_PIN_2
 *num: 1000-2000 or use SERVO_POS_x or SERVO_LOAD
 */
void servo_control(unsigned short int pin, unsigned short int num);//1000-2000

/*
 *effect: on/off
 */
void seven_seg_control(uint16_t effect);

/*
 *blocking delay use define like BIT for specific delay
 */
void delay(int set);
//------------------------hight level functions-----------------------------------------------------------
void shutdown(void);
void recover(void);
uint8_t tunnel_gen(void);
void led_num_show(uint8_t num, uint16_t effect);
void actuator_control(uint8_t num);
void shake(uint16_t effect);
void set_58_timer(void);
void set_shaking_timer(void);
void set_end_of_game_timer(void);
void set_watch_dog(void);
void set_raise_timer(void);
void set_protection_timer(void);
void last3_change(void);
#endif