// Line 414. test case when w is pressed, remove other cases
#include <roach.h>
#include <BOARD.h>
#include <xc.h>


#include <pwm.h>
#include <serial.h>
#include <AD.h>

/*******************************************************************************
 * PRIVATE #DEFINES                                                            *
 ******************************************************************************/

#define LEFT_DIR LATBbits.LATB3
#define LEFT_DIR_INV LATBbits.LATB2
#define RIGHT_DIR LATEbits.LATE5
#define RIGHT_DIR_INV LATEbits.LATE6

#define HALL_FRONT_LEFT _RB8
#define HALL_FRONT_RIGHT _RD9
#define HALL_REAR_RIGHT _RD8
#define HALL_REAR_LEFT _RB10

#define LEFT_DIR_TRIS _TRISB3
#define LEFT_DIR_INV_TRIS _TRISB2
#define RIGHT_DIR_TRIS _TRISE5
#define RIGHT_DIR_INV_TRIS _TRISE6

#define HALL_FRONT_LEFT_TRIS _TRISB8
#define HALL_FRONT_RIGHT_TRIS _TRISD9
#define HALL_REAR_RIGHT_TRIS _TRISD8
#define HALL_REAR_LEFT_TRIS _TRISB10

#define LEFT_PWM PWM_PORTY10
#define RIGHT_PWM PWM_PORTY12

#define LIGHT_SENSOR ROACH_LIGHT_SENSOR
#define ROACH_BAT_VOLTAGE BAT_VOLTAGE

#define FORWARD 'w'
#define BACKWARD 's'
#define SPIN 'a'
#define LED_1 '1'
#define LED_2 '2'
#define LED_3 '3'
#define LED_4 '4'
#define LED_5 '5'
#define LED_6 '6'
#define LED_7 '7'
#define LED_8 '8'
#define LED_9 '9'
#define LED_10 '0'
#define LED_11 '-'
#define LED_12 '='
#define LIGHT 'l'



//light bar defines
#define NUMLEDS 12

#define LED_SetPinOutput(i) *LED_TRISCLR[i] = LED_bitsMap[i]
#define LED_SetPinInput(i) *LED_TRISSET[i] = LED_bitsMap[i];
#define LED_On(i) *LED_LATCLR[(unsigned int)i] = LED_bitsMap[(unsigned int)i];
#define LED_Off(i) *LED_LATSET[(unsigned int)i] = LED_bitsMap[(unsigned int)i];
#define LED_Get(i) (*LED_LAT[(unsigned int)i]&LED_bitsMap[(unsigned int)i])

/*******************************************************************************
 * PRIVATE VARIABLES                                                           *
 ******************************************************************************/

typedef union {

    struct {
        unsigned bit0 : 1;
        unsigned bit1 : 1;
        unsigned bit2 : 1;
        unsigned bit3 : 1;
        unsigned bit4 : 1;
        unsigned bit5 : 1;
        unsigned bit6 : 1;
        unsigned bit7 : 1;
        unsigned bit8 : 1;
        unsigned bit9 : 1;
        unsigned bit10 : 1;
        unsigned bit11 : 1;
        unsigned : 4;
    };
    uint16_t c;
} LEDBank_t;


static volatile unsigned int * const LED_TRISCLR[] = {&TRISECLR, &TRISDCLR, &TRISDCLR, &TRISDCLR,
    &TRISDCLR, &TRISDCLR, &TRISDCLR, &TRISFCLR, &TRISFCLR, &TRISGCLR, &TRISFCLR, &TRISFCLR};

static volatile unsigned int * const LED_TRISSET[] = {&TRISESET, &TRISDSET, &TRISDSET, &TRISDSET,
    &TRISDSET, &TRISDSET, &TRISDSET, &TRISFSET, &TRISFSET, &TRISGSET, &TRISFSET, &TRISFSET};

