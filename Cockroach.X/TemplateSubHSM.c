/*
 * File: TemplateSubHSM.c
 * Author: J. Edward Carryer
 * Modified: Gabriel H Elkaim
 *
 * Template file to set up a Heirarchical State Machine to work with the Events and
 * Services Framework (ES_Framework) on the Uno32 for the CMPE-118/L class. Note that
 * this file will need to be modified to fit your exact needs, and most of the names
 * will have to be changed to match your code.
 *
 * There is for a substate machine. Make sure it has a unique name
 *
 * This is provided as an example and a good place to start.
 *
 * History
 * When           Who     What/Why
 * -------------- ---     --------
 * 09/13/13 15:17 ghe      added tattletail functionality and recursive calls
 * 01/15/12 11:12 jec      revisions for Gen2 framework
 * 11/07/11 11:26 jec      made the queue static
 * 10/30/11 17:59 jec      fixed references to CurrentEvent in RunTemplateSM()
 * 10/23/11 18:20 jec      began conversion from SMTemplate.c (02/20/07 rev)
 */


/*******************************************************************************
 * MODULE #INCLUDE                                                             *
 ******************************************************************************/

#include "ES_Configure.h"
#include "ES_Framework.h"
#include "BOARD.h"
#include "TemplateHSM.h"
#include "TemplateSubHSM.h"
#include "roach.h"
#include "TemplateEventChecker.h"
#include <stdio.h>

/*******************************************************************************
 * MODULE #DEFINES                                                             *
 ******************************************************************************/
typedef enum {
    InitPSubState,
    Forward,
    Backup,
    Spin,
    Dance,
} TemplateSubHSMState_t;

static const char *StateNames[] = {
    "InitPSubState",
    "Forward",
    "Backup",
    "Spin",
    "Dance",
};

#define FLEFT_BUMP_MASK (1)
#define FRIGHT_BUMP_MASK (1<<1)
#define RLEFT_BUMP_MASK (1<<2)
#define RRIGHT_BUMP_MASK (1<<3)

/*******************************************************************************
 * PRIVATE FUNCTION PROTOTYPES                                                 *
 ******************************************************************************/
/* Prototypes for private functions for this machine. They should be functions
   relevant to the behavior of this state machine */

/*******************************************************************************
 * PRIVATE MODULE VARIABLES                                                            *
 ******************************************************************************/
/* You will need MyPriority and the state variable; you may need others as well.
 * The type of state variable should match that of enum in header file. */

static TemplateSubHSMState_t CurrentState = InitPSubState; // <- change name to match ENUM
static uint8_t MyPriority;


/*******************************************************************************
 * PUBLIC FUNCTIONS                                                            *
 ******************************************************************************/

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

    ES_Timer_InitTimer(dance, 5 * 1000);  // Start timer
    
    CurrentState = InitPSubState;
    returnEvent = RunTemplateSubHSM(INIT_EVENT);
    if (returnEvent.EventType == ES_NO_EVENT) {
        return TRUE;
    }
    return FALSE;
}

/**
 * @Function RunTemplateSubHSM(ES_Event ThisEvent)
 * @param ThisEvent - the event (type and param) to be responded.
 * @return Event - return event (type and param), in general should be ES_NO_EVENT
 * @brief This function is where you implement the whole of the heirarchical state
 *        machine, as this is called any time a new event is passed to the event
 *        queue. This function will be called recursively to implement the correct
 *        order for a state transition to be: exit current state -> enter next state
 *        using the ES_EXIT and ES_ENTRY events.
 * @note Remember to rename to something appropriate.
 *       The lower level state machines are run first, to see if the event is dealt
 *       with there rather than at the current level. ES_EXIT and ES_ENTRY events are
 *       not consumed as these need to pass pack to the higher level state machine.
 * @author J. Edward Carryer, 2011.10.23 19:25
 * @author Gabriel H Elkaim, 2011.10.23 19:25 */
