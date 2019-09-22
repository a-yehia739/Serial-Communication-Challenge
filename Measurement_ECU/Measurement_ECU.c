//*****************************************************************************
//
// uart_echo.c - Example for reading data from and writing data to the UART in
//               an interrupt driven fashion.
//
// Copyright (c) 2012-2017 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
// 
// Texas Instruments (TI) is supplying this software for use solely and
// exclusively on TI's microcontroller products. The software is owned by
// TI and/or its suppliers, and is protected under applicable copyright
// laws. You may not combine this software with "viral" open-source
// software in order to form a larger program.
// 
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
// CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES, FOR ANY REASON WHATSOEVER.
// 
// This is part of revision 2.1.4.178 of the EK-TM4C123GXL Firmware Package.
//
//*****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include "uartstdio.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "driverlib/debug.h"
#include "driverlib/fpu.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "Button.h"

//*****************************************************************************
//
//! \addtogroup example_list
//! <h1>UART Echo (uart_echo)</h1>
//!
//! This example application utilizes the UART to echo text.  The first UART
//! (connected to the USB debug virtual serial port on the evaluation board)
//! will be configured in 115,200 baud, 8-n-1 mode.  All characters received on
//! the UART are transmitted back to the UART.
//
//*****************************************************************************
#define UART0_CH    0
#define UART1_CH    1

#define RED         1
#define BLUE        2
#define GREEN       3

#define MAX_SIZE    5

void UARTSend(const uint8_t *pui8Buffer, uint32_t ui32Count, uint8_t channel);



//*****************************************************************************
//
// The error routine that is called if the driver library encounters an error.
//
//*****************************************************************************
#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line)
{
}
#endif


uint8_t Last_Time;
uint8_t Last_Speed;
uint8_t Speed;
uint8_t Time;
uint16_t Distance;

//*****************************************************************************
//
// The UART interrupt handler.
//
//*****************************************************************************

/**************************************************************************************/

void UART1_IntHandler(void)
{
    UARTIntClear(UART1_BASE, UART_INT_RX | UART_INT_RT);
    uint32_t* ret_uart = (uint32_t*)0;

    while(UARTCharsAvail(UART1_BASE))
    {
        Last_Time = Time;
        Last_Speed = Speed;
        *ret_uart = UARTCharGet(UART1_BASE);
        Time = *ret_uart;
        UARTprintf("%d \t",*ret_uart);
        *ret_uart = UARTCharGet(UART1_BASE);
        Speed = *ret_uart;
        UARTprintf("%d\t",*ret_uart);
        Distance += (Time - Last_Time)*Last_Speed;
        UARTprintf("%d\n",Distance);
    }
}


//*****************************************************************************
//
// Init UART
//
//*****************************************************************************

void UART_Init(uint8_t channel)
{

    switch(channel)
    {
                    case UART0_CH:
                        ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
                        ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
                        ROM_IntEnable(INT_UART0);
                        ROM_UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT);
                        GPIOPinConfigure(GPIO_PA0_U0RX);
                        GPIOPinConfigure(GPIO_PA1_U0TX);
                        ROM_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
                        ROM_UARTConfigSetExpClk(UART0_BASE, ROM_SysCtlClockGet(), 115200, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
                        break;

                    case UART1_CH:
                        ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);
                        ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
                        GPIOPinConfigure(GPIO_PB0_U1RX);
                        GPIOPinConfigure(GPIO_PB1_U1TX);
                        ROM_GPIOPinTypeUART(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);
                        ROM_UARTConfigSetExpClk(UART1_BASE, ROM_SysCtlClockGet(), 9600, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
                        ROM_IntEnable(INT_UART1);
                        ROM_UARTIntEnable(UART1_BASE, UART_INT_RX | UART_INT_RT);

                    default :
                        break;
    }
    //
    // Configure the UART for 115,200, 8-N-1 operation.
    //

    UARTFIFOLevelSet(UART0_BASE, UART_FIFO_TX2_8, UART_FIFO_RX2_8);
}


//*****************************************************************************
//
// Send a string to the UART.
//
//*****************************************************************************

void UARTSend(const uint8_t *pui8Buffer, uint32_t ui32Count, uint8_t channel)
{
    //
    // Loop while there are more characters to send.
    //
    switch(channel)
    {
        case UART0_CH:
            while(ui32Count--)
            {
                //
                // Write the next character to the UART.
                //
                ROM_UARTCharPutNonBlocking(UART0_BASE, *pui8Buffer++);
            }
            break;
        case UART1_CH:
            //
            // Write the next character to the UART.
            //
            ROM_UARTCharPutNonBlocking(UART1_BASE, *pui8Buffer++);
            break;
        default:
            break;
    }
}

//*****************************************************************************
//
// System Init
//
//*****************************************************************************
void System_Init(void)
{
    //
    // Enable lazy stacking for interrupt handlers.  This allows floating-point
    // instructions to be used within interrupt handlers, but at the expense of
    // extra stack usage.
    //
    ROM_FPUEnable();
    ROM_FPULazyStackingEnable();

    //
    // Set the clocking to run directly from the crystal.
    //
    ROM_SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

}


//*****************************************************************************
//
// This example demonstrates how to send a string of data to the UART.
//
//*****************************************************************************
void UART_Start_MS(void)
{
    UARTSend("#", 2, UART1_CH);
}

void UART_End_MS(void)
{
    UARTprintf("\nFinal Distance = %d\n", Distance);
}



int main(void)
{
    //uint64_t i;
    // System Configurations
    System_Init();

    // UART Configurations
    UART_Init(UART0_CH);
    UART_Init(UART1_CH);
    UARTStdioConfig(0, 115200, SysCtlClockGet());
    vButton_Init();
    Button_SetCallBack(UART_Start_MS, UART_End_MS);

    //
    // Enable processor interrupts.
    //
    ROM_IntMasterEnable();


    while(1)
    {

    }
}

