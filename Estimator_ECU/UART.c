/*******************************************************************************************************************
 * UART.c
 *
 * Created: 9/2019
 * Author:  Ahmed Yehia
 *******************************************************************************************************************/
/*******************************************************************************************************************
 * include(s)
 *******************************************************************************************************************/
#include "UART.h"
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
void vUART_Init(void)
{
    /*
     *
     */
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOA));
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    UARTStdioConfig(0, 115200, SysCtlClockGet());
    /*
     *
     */
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOC));
    GPIOPinTypeUART(GPIO_PORTC_BASE, GPIO_PIN_6 | GPIO_PIN_7);
    GPIOPinConfigure(GPIO_PC6_U3RX);
    GPIOPinConfigure(GPIO_PC7_U3TX);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART3);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_UART3));
    UARTConfigSetExpClk(UART3_BASE, SysCtlClockGet(), 9600,
                        (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));

    UARTFIFOLevelSet(UART3_BASE, UART_FIFO_TX2_8, UART_FIFO_RX1_8);
    UARTIntRegister(UART3_BASE, vUART_UART0Handler);
    UARTIntEnable(UART3_BASE, UART_INT_RX | UART_INT_RT);
}
/*******************************************************************************************************************
 * ISR & static function definition(s)
 *******************************************************************************************************************/
void vUART_UART0Handler(void)
{
    UARTIntClear(UART3_BASE, UART_INT_RX | UART_INT_RT);
    if(UARTCharsAvail(UART3_BASE))
    {
        if(UARTCharGet(UART3_BASE) == '#')
        {
            UARTprintf("send time\n");
            UARTCharPut(UART3_BASE, Time);
            UARTCharPut(UART3_BASE, Speed);
        }
    }
}
