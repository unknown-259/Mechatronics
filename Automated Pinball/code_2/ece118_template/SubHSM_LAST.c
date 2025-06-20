/*
Author:Neili Hu
Date:May.14.2025
version:0.2
*/
#include "ES_Configure.h"
#include "ES_Framework.h"
#include "BOARD.h"
#include "TemplateSubHSM.h"
#include "SubHSM_LAST.h"
#include "personal_config.h"


//#define no_power

typedef enum {
    shaking,
    init,
    waiting,
    new_game,
    celebration,
    fail,
    set_tunnel,
    guiding,
    raising,
} TemplateSubHSMState_t;

static const char *StateNames[] = {
    "shaking",
    "init",
    "waiting",
    "new_game",
    "celebration",
    "fail",
    "set_tunnel",
    "guiding",
    "raising",
};

static TemplateSubHSMState_t CurrentState = init;
static uint8_t MyPriority;
static uint8_t ir_now = 0;   // 1/2/3/4
static uint8_t score = 0;    // 1/2/3
static uint8_t protection = 0;

void fail_game_prepare(void);
void win_game_prepare(void);
void shake_prepare(void);

ES_Event RunSubHSM_last(ES_Event ThisEvent){
    uint8_t makeTransition = FALSE;
    TemplateSubHSMState_t nextState;
    //ES_Tattle();
//    if (ThisEvent.EventType == ES_TIMEOUT){//turn off the protection for tunnel 1
//        if (ThisEvent.EventParam == protection_timer){
//            protection = 0;
//            ThisEvent.EventType = ES_NO_EVENT;
//#ifdef comment
//            printf("\nprotection muted\n");
//#endif
//            //ES_Tail();
//            return ThisEvent;
//        }
//    }
    switch (CurrentState) {
    case shaking:
        switch (ThisEvent.EventType) {
        case ES_NO_EVENT:
            break;
        case POWER:
            goto power_dealing;
            break;
        case ES_TIMEOUT:
            if (ThisEvent.EventParam == shake_timer){
                shake(off);// no shake needed
                nextState = init;//next state
                makeTransition = TRUE;
#ifdef comment
                printf("\nshaking goto init\n");
#endif                
                break;
            }
        default:
            ThisEvent.EventType = ES_NO_EVENT;
            break;
        }
        break;

    case init:
        switch (ThisEvent.EventType) {
        case ES_NO_EVENT:
            break;
        case POWER:
            if (ThisEvent.EventParam == 1){
                recover();//control pattern
                led_show(LED_1, on);//light all leds
                led_show(LED_2, on);//light all leds
                led_show(LED_3, on);//light all leds
                led_show(LED_4, on);//light all leds
                led_record(add);//light all leds
                led_record(add);//light all leds
                led_record(add);//light all leds
                nextState = waiting;
                makeTransition = TRUE;
#ifdef comment
                printf("\ninit goto waiting\n");
#endif
                break;
            }
        default:
            ThisEvent.EventType = ES_NO_EVENT;
            break;
        }
        break;

    case waiting:
        switch (ThisEvent.EventType){
        case ES_NO_EVENT:
            break;
        case POWER:
#ifdef no_power
            ThisEvent.EventType = ES_NO_EVENT;
#else
            goto power_dealing;
#endif
            break;
        case FLOATING:
            if (ThisEvent.EventParam == 2){
#ifdef comment
                printf("\nwaiting goto new_game\n");
#endif
                led_show(LED_1, off);//off all leds for start of game
                led_show(LED_2, off);//off all leds for start of game
                led_show(LED_3, off);//off all leds for start of game
                led_show(LED_4, off);//off all leds for start of game
                led_record(mute);//off all leds for start of game
                score = 0;//clear recorded score
                ir_now = 0;//clear recorded led
                nextState = new_game;//next state
                makeTransition = TRUE;
                break;
            }
        default:
            ThisEvent.EventType = ES_NO_EVENT;
            break;
        }
        break;

    case new_game:
        switch (ThisEvent.EventType){
        case FLOATING:
            ThisEvent.EventType = ES_NO_EVENT;
            break;
        case POWER:
#ifdef no_power
            ThisEvent.EventType = ES_NO_EVENT;
#else
            goto power_dealing;
#endif
            break;
        default://will be triggered directly with all events
            ir_now = tunnel_gen();// generate new target
#ifdef comment
            printf("\n go:%d\n", ir_now);
#endif
            led_num_show(ir_now, on);//light that tunnel led
            actuator_control(ir_now);// move servo_1 and stepper to position 
            servo_control(SERVO_PIN_1, SERVO_BRIDGE);//load ball
            for(int j = 0; j < 1500000; j++);//delay on purpose
            servo_control(SERVO_PIN_1, SERVO_LOAD);//ready for the game
            ThisEvent.EventType = PLAY_GAME;
            nextState = guiding;//next state
            //makeTransition = TRUE;
            CurrentState = nextState;//emergency state switching
            
            //time ergent---------------------------------- 
            set_58_timer();//open 58 count down
            seven_seg_control(on);//open 58s count down display
            dc_control(on, ir_now);
            break;
        }
        break;

    case celebration:
        switch (ThisEvent.EventType) {
        case ES_NO_EVENT://maybe can use as trigger source for play
            break;
        case POWER:
#ifdef no_power
            ThisEvent.EventType = ES_NO_EVENT;
#else
            goto power_dealing;
#endif
            break;
        case ES_TIMEOUT:
            if (ThisEvent.EventParam == end_of_game_timer){
                stepper_control(on, counter_clocks, 400, 199);
                nextState = init;//next state
                //makeTransition = TRUE;
                CurrentState = nextState;
#ifdef comment
                printf("\ncelebration goto init\n");
#endif
                break;
            }
        default:
            ThisEvent.EventType = ES_NO_EVENT;
            break;
        }
        break;

    case fail:
        switch (ThisEvent.EventType) {
        case ES_NO_EVENT://maybe can use as trigger source for play
            break;
        case POWER:
#ifdef no_power
            ThisEvent.EventType = ES_NO_EVENT;
#else
            goto power_dealing;
#endif
            break;
        case ES_TIMEOUT:
            if (ThisEvent.EventParam == end_of_game_timer){
                stepper_control(on, counter_clocks, 400, 29);
                nextState = init;//next state
                //makeTransition = TRUE;
                CurrentState = nextState;
#ifdef comment
                printf("\nfail goto init\n");
#endif
                break;
            }
        default:
            ThisEvent.EventType = ES_NO_EVENT;
            break;
        }
        break;

    case set_tunnel:
        switch (ThisEvent.EventType) {
        case POWER:
#ifdef no_power
            ThisEvent.EventType = ES_NO_EVENT;
#else
            goto power_dealing;
#endif
            break;
        case ES_TIMEOUT:
            if (ThisEvent.EventParam == timer_58){
                fail_game_prepare();// ready for fail result
                //fail game play action
                ThisEvent.EventType = FAIL_GAME;
                nextState = fail;//next state
                makeTransition = TRUE;
#ifdef comment
                printf("\nset_tunnel goto fail\n");
#endif
            } else if (ThisEvent.EventParam == watching_dog_timer){
                nextState = shaking;//next state
                makeTransition = TRUE;
#ifdef comment
                printf("\nset_tunnel goto shaking\n");
#endif
            
                //time ergent---------------------------------- 
                shake_prepare();//start shaking process
            } else {
                ThisEvent.EventType = ES_NO_EVENT;
            }
            break;
        default://triggered from whatever source
            if (score == 3){//check if all 3 led lights
                win_game_prepare();// ready for celebration
                //win game play action
                ThisEvent.EventType = WIN_GAME;
                nextState = celebration;//next state
                CurrentState = nextState;
                //makeTransition = TRUE;
#ifdef comment
                printf("\n set_tunnel goto celebration\n");
#endif
            } else {//work for next goal
                servo_control(SERVO_PIN_1, SERVO_LOAD);
                led_num_show(ir_now, off);//clean old led
                ir_now = tunnel_gen();//generate new target
#ifdef comment
                printf("\n go:%d\n", ir_now);
#endif
                led_num_show(ir_now, on);// light the tunnel led
                actuator_control(ir_now);// move servo_1 and stepper to position 
                dc_control(on, ir_now);//open elevator
                ThisEvent.EventType = ES_NO_EVENT;
                nextState = raising;//next state
                //makeTransition = TRUE;
                CurrentState = nextState;//emergency state switching
#ifdef comment
                printf("\nset_tunnel goto raising\n");
#endif
                //time ergent----------------------------------
                set_raise_timer();//timer for elevator
            }
            break;
        }
        break;

    case guiding:
        switch (ThisEvent.EventType) {
        case ES_NO_EVENT:
            break;
        case POWER:
#ifdef no_power
            ThisEvent.EventType = ES_NO_EVENT;
#else
            goto power_dealing;
#endif
            break;
        case IR_TRIG:
#ifdef comment
            printf("\ntunnel:%d triggered\n", ThisEvent.EventParam);
#endif
//            if (ThisEvent.EventParam == 1){//turn on the protection
//                protection = 1;
//                set_protection_timer();
//#ifdef comment
//                printf("\nprotection triggered\n");
//#endif
//            }
//            if ((ThisEvent.EventParam == 2) && protection){
//                break;
//            }
            if (ThisEvent.EventParam == ir_now){//right 
                led_record(add);//accumulate one credit
                score++;//add one more score
#ifdef comment
                printf("\nright tunnel");
                printf("\nscore:%d\n",score);
#endif
                nextState = set_tunnel;//next state
                makeTransition = TRUE;
#ifdef comment
                printf("\nguiding goto set_tunnel\n");
#endif
                servo_control(SERVO_PIN_1, SERVO_STOP);
            } else {
                dc_control(on, ir_now);//start raising DC
                nextState = raising;//next state
                makeTransition = TRUE;
#ifdef comment
                printf("\nwrong tunnel");
                printf("\nguiding goto raising\n");
#endif

                //time ergent---------------------------------- 
                set_raise_timer(); //start count dc working time
            }
            set_watch_dog();//refresh obstacle count down
#ifdef comment
            printf("\nset dog\n");
#endif            
            break;
        case ES_TIMEOUT:
            if (ThisEvent.EventParam == watching_dog_timer){
                nextState = shaking;//next state
                makeTransition = TRUE;
#ifdef comment
                printf("\nguiding goto shaking\n");
#endif
                
                //time ergent---------------------------------- 
                shake_prepare();//start shaking process
                break;
            } else if (ThisEvent.EventParam == timer_58){
                fail_game_prepare();// ready for fail result
                //fail game play action
                ThisEvent.EventType = FAIL_GAME;
                nextState = fail;//next state
                makeTransition = TRUE;
#ifdef comment
                printf("\ngoto fail\n");
#endif
                break;
            }
        default:
            ThisEvent.EventType = ES_NO_EVENT;
            break;
        }
        break;

    case raising:
        switch (ThisEvent.EventType) {
        case ES_NO_EVENT:
            break;
        case POWER:
#ifdef no_power
            ThisEvent.EventType = ES_NO_EVENT;
#else
            goto power_dealing;
#endif
            break;
        case IR_TRIG:
#ifdef comment
            printf("\ntunnel:%d triggered\n", ThisEvent.EventParam);
#endif
//            if (ThisEvent.EventParam == 1){
//                protection = 1;
//                set_protection_timer();
//#ifdef comment
//                printf("\nprotection triggered\n");
//#endif
//            }
//            if ((ThisEvent.EventParam == 2) && protection){
//                break;
//            }
            if ((ThisEvent.EventParam & ir_now) == ir_now){//right
                led_record(add);//accumulate one credit
                score++;//add one more score
#ifdef comment
                printf("\nscore:%d\n",score);
#endif
                nextState = set_tunnel;//next state
                makeTransition = TRUE;
#ifdef comment
                printf("\nraising goto set_tunnel\n");
#endif
                servo_control(SERVO_PIN_1, SERVO_STOP);
            } else {
                ThisEvent.EventType = ES_NO_EVENT;
                
                //time ergent---------------------------------- 
                set_raise_timer(); //restart count dc working time
            }
            set_watch_dog();//refresh obstacle count down
#ifdef comment
            printf("\nset dog\n");
#endif
            break;
        case ES_TIMEOUT:
            if (ThisEvent.EventParam == raise_timer){
                nextState = guiding;//next state
                makeTransition = TRUE;
#ifdef comment
                printf("\nraising goto guiding\n");
#endif
                break;
            } else if (ThisEvent.EventParam == timer_58){
                fail_game_prepare();// ready for fail result
                //fall game play action
                ThisEvent.EventType = FAIL_GAME;
                nextState = fail;//next state
                makeTransition = TRUE;
#ifdef comment
                printf("\nraising goto fail\n");
#endif
                break;
            } else if (ThisEvent.EventParam == watching_dog_timer){
                dc_control(off, 0);// timeout for obstacle
                nextState = shaking;//next state
                makeTransition = TRUE;
#ifdef comment
                printf("\nraising goto shaking\n");
#endif
                
                //time ergent---------------------------------- 
                shake_prepare();//start shaking process
                break;
            }
        default:
            ThisEvent.EventType = ES_NO_EVENT;
            break;
        }
        break;

    default:
        printf("A mysterious place.\r\n");
        break;

power_dealing:
        if (ThisEvent.EventParam == 0){
            shutdown();// "close" all systems
            nextState = init;//next state
            makeTransition = TRUE;
#ifdef comment
            printf("\npower event & goto init\n");
#endif
        } else {
            ThisEvent.EventType = ES_NO_EVENT;
        }
    }
    if (makeTransition == TRUE) {
        RunSubHSM_last(EXIT_EVENT); // <- rename to your own Run function
        CurrentState = nextState;
        RunSubHSM_last(ENTRY_EVENT); // <- rename to your own Run function
    }
    //ES_Tail();
    return ThisEvent;
}