static volatile unsigned int * const LED_LATCLR[] = {&LATECLR, &LATDCLR, &LATDCLR, &LATDCLR,
    &LATDCLR, &LATDCLR, &LATDCLR, &LATFCLR, &LATFCLR, &LATGCLR, &LATFCLR, &LATFCLR};

static volatile unsigned int * const LED_LATSET[] = {&LATESET, &LATDSET, &LATDSET, &LATDSET,
    &LATDSET, &LATDSET, &LATDSET, &LATFSET, &LATFSET, &LATGSET, &LATFSET, &LATFSET};

static volatile unsigned int * const LED_LAT[] = {&LATE, &LATD, &LATD, &LATD,
    &LATD, &LATD, &LATD, &LATF, &LATF, &LATG, &LATF, &LATF};

static unsigned short int LED_bitsMap[] = {BIT_7, BIT_5, BIT_10, BIT_11, BIT_3, BIT_6, BIT_7, BIT_6, BIT_4, BIT_6, BIT_5, BIT_1};

//static unsigned short int LED_ShiftAmount[] = {7, 5, 10, 11, 3, 6, 7, 6, 4, 6, 5, 1};


/*******************************************************************************
 * PUBLIC FUNCTIONS                                                           *
 ******************************************************************************/

/**
 * @Function Roach_Init(void)
 * @param None.
 * @return None.
 * @brief  Performs all the initialization necessary for the roach. this includes initializing 
 * the PWM module, the A/D converter, the data directions on some pins, and 
 * setting the initial motor directions.
 * @note  None.
 * @author Max Dunne, 2012.01.06 */
void Roach_Init(void)
{

    //set the control pins for the motors
    PWM_Init();
    PWM_SetFrequency(1000);
    PWM_AddPins(LEFT_PWM | RIGHT_PWM);

    LEFT_DIR_TRIS = 0;
    LEFT_DIR_INV_TRIS = 0;
    RIGHT_DIR_TRIS = 0;
    RIGHT_DIR_INV_TRIS = 0;
    LEFT_DIR = 0;
    LEFT_DIR_INV = ~LEFT_DIR;
    RIGHT_DIR = 0;
    RIGHT_DIR_INV = ~RIGHT_DIR;

    //set up the hall effect sensors
    HALL_FRONT_LEFT_TRIS = 1;
    HALL_FRONT_RIGHT_TRIS = 1;
    HALL_REAR_RIGHT_TRIS = 1;
    HALL_REAR_LEFT_TRIS = 1;

    //set up the light bank

    uint8_t CurPin;
    for (CurPin = 0; CurPin < NUMLEDS; CurPin++) {
        LED_SetPinOutput(CurPin);
        LED_Off(CurPin);
    }
    //while (1);

    //Initialize the light sensor
    AD_Init();
    //    printf("Current pins: %d\n",AD_ActivePins());
    //    printf("Add Result: %d\n",AD_AddPins(LIGHT_SENSOR));
    //    while(1);
    AD_AddPins(LIGHT_SENSOR);

    //enable interrupts
}

/**
 * @Function Roach_LeftMtrSpeed(char newSpeed)
 * @param newSpeed - A value between -100 and 100 which is the new speed
 * @param of the motor. 0 stops the motor. A negative value is reverse.
 * @return SUCCESS or ERROR
 * @brief  This function is used to set the speed and direction of the left motor.
 * @author Max Dunne, 2012.01.06 */
char Roach_LeftMtrSpeed(char newSpeed)
{
    if ((newSpeed < -ROACH_MAX_SPEED) || (newSpeed > ROACH_MAX_SPEED)) {
        return (ERROR);
    }
    newSpeed = -newSpeed;
  
    if (newSpeed < 0) {
        LEFT_DIR = 0;
        newSpeed = newSpeed * (-1); // set speed to a positive value
    } else {
        LEFT_DIR = 1;
    }
    LEFT_DIR_INV = ~(LEFT_DIR);
    if (PWM_SetDutyCycle(LEFT_PWM, newSpeed * (MAX_PWM / ROACH_MAX_SPEED)) == ERROR) {
        //printf("ERROR: setting channel 1 speed!\n");
        return (ERROR);
    }
    return (SUCCESS);
}