ES_Event RunTemplateSubHSM(ES_Event ThisEvent)
{
    uint8_t makeTransition = FALSE; // use to flag transition
    TemplateSubHSMState_t nextState; // <- change type to correct enum

    ES_Tattle(); // trace call stack
    
    int motorSpeed;
    uint16_t param_hold;

    switch (CurrentState) {
    case InitPSubState: // If current state is initial Psedudo State
        if (ThisEvent.EventType == ES_INIT)// only respond to ES_Init
        {
            // this is where you would put any actions associated with the
            // transition from the initial pseudo-state into the actual
            // initial state

            // now put the machine into the actual initial state
            nextState = Forward;
            makeTransition = TRUE;
            ThisEvent.EventType = ES_NO_EVENT;
        }
        break;

    case Forward: // in the first state, replace this with correct names
        
        if(ThisEvent.EventType == ES_TIMEOUT){
            ES_Timer_InitTimer(dance_2, 3*1000);  // Start timer
            nextState = Dance;
            makeTransition = TRUE;
            ThisEvent.EventType = ES_NO_EVENT;
        }
        
        if (ThisEvent.EventType == LIGHT_TO_DARK) { 
            //go from the InLight to InDark
            break;
            
        } else {
            motorSpeed = 80;
            Roach_LeftMtrSpeed(motorSpeed);
            Roach_RightMtrSpeed(motorSpeed);
            printf("going forward\n");
            if ((ThisEvent.EventType == BUMPER_TRIGGERED)) {
                if(ThisEvent.EventParam == FLEFT_BUMP_MASK){
                    printf("front left hit\n");
                    param_hold = FLEFT_BUMP_MASK;
                    ES_Timer_InitTimer(backup_2, 1000);  // Start timer
                    nextState = Backup;
                    makeTransition = TRUE;
                    ThisEvent.EventType = ES_NO_EVENT;
                }
                if(ThisEvent.EventParam == FRIGHT_BUMP_MASK){
                    printf("front right hit\n");
                    param_hold = FRIGHT_BUMP_MASK;
                    ES_Timer_InitTimer(backup_2, 1000);  // Start timer
                    nextState = Backup;
                    makeTransition = TRUE;
                    ThisEvent.EventType = ES_NO_EVENT;
                }
                if(ThisEvent.EventParam == RLEFT_BUMP_MASK){
                    printf("rear left hit\n");
                    param_hold = RLEFT_BUMP_MASK;
                }
                if(ThisEvent.EventParam == RRIGHT_BUMP_MASK){
                    printf("rear right hit\n");
                    param_hold = RRIGHT_BUMP_MASK;
                }
            }
        }
        
        break;
        
    case Backup:
        
        printf("backing up\n");
        Roach_LeftMtrSpeed(-80);
        Roach_RightMtrSpeed(-80);
        if (ThisEvent.EventType == BUMPER_TRIGGERED &&
            (ThisEvent.EventParam == RLEFT_BUMP_MASK || ThisEvent.EventParam == RRIGHT_BUMP_MASK)) {
            printf("switching to forward.\n");
            nextState = Forward;
            makeTransition = TRUE;
            ThisEvent.EventType = ES_NO_EVENT;
            break;
        }

        if (ThisEvent.EventType == ES_TIMEOUT) {
            ES_Timer_InitTimer(backup_4, 1000);
            nextState = Spin;
            makeTransition = TRUE;
            ThisEvent.EventType = ES_NO_EVENT;
        }
        
        break;
    
    case Spin:
        printf("spinning in place\n");
        Roach_LeftMtrSpeed(80);
        Roach_RightMtrSpeed(-80);
        if (ThisEvent.EventType == ES_TIMEOUT) {
            printf("returning to Forward state\n");
            nextState = Forward;
            makeTransition = TRUE;
            ThisEvent.EventType = ES_NO_EVENT;
        }
        
        break;

    case Dance:
        printf("dancing\n");
        Roach_LeftMtrSpeed(-80);
        Roach_RightMtrSpeed(80);
        if (ThisEvent.EventType == ES_TIMEOUT) {
            printf("returning to Forward state\n");
            ES_Timer_InitTimer(dance, 5 * 1000);  // Restart timer
            nextState = Forward;
            makeTransition = TRUE;
            ThisEvent.EventType = ES_NO_EVENT;
        }
        
        break;
        
    default: // all unhandled states fall into here
        break;
    } // end switch on Current State

    if (makeTransition == TRUE) { // making a state transition, send EXIT and ENTRY
        // recursively call the current state with an exit event
        RunTemplateSubHSM(EXIT_EVENT); // <- rename to your own Run function
        CurrentState = nextState;
        RunTemplateSubHSM(ENTRY_EVENT); // <- rename to your own Run function
    }

    ES_Tail(); // trace call stack end
    return ThisEvent;
}


/*******************************************************************************
 * PRIVATE FUNCTIONS                                                           *
 ******************************************************************************/

