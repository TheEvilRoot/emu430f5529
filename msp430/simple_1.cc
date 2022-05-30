#include <msp430.h> 

#define FIRST_PRESSED !(P1IN&BIT7)
#define SECOND_PRESSED !(P2IN&BIT2)

#define FIRST_NOT_PRESSED (P1IN & BIT7)
#define SECOND_NOT_PRESSED (P2IN & BIT2)

#define SET_LED3_ON (P8OUT |= BIT2)
#define SET_LED3_OFF (P8OUT &= ~BIT2)
#define TOGGLE_LED3 (P8OUT ^= BIT2)

#define SET_LED2_ON (P8OUT |= BIT1)
#define SET_LED2_OFF (P8OUT &= ~BIT1)
#define TOGGLE_LED2 (P8OUT ^= BIT1)

#define SET_FIRST_ON_DOWN (P1IES &= ~BIT7)
#define SET_FIRST_ON_UP (P1IES |= BIT7)

#define SET_SECOND_ON_DOWN (P2IES &= ~BIT2)
#define SET_SECOND_ON_UP (P2IES |= BIT2)

/**
 * main.c
 */

#pragma vector = PORT1_VECTOR
__interrupt void __attribute__((interrupt(PORT1_VECTOR))) butt1(void) {
    //clear interrupt flag
    P1IFG &= ~BIT7;
	if (FIRST_PRESSED) {
		SET_LED3_ON;
		SET_FIRST_ON_UP;
	} else {
		SET_LED3_OFF;
		SET_FIRST_ON_DOWN;
	}
}

#pragma vector = PORT2_VECTOR
__interrupt void __attribute__((interrupt(PORT2_VECTOR))) butt2(void){
    P2IFG &= ~BIT2; //clear interrupt flag
	if (SECOND_PRESSED) {
		SET_LED2_ON;
		SET_SECOND_ON_UP;
	} else {
		SET_LED2_OFF;
		SET_SECOND_ON_DOWN;
	}
}

int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer

	P8DIR |= BIT2; //led 3

	P8DIR |= BIT1; //led 2

	__bis_SR_register(GIE); // for enable interrupts

    //button 1
    P1DIR &= ~BIT7;
    P1REN |= BIT7;
    P1OUT |= BIT7;

    P1IFG &= ~BIT7; //clear interrupt flag
    P1IE |= BIT7;   //allow interrupt for port


    //button 2
    P2DIR &= ~BIT2;
    P2REN |= BIT2;
    P2OUT |= BIT2;

    P2IFG &= ~BIT2; //clear interrupt flag
    P2IE |= BIT2;

    SET_LED3_OFF;
    SET_LED2_OFF;

    SET_FIRST_ON_DOWN;
    SET_SECOND_ON_DOWN;

    __no_operation();
    return 0;
}
