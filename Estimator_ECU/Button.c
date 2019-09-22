/*******************************************************************************************************************
 * Button.c
 *
 * Created: 9/2019
 * Author:  Ahmed Yehia
 *******************************************************************************************************************/
/*******************************************************************************************************************
 * include(s)
 *******************************************************************************************************************/
#include "Button.h"
/*******************************************************************************************************************
 * shared global variable(s)
 *******************************************************************************************************************/

/*******************************************************************************************************************
 * static global variable(s)
 *******************************************************************************************************************/

/*******************************************************************************************************************
 * ISR & static function prototype(s)
 *******************************************************************************************************************/

/*******************************************************************************************************************
 * shared function definition(s)
 *******************************************************************************************************************/
void vButton_Init(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF));

    HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
    HWREG(GPIO_PORTF_BASE + GPIO_O_CR) |= 0x01;
    HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = 0;

    GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_4);
    GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_4, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

    GPIOIntTypeSet(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_4, GPIO_FALLING_EDGE);
    GPIOIntRegister(GPIO_PORTF_BASE, vButton_Handler);
    GPIOIntEnable(GPIO_PORTF_BASE, GPIO_INT_PIN_0 | GPIO_INT_PIN_4);
}
/*******************************************************************************************************************
 * ISR & static function definition(s)
 *******************************************************************************************************************/
void vButton_Handler(void)
{
    GPIOIntClear(GPIO_PORTF_BASE, GPIO_INT_PIN_0 | GPIO_INT_PIN_4);

    if(GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_0) != GPIO_PIN_0)
    {
        if(Speed != 255)
        {
            Speed++;
            while(UARTBusy(UART3_BASE));
            UARTCharPut(UART3_BASE, Time);
            while(UARTBusy(UART3_BASE));
            UARTCharPut(UART3_BASE, Speed);
        }
    }
    if(GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4) != GPIO_PIN_4)
    {
        if(Speed != 0)
        {
            Speed--;
            while(UARTBusy(UART3_BASE));
            UARTCharPut(UART3_BASE, Time);
            while(UARTBusy(UART3_BASE));
            UARTCharPut(UART3_BASE, Speed);
        }
    }
}
