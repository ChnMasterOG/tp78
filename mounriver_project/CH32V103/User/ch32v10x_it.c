/********************************** (C) COPYRIGHT *******************************
* File Name          : ch32v10x_it.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2020/04/30
* Description        : Main Interrupt Service Routines.
*******************************************************************************/
#include "ch32v10x_it.h"
#include "los_interrupt.h"
void NMI_Handler(void) __attribute__((interrupt()));
void HardFault_Handler(void) __attribute__((interrupt()));

/*******************************************************************************
* Function Name  : NMI_Handler
* Description    : This function handles NMI exception.
* Input          : None
* Return         : None
*******************************************************************************/
void NMI_Handler(void)
{
    GET_INT_SP();
    HalIntEnter();
    while(1)
    {

    }
    HalIntExit();
    FREE_INT_SP();
}

/*******************************************************************************
* Function Name  : HardFault_Handler
* Description    : This function handles Hard Fault exception.
* Input          : None
* Return         : None
*******************************************************************************/

void HardFault_Handler(void)
{
    GET_INT_SP();
    HalIntEnter();
    printf("mcause:%08x\r\n",__get_MCAUSE());
    printf("mtval:%08x\r\n",__get_MTVAL());
    printf("mepc:%08x\r\n",__get_MEPC());
    while (1)
    {
    }
    HalIntExit();
    FREE_INT_SP();
}


