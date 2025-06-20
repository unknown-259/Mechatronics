/*
 * File: TemplateFSM.c
 * Author: J. Edward Carryer
 * Modified: Gabriel H Elkaim
 *
 * Template file to set up a Flat State Machine to work with the Events and Services
 * Frameword (ES_Framework) on the Uno32 for the CMPE-118/L class. Note that this file
 * will need to be modified to fit your exact needs, and most of the names will have
 * to be changed to match your code.
 *
 * This is provided as an example and a good place to start.
 *
 *Generally you will just be modifying the statenames and the run function
 *However make sure you do a find and replace to convert every instance of
 *  "Template" to your current state machine's name
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

#include "TemplateFSM.h"
#include <BOARD.h>
//Uncomment these for the Roaches
#include "roach.h"
//#include "RoachFrameworkEvents.h"
#include "TemplateEventChecker.h"
#include <stdio.h>


/*******************************************************************************
 * MODULE #DEFINES                                                             *
 ******************************************************************************/
#define FLEFT_BUMP_MASK (1)
#define FRIGHT_BUMP_MASK (1<<1)
#define RLEFT_BUMP_MASK (1<<2)
#define RRIGHT_BUMP_MASK (1<<3)

/*******************************************************************************
 * PRIVATE FUNCTION PROTOTYPES                                                 *
 ******************************************************************************/
/* Prototypes for private functions for this machine. They should be functions
   relevant to the behavior of this state machine.*/


/*******************************************************************************
 * PRIVATE MODULE VARIABLES                                                            *
 ******************************************************************************/

/* You will need MyPriority and the state variable; you may need others as well.
 * The type of state variable should match that of enum in header file. */

typedef enum {
    InitPState,
    Forward,
    Backup,
    Spin,
    Idle,
} TemplateFSMState_t; // <- For Tattletale, keep the "State_t" on the end of the enum type name.

static const char *StateNames[] = {
	"InitPState",
	"Forward",
    "Backup",
    "Spin",
    "Idle",
};


static TemplateFSMState_t CurrentState = InitPState; // <- change enum name to match ENUM
static TemplateFSMState_t PrevState = InitPState;
static uint8_t MyPriority;
static uint16_t lightVal;



/*******************************************************************************
 * PUBLIC FUNCTIONS                                                            *
 ******************************************************************************/

/**
 * @Function InitTemplateFSM(uint8_t Priority)
 * @param Priority - internal variable to track which event queue to use
 * @return TRUE or FALSE
 * @brief This will get called by the framework at the beginning of the code
 *        execution. It will post an ES_INIT event to the appropriate event
 *        queue, which will be handled inside RunTemplateFSM function. Remember
 *        to rename this to something appropriate.
 *        Returns TRUE if successful, FALSE otherwise
 * @author J. Edward Carryer, 2011.10.23 19:25 */
uint8_t InitTemplateFSM(uint8_t Priority)
{
    MyPriority = Priority;
    // put us into the Initial PseudoState
    CurrentState = InitPState;
    
    //ES_Timer_InitTimer(backup, 2*1000);
    
    // post the initial transition event
    if (ES_PostToService(MyPriority, INIT_EVENT) == TRUE) {
        return TRUE;
    } else {
        return FALSE;
    }
}

/**
 * @Function PostTemplateFSM(ES_Event ThisEvent)
 * @param ThisEvent - the event (type and param) to be posted to queue
 * @return TRUE or FALSE
 * @brief This function is a wrapper to the queue posting function, and its name
 *        will be used inside ES_Configure to point to which queue events should
 *        be posted to. Remember to rename to something appropriate.
 *        Returns TRUE if successful, FALSE otherwise
 * @author J. Edward Carryer, 2011.10.23 19:25 */
uint8_t PostTemplateFSM(ES_Event ThisEvent)
{
    return ES_PostToService(MyPriority, ThisEvent);
}

/**
 * @Function RunTemplateFSM(ES_Event ThisEvent)
 * @param ThisEvent - the event (type and param) to be responded.
 * @return Event - return event (type and param), in general should be ES_NO_EVENT
 * @brief This function is where you implement the whole of the flat state machine,
 *        as this is called any time a new event is passed to the event queue. This
 *        function will be called recursively to implement the correct order for a
 *        state transition to be: exit current state -> enter next state using the
 *        ES_EXIT and ES_ENTRY events.
 * @note Remember to rename to something appropriate.
 *       Returns ES_NO_EVENT if the event have been "consumed."
 * @author J. Edward Carryer, 2011.10.23 19:25 */
ES_Event RunTemplateFSM(ES_Event ThisEvent)
{
    uint8_t makeTransition = FALSE; // use to flag transition
    TemplateFSMState_t nextState; // <- need to change enum type here

    ES_Tattle(); // trace call stack
    
    int motorSpeed;
    uint16_t param_hold;

    switch (CurrentState) {
    case InitPState: // If current state is initial Psedudo State
        if (ThisEvent.EventType == ES_INIT)// only respond to ES_Init
        {
            printf("init\n");
            // this is where you would put any actions associated with the
            // transition from the initial pseudo-state into the actual
            // initial state

            // now put the machine into the actual initial state
            
            nextState = Forward;
            makeTransition = TRUE;
            ThisEvent.EventType = ES_NO_EVENT;
        }
        break;

    case Forward: // in the first state, replace this with appropriate state
        if (ThisEvent.EventType == LIGHT_TO_DARK) { 
            nextState = Idle;  // go to idle state if light level is above threshold
            makeTransition = TRUE;
            ThisEvent.EventType = ES_NO_EVENT;
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
        
    case Idle:
        motorSpeed = 0;
        Roach_LeftMtrSpeed(motorSpeed);
        Roach_RightMtrSpeed(motorSpeed);

        // idk why this doesn't always work
        // move forward if light level is less than 300
        if (ThisEvent.EventType == DARK_TO_LIGHT) {
            printf("going to Forward state.\n");
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
        RunTemplateFSM(EXIT_EVENT);
        PrevState = CurrentState;
        CurrentState = nextState;
        RunTemplateFSM(ENTRY_EVENT);
    }
    ES_Tail(); // trace call stack end
    return ThisEvent;
}


/*******************************************************************************
 * PRIVATE FUNCTIONS                                                           *
 ******************************************************************************/
