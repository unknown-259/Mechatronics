/*
Author:Neili Hu
Date:May.14.2025
version:0.2
*/

#include "ES_Configure.h"
#include "ES_Framework.h"
#include "BOARD.h"
#include "TemplateHSM.h"
#include "TemplateSubHSM.h"
#include "SubHSM_LAST.h"
#include "personal_config.h"

/*******************************************************************************
 * MODULE #DEFINES                                                             *
 ******************************************************************************/
typedef enum {
    Jamming,
    Pause,
    Result,
    Playing,
} TemplateSubHSMState_t;

static const char *StateNames[] = {
    "Jamming",
    "Pause",
    "Result",
    "Playing",
};

static TemplateSubHSMState_t CurrentState = Pause; // <- change name to match ENUM
static uint8_t MyPriority;

/**
 * @Function InitTemplateSubHSM(uint8_t Priority)
 * @param Priority - internal variable to track which event queue to use
 * @return TRUE or FALSE
 * @brief This will get called by the framework at the beginning of the code
 *        execution. It will post an ES_INIT event to the appropriate event
 *        queue, which will be handled inside RunTemplateFSM function. Remember
 *        to rename this to something appropriate.
 *        Returns TRUE if successful, FALSE otherwise
 * @author J. Edward Carryer, 2011.10.23 19:25 */
uint8_t InitTemplateSubHSM(void)
{
    ES_Event returnEvent;

    CurrentState = Pause;
    returnEvent = RunTemplateSubHSM_2(INIT_EVENT);
    if (returnEvent.EventType == ES_NO_EVENT) {
        return TRUE;
    }
    return FALSE;
}

ES_Event RunTemplateSubHSM_2(ES_Event ThisEvent){
    uint8_t makeTransition = FALSE; // use to flag transition
    TemplateSubHSMState_t nextState; // <- change type to correct enum
    //ES_Tattle(); // trace call stack

    switch (CurrentState) {
    case Result:
        ThisEvent = RunSubHSM_last(ThisEvent);
        switch (ThisEvent.EventType) {
        case ES_NO_EVENT:
            break;
        case POWER:
            if (ThisEvent.EventParam == 0){
                nextState = Pause;
                makeTransition = TRUE;
            }
            break;
        case ES_TIMEOUT:
            if (ThisEvent.EventParam == end_of_game_timer){
                nextState = Pause;
                makeTransition = TRUE;
            }
        default:
            ThisEvent.EventType = ES_NO_EVENT;
        }
        break;

    case Playing:
        ThisEvent = RunSubHSM_last(ThisEvent);
        switch (ThisEvent.EventType) {
        case ES_NO_EVENT:
            break;
        case POWER:
            if (ThisEvent.EventParam == 0){
                nextState = Pause;
                makeTransition = TRUE;
            } else {
                ThisEvent.EventType = ES_NO_EVENT;
            }
            break;
        case WIN_GAME:
            nextState = Result;
            makeTransition = TRUE;
            break;
        case FAIL_GAME:
            nextState = Result;
            makeTransition = TRUE;
            break;
        case ES_TIMEOUT:
            if (ThisEvent.EventParam == watching_dog_timer){
                nextState = Jamming;
                makeTransition = TRUE;
            }
        default:
            ThisEvent.EventType = ES_NO_EVENT;
        }
        break;

    case Jamming:
        ThisEvent = RunSubHSM_last(ThisEvent);
        switch (ThisEvent.EventType) {
        case ES_NO_EVENT:
            break;
        case POWER:
            if (ThisEvent.EventParam == 0){
                nextState = Pause;
                makeTransition = TRUE;
            } else {
                ThisEvent.EventType = ES_NO_EVENT;
            }
            break;
        case ES_TIMEOUT:
            if (ThisEvent.EventParam == shake_timer){
                nextState = Pause;
                makeTransition = TRUE;
            }
        default:
            ThisEvent.EventType = ES_NO_EVENT;
        }
        break;

    case Pause:
        ThisEvent = RunSubHSM_last(ThisEvent);
        switch (ThisEvent.EventType) {
        case ES_NO_EVENT:
            break;
        case PLAY_GAME:
            nextState = Playing;
            makeTransition = TRUE;
            break;
        }
        break;
    }

    if (makeTransition == TRUE) {
        RunTemplateSubHSM_2(EXIT_EVENT); // <- rename to your own Run function
        CurrentState = nextState;
        RunTemplateSubHSM_2(ENTRY_EVENT); // <- rename to your own Run function
    }

    //ES_Tail(); // trace call stack end
    return ThisEvent;
}

