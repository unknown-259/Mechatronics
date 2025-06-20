/*
 * File:   TemplateEventChecker.c
 * Author: Gabriel Hugh Elkaim
 *
 * Template file to set up typical EventCheckers for the  Events and Services
 * Framework (ES_Framework) on the Uno32 for the CMPE-118/L class. Note that
 * this file will need to be modified to fit your exact needs, and most of the
 * names will have to be changed to match your code.
 *
 * This EventCheckers file will work with both FSM's and HSM's.
 *
 * Remember that EventCheckers should only return TRUE when an event has occured,
 * and that the event is a TRANSITION between two detectable differences. They
 * should also be atomic and run as fast as possible for good results.
 *
 * This file includes a test harness that will run the event detectors listed in the
 * ES_Configure file in the project, and will conditionally compile main if the macro
 * EVENTCHECKER_TEST is defined (either in the project or in the file). This will allow
 * you to check you event detectors in their own project, and then leave them untouched
 * for your project unless you need to alter their post functions.
 *
 * Created on September 27, 2013, 8:37 AM
 */

/*******************************************************************************
 * MODULE #INCLUDE                                                             *
 ******************************************************************************/

#include "ES_Configure.h"
#include "TemplateEventChecker.h"
#include "ES_Events.h"
#include "serial.h"
#include "AD.h"
#include "personal_config.h"
#include "TemplateHSM.h"
/*******************************************************************************
 * MODULE #DEFINES                                                             *
 ******************************************************************************/
#define BATTERY_DISCONNECT_THRESHOLD 175

/**
 * @Function TemplateCheckBattery(void)
 * @param none
 * @return TRUE or FALSE
 * @brief This function is a prototype event checker that checks the battery voltage
 *        against a fixed threshold (#defined in the .c file). Note that you need to
 *        keep track of previous history, and that the actual battery voltage is checked
 *        only once at the beginning of the function. The function will post an event
 *        of either BATTERY_CONNECTED or BATTERY_DISCONNECTED if the power switch is turned
 *        on or off with the USB cord plugged into the Uno32. Returns TRUE if there was an 
 *        event, FALSE otherwise.
 * @note Use this code as a template for your other event checkers, and modify as necessary.
 * @author Gabriel H Elkaim, 2013.09.27 09:18
 * @modified Gabriel H Elkaim/Max Dunne, 2016.09.12 20:08 */
uint8_t TemplateCheckBattery(void) {
    static ES_EventTyp_t lastEvent = BATTERY_DISCONNECTED;
    ES_EventTyp_t curEvent;
    ES_Event thisEvent;
    uint8_t returnVal = FALSE;
    uint16_t batVoltage = AD_ReadADPin(BAT_VOLTAGE); // read the battery voltage

    if (batVoltage > BATTERY_DISCONNECT_THRESHOLD) { // is battery connected?
        curEvent = BATTERY_CONNECTED;
    } else {
        curEvent = BATTERY_DISCONNECTED;
    }
    if (curEvent != lastEvent) { // check for change from last time
        thisEvent.EventType = curEvent;
        thisEvent.EventParam = batVoltage;
        returnVal = TRUE;
        lastEvent = curEvent; // update history
#ifndef EVENTCHECKER_TEST           // keep this as is for test harness
        PostTemplateHSM(thisEvent); // Change it to your target service's post function
#else
        SaveEvent(thisEvent);
#endif   
    }
    return (returnVal);
}

uint8_t read_all_IR(void){
    uint8_t res = 0;
    static uint16_t threshold = 700;
    static uint16_t baseline = 60;
    uint16_t temp = 0;
    
    //while(!AD_IsNewDataReady());
    temp = AD_ReadADPin(IR_PIN_1);
    if (temp < threshold){//118/1023
        if (temp > baseline){
            res = 1;
        }
    }
    //while(!AD_IsNewDataReady());
    temp = AD_ReadADPin(IR_PIN_2);
    if (temp < threshold){//118/1023
        if (temp > baseline){//when led_1 covered this pin also affected....
            res = 2;
        }
    }
    //while(!AD_IsNewDataReady());
    temp = AD_ReadADPin(IR_PIN_3);
    if (temp < threshold){//114/1023
        if (temp > baseline){
            res = 3;
        }
    }
    //while(!AD_IsNewDataReady());
    temp = AD_ReadADPin(IR_PIN_4);
    if (temp < threshold){//114/1023
        if (temp > baseline){
            res = 4;
        }
    }
    return res;
}

uint8_t IR_LED(void){
    static uint8_t IR_VAL = 0;
    ES_Event thisEvent;
    uint8_t returnVal = FALSE;
    uint8_t IR_RES = read_all_IR();
    
    if(IR_RES == 0){
        IR_VAL = IR_RES;
    } else if(IR_VAL != IR_RES){
        thisEvent.EventType = IR_TRIG;
        thisEvent.EventParam = IR_RES;
        IR_VAL = IR_RES;
        returnVal = TRUE;
        PostTemplateHSM(thisEvent);
    }
    return (returnVal);
}

uint8_t Trigger(void){
    ES_Event thisEvent;
    uint8_t returnVal = FALSE;
    uint8_t TG_RES = (IO_PortsReadPort(PORTX)&0b100000)>>5;
    if(TG_RES == 0){//trigger on
        thisEvent.EventType = FLOATING;
        thisEvent.EventParam = 2;
        PostTemplateHSM(thisEvent);
    } else if (TG_RES == 1){//trigger off
        thisEvent.EventType = FLOATING;
        thisEvent.EventParam = 1;
        PostTemplateHSM(thisEvent);
    }
    return (returnVal);
}

uint8_t Power(void){
    ES_Event thisEvent;
    uint8_t returnVal = FALSE;
    uint8_t PW_RES = (IO_PortsReadPort(PORTX)&0b1000000)>>6;

    if(PW_RES == 0){//power on
        thisEvent.EventType = POWER;
        thisEvent.EventParam = 1;
        PostTemplateHSM(thisEvent);
    } else if (PW_RES == 1){// power off
        thisEvent.EventType = POWER;
        thisEvent.EventParam = 0;
        PostTemplateHSM(thisEvent);
    }
    return (returnVal);
}