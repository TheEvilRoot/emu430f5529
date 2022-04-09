#include <msp430.h>
#include <msp430x552x.h>
int main() {
    int value = 0x3;
    P8DIR |= value;
    while (true) {
        P8OUT &= ~0x3;
        P8OUT |= value;
        value = ~value;
        value = value & 0x3;
    }
}