/**
 * @Function Roach_RightMtrSpeed(char newSpeed)
 * @param newSpeed - A value between -100 and 100 which is the new speed
 * @param of the motor. 0 stops the motor. A negative value is reverse.
 * @return SUCCESS or ERROR
 * @brief  This function is used to set the speed and direction of the left motor.
 * @author Max Dunne, 2012.01.06 */
char Roach_RightMtrSpeed(char newSpeed)
{
    if ((newSpeed < -ROACH_MAX_SPEED) || (newSpeed > ROACH_MAX_SPEED)) {
        return (ERROR);
    }
    if (newSpeed < 0) {
        RIGHT_DIR = 0;
        newSpeed = newSpeed * (-1); // set speed to a positive value
    } else {
        RIGHT_DIR = 1;
    }
    RIGHT_DIR_INV = ~(RIGHT_DIR);
    if (PWM_SetDutyCycle(RIGHT_PWM, newSpeed * (MAX_PWM / ROACH_MAX_SPEED)) == ERROR) {
        //puts("\aERROR: setting channel 1 speed!\n");
        return (ERROR);
    }
    return (SUCCESS);
}

/**
 * @Function Roach_LightLevel(void)
 * @param None.
 * @return a 10-bit value corresponding to the amount of light received.
 * @brief  Returns the current light level. A higher value means less light is detected.
 * @author Max Dunne, 2012.01.06 */
unsigned int Roach_LightLevel(void)
{
    return AD_ReadADPin(LIGHT_SENSOR);
}

/**
 * @Function Roach_BatteryVoltage(void)
 * @param None.
 * @return a 10-bit value corresponding to the current voltage of the roach
 * @brief  returns a 10:1 scaled value of the roach battery level
 * @author Max Dunne, 2013.07.12 */
unsigned int Roach_BatteryVoltage(void)
{
    return AD_ReadADPin(ROACH_BAT_VOLTAGE);
}

/**
 * @Function Roach_ReadFrontLeftBumper(void)
 * @param None.
 * @return BUMPER_TRIPPED or BUMPER_NOT_TRIPPED
 * @brief  Returns the state of the front left bumper
 * @author Max Dunne, 2012.01.06 */
unsigned char Roach_ReadFrontLeftBumper(void)
{
    return !HALL_FRONT_LEFT;
}

/**
 * @Function Roach_ReadFrontRightBumper(void)
 * @param None.
 * @return BUMPER_TRIPPED or BUMPER_NOT_TRIPPED
 * @brief  Returns the state of the front right bumper
 * @author Max Dunne, 2012.01.06 */
unsigned char Roach_ReadFrontRightBumper(void)
{
    return !HALL_FRONT_RIGHT;
}

/**
 * @Function Roach_ReadRearLeftBumper(void)
 * @param None.
 * @return BUMPER_TRIPPED or BUMPER_NOT_TRIPPED
 * @brief  Returns the state of the rear left bumper
 * @author Max Dunne, 2012.01.06 */
unsigned char Roach_ReadRearLeftBumper(void)
{
    return !HALL_REAR_LEFT;
}

/**
 * @Function Roach_ReadRearRightBumper(void)
 * @param None.
 * @return BUMPER_TRIPPED or BUMPER_NOT_TRIPPED
 * @brief  Returns the state of the rear right bumper
 * @author Max Dunne, 2012.01.06 */
unsigned char Roach_ReadRearRightBumper(void)
{
    return !HALL_REAR_RIGHT;
}

/**
 * @Function Roach_ReadBumpers(void)
 * @param None.
 * @return 4-bit value representing all four bumpers in following order: front left,front right, rear left, rear right
 * @brief  Returns the state of all 4 bumpers
 * @author Max Dunne, 2012.01.06 */
