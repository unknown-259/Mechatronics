/****************************************************************************
 Module
   TemplateService.c

 Revision
   1.0.1

 Description
   This is a template file for implementing a simple service under the 
   Gen2 Events and Services Framework.

 Notes

 History
 When           Who     What/Why
 -------------- ---     --------
 01/16/12 09:58 jec      began conversion from TemplateFSM.c
 ****************************************************************************/
/*----------------------------- Include Files -----------------------------*/
/* include header files for this state machine as well as any machines at the
   next lower level in the hierarchy that are sub-machines to this machine
 */
#include "ES_Configure.h"
#include "ES_Framework.h"
#include "ES_TimerService.h"
#include <BOARD.h>
#include <ES_TattleTale.h>
#include "TemplateEventChecker.h"
#include "TemplateHSM.h"
/*----------------------------- Module Defines ----------------------------*/

/*---------------------------- Module Functions ---------------------------*/
/* prototypes for private functions for this service.They should be functions
   relevant to the behavior of this service
 */

/*---------------------------- Module Variables ---------------------------*/
// with the introduction of Gen2, we need a module level Priority variable
static uint8_t MyPriority;

static ES_EventTyp_t UserTimerStates[TIMERS_USED];


/*------------------------------ Module Code ------------------------------*/

/**
 * @Function InitTimerService(uint8_t Priority)
 * @param uint8_t - the priorty of this service
 * @return uint8_t - FALSE if error in initialization, TRUE otherwise
 * @brief  Saves away the priority, and does any  other required initialization for 
 * this service
 * @author Max Dunne   2013.01.04 */
uint8_t InitTimerService(uint8_t Priority) {
    ES_Event ThisEvent;

    MyPriority = Priority;

    // post the initial transition event
    ThisEvent.EventType = ES_INIT;
    if (ES_PostToService(MyPriority, ThisEvent) == TRUE) {
        return TRUE;
    } else {
        return FALSE;
    }

}

/**
 * @Function PostTimerService(ES_Event ThisEvent)
 * @param ThisEvent - the event to post to the queue
 * @return FALSE or TRUE
 * @brief  Posts an event to the timers queue
 * @author Max Dunne   2013.01.04 */
uint8_t PostTimerService(ES_Event ThisEvent) {
    return ES_PostToService(MyPriority, ThisEvent);
}

/**
 * @Function RunTimerService(ES_Event ThisEvent)
 * @param ES_Event - the event to process
 * @return ES_NO_EVENT or ES_ERROR 
 * @brief  accepts the timer events and updates the state arrays
 * @author Max Dunne   2013.01.04 */
ES_Event RunTimerService(ES_Event ThisEvent) {
    ES_Event ReturnEvent;
    
    ReturnEvent.EventType = ES_NO_EVENT; // assume no errors

    switch (ThisEvent.EventType) {
        case ES_INIT:
            break;
        case ES_TIMEOUT:
            if (ThisEvent.EventParam == 6){//IR
                IR_LED();
                ES_Timer_InitTimer(6, 15);
            } else if (ThisEvent.EventParam == 7){
                //ES_Timer_InitTimer(7, 500);
            } else if (ThisEvent.EventParam == 8){//POWER & TRIGGER
                Power();
                Trigger();
                ES_Timer_InitTimer(8, 250);
            } else if (ThisEvent.EventParam == 9){//general call
                PostTemplateHSM(ReturnEvent);
                ES_Timer_InitTimer(9, 80);
            }
            break;
        default:
            break;
    }

    //ES_Tail();
    return ReturnEvent;
}

/**
 * @Function IsTimerExpired(unsigned char Num)
 * @param Num - the number of the timer to check
 * @return ERROR or TRUE or FALSE 
 * @brief  used to determine if a timer is currently expired.
 * @author Max Dunne   2013.01.04 */
int8_t IsTimerExpired(unsigned char Num) {
    if (Num >= TIMERS_USED) {
        return ERROR;
    }
    if (UserTimerStates[Num] == ES_TIMEOUT) {
        return TRUE;
    } else {
        return FALSE;
    }
}

/**
 * @Function IsTimerActive(unsigned char Num)
 * @param Num - the number of the timer to check
 * @return ERROR or TRUE or FALSE
 * @brief  used to determine if a timer is currently active.
 * @author Max Dunne   2013.01.04 */
int8_t IsTimerActive(unsigned char Num) {
    if (Num >= TIMERS_USED) {
        return ERROR;
    }
    if (UserTimerStates[Num] == ES_TIMERACTIVE) {
        return TRUE;
    } else {
        return FALSE;
    }
}

/**
 * @Function IsTimerStopped(unsigned char Num)
 * @param Num - the number of the timer to check
 * @return ERROR or TRUE or FALSE
 * @brief  used to determine if a timer is currently stopped.
 * @author Max Dunne   2013.01.04 */
int8_t IsTimerStopped(unsigned char Num) {
    if (Num >= TIMERS_USED) {
        return ERROR;
    }
    if (UserTimerStates[Num] == ES_TIMERSTOPPED) {
        return TRUE;
    } else {
        return FALSE;
    }
}

/**
 * @Function GetUserTimerState(unsigned char Num)
 * @param Num - the number of the timer to check
 * @return ES_EventTyp_t current state of timer
 * @brief  used to get the current timer state.
 * @author Max Dunne   2013.01.04 */
ES_EventTyp_t GetUserTimerState(unsigned char Num) {
    if (Num >= TIMERS_USED) {
        return ERROR;
    }
    return UserTimerStates[Num];
}

/*------------------------------- Footnotes -------------------------------*/
/*------------------------------ End of file ------------------------------*/

