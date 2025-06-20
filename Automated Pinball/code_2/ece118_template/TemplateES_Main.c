#include <BOARD.h>
#include <xc.h>
#include <stdlib.h>
#include <stdio.h>
#include "ES_Configure.h"
#include "ES_Framework.h"
#include "personal_config.h"
#include "serial.h"
#include "IO_Ports.h"

void main(void)
{
    ES_Return_t ErrorType;

    BOARD_Init();
    SERIAL_Init();
#ifdef comment
    printf("This is testing time.\n");
#endif
//    int j = 0;
//    while(1){
//        for(j=0;j<2000000;j++);
//        led_show(LED_1, on);
//        for(j=0;j<2000000;j++);
//        led_show(LED_1, off);
//    }
//    //*************IRSENSORTEST
//    int ir_threshold = 300;
//    init_config();
//    while(1){
//    
//    for(int i=0;i<100000;i++);
//    if (AD_ReadADPin(IR_PIN_1) < ir_threshold) {
//        printf("read IR-1 v5:%d\n", AD_ReadADPin(IR_PIN_1));
//        RC_SetPulseTime(SERVO_PIN_1, 1000);
//    }
//    while(!AD_IsNewDataReady());
//    if (AD_ReadADPin(IR_PIN_2) < ir_threshold) {
//        printf("read IR-2 v6:%d\n", AD_ReadADPin(IR_PIN_2));
//        RC_SetPulseTime(SERVO_PIN_1, 1200);
//    }
//    while(!AD_IsNewDataReady());
//    if (AD_ReadADPin(IR_PIN_3) < ir_threshold) {
//        printf("read IR-3 v8:%d\n", AD_ReadADPin(IR_PIN_3));
//        RC_SetPulseTime(SERVO_PIN_1, 1400);
//    }
//    while(!AD_IsNewDataReady());
//    if (AD_ReadADPin(IR_PIN_4) < ir_threshold) {
//        printf("read IR-4 w8:%d\n", AD_ReadADPin(IR_PIN_4));
//        RC_SetPulseTime(SERVO_PIN_1, 1600);
//    }
//    }
//    //*************************IRSENSORTEST
//    
//    //*************************LEDTEST
//    IO_PortsSetPortOutputs(PORTZ, (PIN3|PIN4|PIN5|PIN11|PIN7|PIN8|PIN9));
//    char input = GetChar();
//    while(1){
//        input = GetChar();
//        switch (input) {
//            case '1':
//                printf("received 1\n");
//                IO_PortsWritePort(PORTZ, PIN3);
//                break;
//            case '2':
//                printf("received 2\n");
//                IO_PortsWritePort(PORTZ, PIN4);
//                break;
//            case '3':
//                printf("received 3\n");
//                IO_PortsWritePort(PORTZ, PIN5);
//                break;
//            case '4':
//                printf("received 4\n");
//                IO_PortsWritePort(PORTZ, PIN11);
//                break;
//            case '5':
//                printf("received 5\n");
//                IO_PortsWritePort(PORTZ, PIN7);
//                break;
//            case '6':
//                printf("received 6\n");
//                IO_PortsWritePort(PORTZ, PIN8);
//                break;
//            case '7':
//                printf("received 7\n");
//                IO_PortsWritePort(PORTZ, PIN9);
//                break;
//            default:
//                break;
//        }
//    }
//    //********************************LEDTEST
//    
//    
//    
//    while(!AD_IsNewDataReady());
//    //printf("read TG-1 x5:%d\n", (IO_PortsReadPort(PORTX)&0b100000)>>5);
//    while(!AD_IsNewDataReady());
//    //printf("read PW-1 x6:%d\n", (IO_PortsReadPort(PORTX)&0b1000000)>>6);
//    
//    
//    while(1);
#ifdef comment
    printf("Init all hardware.\n");
#endif
    ES_Timer_Init();
    init_config();
    shutdown();
#ifdef LED_TEST
    led_show(LED_1, on);
    led_show(LED_2, on);
    led_show(LED_3, on);
    led_show(LED_4, on);
    led_record(add);
    led_record(add);
    led_record(add);
    while(1);
#endif
    
#ifdef IR2_TEST
    while(1){
        for(int i=0;i<50000;i++);
        printf("%d\n", AD_ReadADPin(IR_PIN_2));
    }      
#endif
    
#ifdef SERVO_TEST
    recover();
    servo_control(SERVO_PIN_1, SERVO_LOAD);
    while(1);     
#endif
    
#ifdef comment
    printf("set IR timer in 50 hz.\n");
#endif
    ES_Timer_InitTimer(6, 20);

#ifdef comment
    printf("set power & trigger timer in 4 hz.\n");
#endif
    ES_Timer_InitTimer(8, 250);
    
#ifdef comment
    printf("set general call in 25 hz.\n");
#endif    
    ES_Timer_InitTimer(9, 40);
    
    // Your hardware initialization function calls go here

    // now initialize the Events and Services Framework and start it running
    ErrorType = ES_Initialize();
    if (ErrorType == Success) {
        ErrorType = ES_Run();

    }
    //if we got to here, there was an error
    switch (ErrorType) {
    case FailedPointer:
        printf("Failed on NULL pointer");
        break;
    case FailedInit:
        printf("Failed Initialization");
        break;
    default:
        printf("Other Failure: %d", ErrorType);
        break;
    }
    for (;;)
        ;

};

/*------------------------------- Footnotes -------------------------------*/
/*------------------------------ End of file ------------------------------*/