void fail_game_prepare(void){
    seven_seg_control(off);
    actuator_control(5);
    last3_change();
    led_num_show(ir_now, off);
    dc_control(off, 0);
    ES_Timer_StopTimer(timer_58);
    ES_Timer_StopTimer(watching_dog_timer);
    set_end_of_game_timer();
    servo_control(SERVO_PIN_1, SERVO_STOP);
    stepper_control(on, clocks, 400, 28);
    
}

void win_game_prepare(void){
    actuator_control(5);
    ES_Timer_StopTimer(timer_58);
    ES_Timer_StopTimer(watching_dog_timer);
    set_end_of_game_timer();
    dc_control(off, 0);
    led_show(LED_1, on);
    led_show(LED_2, on);
    led_show(LED_3, on);
    led_show(LED_4, on);
    seven_seg_control(off);
    servo_control(SERVO_PIN_1, SERVO_STOP);
    stepper_control(on, clocks, 400, 200);
}
void shake_prepare(void){
    actuator_control(5);
    led_num_show(ir_now, off);
    led_record(mute);
    seven_seg_control(off);
    servo_control(SERVO_PIN_1, SERVO_STOP);
    ES_Timer_StopTimer(timer_58);
    ES_Timer_StopTimer(watching_dog_timer);
    set_shaking_timer();
    shake(on);
    
}