unsigned char Roach_ReadBumpers(void)
{
    //unsigned char bump_state;
    //bump_state = (!HALL_FRONT_LEFT + ((!HALL_FRONT_RIGHT) << 1)+((!HALL_REAR_LEFT) << 2)+((!HALL_REAR_RIGHT) << 3));
    return (!HALL_FRONT_LEFT + ((!HALL_FRONT_RIGHT) << 1)+((!HALL_REAR_LEFT) << 2)+((!HALL_REAR_RIGHT) << 3));
}

/**
 * @Function Roach_LEDSSet( unsigned char pattern)
 * @param pattern - sets LEDs on (1) or off (0) as in the pattern.
 * @return SUCCESS or ERROR
 * @brief  Forces the LEDs in (bank) to on (1) or off (0) to match the pattern.
 * @author Gabriel Hugh Elkaim, 2011.12.25 01:16 Max Dunne 2015.09.18 */
char Roach_LEDSSet(uint16_t pattern)
{
    char i;
    for (i = 0; i < NUMLEDS; i++) {
        if (pattern & (1 << i)) {
            LED_On(i);
        } else {
            LED_Off(i);
        }
    }
    return SUCCESS;
}

/**
 * @Function Roach_LEDSGet(void)
 * @return uint16_t: ERROR or state of BANK
 * @author Max Dunne, 203.10.21 01:16 2015.09.18 */
uint16_t Roach_LEDSGet(void)
{
    uint16_t LEDStatus = 0;
    int8_t i;
    for (i = (NUMLEDS - 1); i >= 0; i--) {
        LEDStatus |= !LED_Get(i);
        LEDStatus <<= 1;
        //        printf("%d\t",i);
    }
    LEDStatus >>= 1;
    return LEDStatus;
    return 0;
}

/**
 * @Function Roach_BarGraph(uint8_t Number)
 * @param Number - value to light between 0 and 12 leds
 * @return SUCCESS or ERROR
 * @brief  allows all leds to be used as a bar graph
 * @author  Max Dunne 2015.09.18 */


char Roach_BarGraph(uint8_t Number)
{
    if (Number > NUMLEDS) {
        return ERROR;
    }
    uint16_t Pattern = 0;
    uint8_t iterations;

    for (iterations = 0; iterations < Number; iterations++) {
        Pattern <<= 1;
        Pattern |= 1;
    }
    Roach_LEDSSet(Pattern);
    return SUCCESS;
}

#define ROACH_TEST
#ifdef ROACH_TEST


#include <stdio.h>

//#define WANDER_MODE
#define WANDER_SAMPLE_RATE 100

#define FLEFT_BUMP_MASK (1)
#define FRIGHT_BUMP_MASK (1<<1)
#define RLEFT_BUMP_MASK (1<<2)
#define RRIGHT_BUMP_MASK (1<<3)

#define DELAY(x)    for (wait = 0; wait <= x; wait++) {asm("nop");}
#define A_BIT       18300
#define A_BIT_MORE  36600
#define YET_A_BIT_LONGER (A_BIT_MORE<<2)
#define A_LOT       183000
#define NUM_TIMES_REPEAT_LED 5
#define MOTOR_TIME (A_LOT<<2)

#define LOW_BAT 263
#define HIGH_BAT 310



#include <roach.h>

void FlashLEDBar(uint8_t numtimes);

