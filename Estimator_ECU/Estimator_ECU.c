#include "Button.h"
#include "UART.h"
#include "Timer.h"

#define LENGTH 3
uint8_t Time = 0;
uint8_t Speed = 0;

int main(void)
{
    uint8_t i;
    uint8_t string[LENGTH];
    /*
     *
     */
    vUART_Init();
    vButton_Init();
    vTimer_Init();
    /*
     *
     */
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF));

    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);
    /*
     *
     */
    UARTprintf("\n");
    UARTprintf("**********************************\n");
    UARTprintf(" Serial Communication Challenge\n");
    UARTprintf("**********************************\n");
    UARTprintf("enter current time : ");
    for(i = 0; i < LENGTH; i++)
    {
        string[i] = UARTgetc();
        UARTprintf("%c", string[i]);
        Time += (string[i] - '0') * pow(10, LENGTH-1-i);
    }
    UARTprintf("\n");
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0xFF);
    /*
     *
     */
    TimerEnable(TIMER0_BASE, TIMER_A);
    for(;;)
    {
    }
}
