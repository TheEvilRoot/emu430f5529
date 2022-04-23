#include <msp430.h>

#define FIRST_PRESSED !(P1IN&BIT7)
#define SECOND_PRESSED !(P2IN&BIT2)

#define FIRST_NOT_PRESSED (P1IN & BIT7)
#define SECOND_NOT_PRESSED (P2IN & BIT2)

#define SET_LED3_ON (P8OUT |= BIT2)
#define SET_LED3_OFF (P8OUT &= ~BIT2)

#define SET_LED2_ON (P8OUT |= BIT1)
#define SET_LED2_OFF (P8OUT &= ~BIT1)

/**
 * main.c
 */


int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer

    P8DIR |= BIT2;
    P8DIR |= BIT1;

    //button 1
    P1DIR &= ~BIT7;
    P1REN |= BIT7;
    P1OUT |= BIT7;

    //button 2
    P2DIR &= ~BIT2;
    P2REN |= BIT2;
    P2OUT |= BIT2;

    SET_LED3_OFF;
    SET_LED2_OFF;

    int first_was_pressed = FIRST_PRESSED;
    int second_was_pressed = SECOND_PRESSED;

    while (1) {
        int first_pressed = FIRST_PRESSED;
        int second_pressed = SECOND_PRESSED;

        // S1 up
        if (first_was_pressed && !first_pressed) {
            // S2 U
            if (SECOND_NOT_PRESSED) {
                // TODO: maybe delay
                SET_LED3_ON;
            }
            SET_LED2_OFF;
        }

        // S1 down
        if (!first_was_pressed && first_pressed) {
            // S2 U
            if (SECOND_NOT_PRESSED) {
                // TODO: maybe delay
                SET_LED3_OFF;
            }
        }

        // S2 down
        if (!second_was_pressed && second_pressed) {
            // S1 D
            if (FIRST_PRESSED) {
                // TODO: maybe delay
                SET_LED2_ON;
            }
        }

        first_was_pressed = first_pressed;
        second_was_pressed = second_pressed;
    }
    __no_operation();
    return 0;
}