int main(void)
{
    BOARD_Init();
    Roach_Init();

    unsigned int wait;
    unsigned int i, j;
    unsigned int scaledValue;
    int motorSpeed;
    printf("\r\nPart 4 Test Harness");
    //  TEST THE LEDS
    for (i = 0; i < NUM_TIMES_REPEAT_LED; i++) {
        for (j = 0; j < NUMLEDS; j++) {
            Roach_LEDSSet(1 << j);
            DELAY(A_BIT_MORE);
        }
        Roach_LEDSSet(0);
        DELAY(A_BIT_MORE);
    }
    Roach_LEDSSet(0x555);
    DELAY(YET_A_BIT_LONGER);
    for (i = 0; i < NUM_TIMES_REPEAT_LED; i++) {
        Roach_LEDSSet(Roach_LEDSGet()^0xFFF); // toggle bits on LED
        DELAY(YET_A_BIT_LONGER);
    }
    Roach_LEDSSet(0);
    while (1) {
        
        switch (GetChar()) {

            case FORWARD: // Forward when 'w' clicked
                FlashLEDBar(1);
                motorSpeed = 100;
                DELAY(A_LOT);
                printf("\r\nBoth Motor at %d", motorSpeed);
                Roach_LeftMtrSpeed(motorSpeed);
                Roach_RightMtrSpeed(motorSpeed);
                Roach_LEDSSet(0b111100000000);
                printf("\r\nForward Test Complete");
                FlashLEDBar(1);
                break;
                
            case BACKWARD: // Backward when 's' clicked
                FlashLEDBar(2);
                motorSpeed = -100;
                DELAY(A_LOT);
                printf("\r\nBoth Motor at %d", motorSpeed);
                Roach_LeftMtrSpeed(motorSpeed);
                Roach_RightMtrSpeed(motorSpeed);
                Roach_LEDSSet(0b111100000000);
                printf("\r\nBackward Test Complete");
                FlashLEDBar(2);
                break;
                
            case SPIN: // Spin when 'a' clicked
                FlashLEDBar(3);
                motorSpeed = 100;
                DELAY(A_LOT);
                printf("\r\nLeft Motor at -%d to rotate", motorSpeed);
                printf("\r\nRight Motor at %d to rotate", motorSpeed);
                Roach_LeftMtrSpeed(-1*motorSpeed);
                Roach_RightMtrSpeed(motorSpeed);
                Roach_LEDSSet(0b111100000000);
                printf("\r\nSpin Test Complete");
                FlashLEDBar(3);
                break;
            
            case LED_1: // Turns on left most LED
                motorSpeed = 0;
                Roach_LEDSSet(0b000000000001);
                Roach_LeftMtrSpeed(motorSpeed);
                Roach_RightMtrSpeed(motorSpeed);
                printf("\r\nled 1 TEST");
                break;
            case LED_2:
                motorSpeed = 0;
                Roach_LEDSSet(0b000000000010);
                Roach_LeftMtrSpeed(motorSpeed);
                Roach_RightMtrSpeed(motorSpeed);
                printf("\r\nled 2 TEST");
                break;
            case LED_3:
                motorSpeed = 0;
                Roach_LEDSSet(0b000000000100);
                Roach_LeftMtrSpeed(motorSpeed);
                Roach_RightMtrSpeed(motorSpeed);
                printf("\r\nled 3 TEST");
                break;
            case LED_4:
                motorSpeed = 0;
                Roach_LEDSSet(0b000000001000);
                Roach_LeftMtrSpeed(motorSpeed);
                Roach_RightMtrSpeed(motorSpeed);
                printf("\r\nled 4 TEST");
                break;
            case LED_5:
                motorSpeed = 0;
                Roach_LEDSSet(0b000000010000);
                Roach_LeftMtrSpeed(motorSpeed);
                Roach_RightMtrSpeed(motorSpeed);
                printf("\r\nled 5 TEST");
                break;
            case LED_6:
                motorSpeed = 0;
                Roach_LEDSSet(0b000000100000);
                Roach_LeftMtrSpeed(motorSpeed);
                Roach_RightMtrSpeed(motorSpeed);
                printf("\r\nled 6 TEST");
                break;
            case LED_7:
                motorSpeed = 0;
                Roach_LEDSSet(0b0000001000000);
                Roach_LeftMtrSpeed(motorSpeed);
                Roach_RightMtrSpeed(motorSpeed);
                printf("\r\nled 7 TEST");
                break;
            case LED_8:
                motorSpeed = 0;
                Roach_LEDSSet(0b000001000000);
                Roach_LeftMtrSpeed(motorSpeed);
                Roach_RightMtrSpeed(motorSpeed);
                printf("\r\nled 8 TEST");
                break;
            case LED_9:
                motorSpeed = 0;
                Roach_LEDSSet(0b000010000000);
                Roach_LeftMtrSpeed(motorSpeed);
                Roach_RightMtrSpeed(motorSpeed);
                printf("\r\nled 9 TEST");
                break;
            case LED_10:
                motorSpeed = 0;
                Roach_LEDSSet(0b001000000000);
                Roach_LeftMtrSpeed(motorSpeed);
                Roach_RightMtrSpeed(motorSpeed);
                printf("\r\nled 10 TEST");
                break;
            case LED_11:
                motorSpeed = 0;
                Roach_LEDSSet(0b010000000000);
                Roach_LeftMtrSpeed(motorSpeed);
                Roach_RightMtrSpeed(motorSpeed);
                printf("\r\nled 11 TEST");
                break;
            case LED_12: // Turns on right most LED
                motorSpeed = 0;
                Roach_LEDSSet(0b100000000000);
                Roach_LeftMtrSpeed(motorSpeed);
                Roach_RightMtrSpeed(motorSpeed);
                printf("\r\nled 12 TEST");
                break;
            case LIGHT:
                motorSpeed = 0;
                Roach_LeftMtrSpeed(motorSpeed);
                Roach_RightMtrSpeed(motorSpeed);
                scaledValue = Roach_LightLevel();
                printf("\r\nCurrent Light Level: %d", scaledValue);
             
        }
        switch (Roach_ReadBumpers()) {

        case FLEFT_BUMP_MASK: // Front Left bumper
                FlashLEDBar(4);
                motorSpeed = -100;
                DELAY(A_LOT);
                printf("\r\nLeft Motor at %d", motorSpeed);
                Roach_LeftMtrSpeed(motorSpeed);
                Roach_LEDSSet(0b111100000000);
                motorSpeed = 0;
                Roach_RightMtrSpeed(motorSpeed);
                printf("\r\nFront Left Bumper Test Complete");
                FlashLEDBar(4);
            break;

        case FRIGHT_BUMP_MASK: // Front Right bumper
            FlashLEDBar(5);
            motorSpeed = -100;
            DELAY(A_LOT);
            printf("\r\nRight Motor at %d", motorSpeed);
            Roach_RightMtrSpeed(motorSpeed);
            Roach_LEDSSet(0b111100000000);
            motorSpeed = 0;
            Roach_LeftMtrSpeed(motorSpeed);
            printf("\r\nFront Right Bumper Test Complete");
            FlashLEDBar(5);
            break;

        case RLEFT_BUMP_MASK: // Rear Left bumper
            FlashLEDBar(6);
            motorSpeed = 100;
            DELAY(A_LOT);
            printf("\r\nLeft Motor at %d", motorSpeed);
            Roach_LeftMtrSpeed(motorSpeed);
            Roach_LEDSSet(0b111100000000);
            motorSpeed = 0;
            Roach_RightMtrSpeed(motorSpeed);
            printf("\r\nRear Left Bumper Test Complete");
            FlashLEDBar(6);
            break;

        case RRIGHT_BUMP_MASK: // Rear Right bumper
            FlashLEDBar(7);
            motorSpeed = 100;
            DELAY(A_LOT);
            printf("\r\nRight Motor at %d", motorSpeed);
            Roach_RightMtrSpeed(motorSpeed);
            Roach_LEDSSet(0b000000000000);
            motorSpeed = 0;
            Roach_LeftMtrSpeed(motorSpeed);
            printf("\r\nRear Right Bumper Test Complete");
            FlashLEDBar(7);
            break;
            
        }


    }
    while (1);
}

void FlashLEDBar(uint8_t numtimes)
{
    unsigned int wait, i;
    Roach_LEDSSet(0);
    for (i = 0; i < numtimes; i++) {
        Roach_LEDSSet(0xFFF);
        DELAY(YET_A_BIT_LONGER);
        Roach_LEDSSet(0x000);
        DELAY(YET_A_BIT_LONGER);
    }
}
#endif
