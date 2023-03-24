/*
  TODO: Doc
*/

/*----------------------------------------------------------
    Pin defines
----------------------------------------------------------*/

#define LED_PIN PIN5
#define BUZZ_PIN PIN4
#define PR_PIN PIN3
#define PORT   sfr_PORTD

/*----------------------------------------------------------
    INCLUDE FILES
----------------------------------------------------------*/
#include "STM8S103F3.h"


// toggle specified pin. Pass port struct as pointer
void toggle_pin(PORT_t *port, uint8_t pin) {

    port->ODR.byte ^= pin;

} // toggle_pin

uint16_t read_pr() {
    uint16_t value;
    uint8_t drh = sfr_ADC1.DRH.byte;
    uint8_t drl = sfr_ADC1.DRL.byte;
    value = ((uint16_t)drh << 2  | (uint16_t)drl);
    return value;
}

void init() {
    // Init GPIO.

    // Only output pins are set to 1, all other must be 0
    sfr_PORTD.DDR.byte = 0x0 | LED_PIN | BUZZ_PIN;
    // 
    sfr_PORTD.CR1.byte = 0;
    sfr_PORTD.CR2.byte = 0;

    // GPIO used for ADC
    sfr_PORTD.CR1.byte = 0;
    sfr_PORTD.CR2.byte = 0;

    // Init ADC
    // Disable Schmitt trigger to save some power
    sfr_ADC1.TDRH.TD = 2;
    sfr_ADC1.TDRL.TL = 0;
    // Switch the ADC on
    sfr_ADC1.CR1.ADON = 1;

    return;
}

void main (void) {

    init();

    uint32_t  i = 0;

    // main loop
    while(1) {
  
        // toggle LED. Pass port struct as pointer
        // toggle_pin(&LED_PORT, LED_PIN);
        read_pr();
        // simple wait ~500ms @ 16MHz
        for (i=0; i<300000L; i++)
          NOP();
  
    } // main loop

} // main

/*-----------------------------------------------------------------------------
    END OF MODULE
-----------------------------------------------------------------------------*/
