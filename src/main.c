/*
  The main task of this? Blinking and beeping in a somewhat obscuring manner.
  The device will beep and blink in short bursts with a pattern that'll vary every time.
  Which pattern will be used is chosen for the LED and the buzzer individually.
  Patterns are described in the look up table below.
*/

/*----------------------------------------------------------
    Defines
----------------------------------------------------------*/

#define LED_PIN PIN5
#define BUZZ_PIN PIN4
#define PR_PIN PIN3
#define CONFIG_PIN PIN2
#define PORT   sfr_PORTD
#define DELAY_SCALER 1024
#define PATTERN_LENGTH 12

/*----------------------------------------------------------
    Include files
----------------------------------------------------------*/
#include "STM8S103F3.h"

/*----------------------------------------------------------
    Global variables
----------------------------------------------------------*/
uint8_t* pattern[PATTERN_LENGTH][PATTERN_LENGTH] = {
    {51, 19, 124, 33, 39, 173, 194, 143, 61, 175, 203, 59},
    {148, 106, 56, 49, 182, 16, 51, 120, 195, 45, 107, 221},
    {178, 198, 5, 223, 63, 238, 241, 180, 226, 232, 55, 40},
    {43, 186, 115, 75, 250, 73, 3, 130, 184, 150, 117, 9},
    {75, 41, 17, 74, 217, 39, 78, 180, 55, 119, 82, 117},
    {114, 152, 46, 241, 159, 154, 244, 100, 213, 183, 140, 229},
    {36, 1, 203, 37, 43, 11, 163, 66, 72, 227, 136, 24},
    {55, 236, 23, 18, 2, 233, 106, 139, 145, 15, 51, 28},
    {76, 108, 182, 69, 231, 17, 30, 106, 163, 61, 134, 226},
    {248, 195, 86, 116, 31, 234, 58, 236, 27, 17, 75, 255},
    {71, 95, 70, 90, 116, 123, 196, 62, 253, 16, 164, 234},
    {14, 148, 216, 42, 9, 252, 129, 211, 114, 128, 182, 63}
};

uint32_t index = 0;

/*----------------------------------------------------------
    Functions
----------------------------------------------------------*/

void toggle_pin(PORT_t *port, uint8_t pin) {
    port->ODR.byte ^= pin;
}

/* Will return the measured AD value of the photo resistor */
uint16_t read_pr() {
    uint16_t value;
    uint8_t drh = sfr_ADC1.DRH.byte;
    uint8_t drl = sfr_ADC1.DRL.byte;
    value = ((uint16_t)drh << 2  | (uint16_t)drl);
    return value;
}

/* Will return 1 if the CFG jumper is set, otherwise 0 */
uint8_t read_cfg() {
    uint8_t value = 0;
    uint8_t drh = sfr_ADC1.DRH.byte;
    return value;
}

/* Will delay by No-Oping */
void delay(uint8_t units) {
    /*  Yes, this is a stupid nested loop. The max. bit width in the registers is 16 bit,#
        which isn't enough for the delays we're looking at. */
    for (int i = 1024; i>0; i--) {
        for (int j = units; j>0; j--) {
            NOP();
        }
    }
}

void init_GPIO() {
    // Init GPIO.

    // Only output pins are set to 1, all other must be 0
    sfr_PORTD.DDR.byte = 0x0 | LED_PIN | BUZZ_PIN;
    // 
    sfr_PORTD.CR1.byte = 0x0 | LED_PIN | BUZZ_PIN;
    sfr_PORTD.CR2.byte = 0;

    // GPIO used for ADC
    // sfr_PORTD.CR1.byte = 0;
    // sfr_PORTD.CR2.byte = 0;

    // Init ADC
    // Disable Schmitt trigger to save some power
    sfr_ADC1.TDRH.TD = 2;
    sfr_ADC1.TDRL.TL = 0;
    // Switch the ADC on
    sfr_ADC1.CR1.ADON = 1;

    return;
}

void init_AWU() {
    // Measuring the LS clock frequency isn't required, as the timing doesn't need to be exact
    // Selecting the highest time base here, we want the full 30s
    sfr_AWU.TBR.AWUTB = 0xf;
    // Same thing, we're just not allowed to take 0x3f, which is the reset value
    sfr_AWU.APR.APR = 0x3e;
    // Set the AWUEN bit in the Control/status register (AWU_CSR)
    sfr_AWU.CSR1.AWUEN = 1;

    // ENABLE_INTERRUPTS();
}

void main (void) {

    init_GPIO();
    init_AWU();
    read_pr();
    read_cfg();

    toggle_pin(&PORT, LED_PIN);
    delay(255);
    toggle_pin(&PORT, BUZZ_PIN);
    delay(255);
    toggle_pin(&PORT, LED_PIN);
    delay(255);
    toggle_pin(&PORT, BUZZ_PIN);
    delay(255);

    while (1)
    {
        // Do the beepy blinky thing
        for (int i = 0; i < PATTERN_LENGTH; i++)
        {
            toggle_pin(&PORT, LED_PIN);
            delay((uint8_t)pattern[index % PATTERN_LENGTH][i]);
            toggle_pin(&PORT, BUZZ_PIN);
        }
        index++;

        // Go to sleep
        ENTER_HALT();

        // reset wakeup flag
        sfr_AWU.CSR1.AWUF = 0;
        sfr_AWU.CSR1.AWUEN = 1;
    }

}

/*-----------------------------------------------------------------------------
    END OF MODULE
-----------------------------------------------------------------------------*/
