/*
 * Copyright (c) 2013-2019 Huawei Technologies Co., Ltd. All rights reserved.
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 *    conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 *    of conditions and the following disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "debug.h"
#include "los_tick.h"
#include "los_task.h"
#include "los_config.h"
#include "los_interrupt.h"
#include "los_debug.h"
#include "los_compiler.h"

/* Global define */


/* Global Variable */
__attribute__((aligned (4))) UINT8 g_memStart[LOSCFG_SYS_HEAP_SIZE];
UINT32 g_VlaueSp=0;


/*******************************************************************************
* Function Name  : taskSampleEntry2
* Description    : taskSampleEntry2 program.
* Input          : None
* Return         : None
*******************************************************************************/
VOID taskSampleEntry2(VOID)
{
    while(1) {
      LOS_TaskDelay(1000);
      LOS_TaskDelay(1000);
      LOS_TaskDelay(1000);
      LOS_TaskDelay(1000);
      LOS_TaskDelay(1000);
      printf("taskSampleEntry2 running,task2 SP:%08x\n",__get_SP());
    }
}

/*******************************************************************************
* Function Name  : taskSampleEntry1
* Description    : taskSampleEntry1 program.
* Input          : None
* Return         : None
*******************************************************************************/
VOID taskSampleEntry1(VOID)
{
    while(1) {
      LOS_TaskDelay(1000);
      printf("taskSampleEntry1 running,task1 SP:%08x\n",__get_SP());
    }

}

/*******************************************************************************
* Function Name  : EXTI0_INT_INIT
* Description    : Initializes EXTI0 collection.
* Input          : None
* Return         : None
*******************************************************************************/
void EXTI0_INT_INIT(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  EXTI_InitTypeDef EXTI_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO|RCC_APB2Periph_GPIOA,ENABLE);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

   /* GPIOA ----> EXTI_Line0 */
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource0);
  EXTI_InitStructure.EXTI_Line=EXTI_Line0;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);

  NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 4;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

/*******************************************************************************
* Function Name  : taskSample
* Description    : taskSample program.
* Input          : None
* Return         : None
*******************************************************************************/
UINT32 taskSample(VOID)
{
    UINT32  uwRet;
    UINT32 taskID1,taskID2;
    TSK_INIT_PARAM_S stTask={0};
    stTask.pfnTaskEntry = (TSK_ENTRY_FUNC)taskSampleEntry1;
    stTask.uwStackSize  = 0X500;
    stTask.pcName       = "taskSampleEntry1";
    stTask.usTaskPrio   = 6;/* ???????? */
    uwRet = LOS_TaskCreate(&taskID1, &stTask);
    if (uwRet != LOS_OK) {
        printf("create task1 failed\n");
    }

    stTask.pfnTaskEntry = (TSK_ENTRY_FUNC)taskSampleEntry2;
    stTask.uwStackSize  = 0X500;
    stTask.pcName       = "taskSampleEntry2";
    stTask.usTaskPrio   = 7;/* ???????? */
    uwRet = LOS_TaskCreate(&taskID2, &stTask);
    if (uwRet != LOS_OK) {
        printf("create task2 failed\n");
    }

    EXTI0_INT_INIT();
    return LOS_OK;
}


/*******************************************************************************
* Function Name  : main
* Description    : Main program.
* Input          : None
* Return         : None
*******************************************************************************/
LITE_OS_SEC_TEXT_INIT int main(void)
{
    unsigned int ret;
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
    USART_Printf_Init(115200);
    printf("SystemClk:%d\r\n",SystemCoreClock);
    ret = LOS_KernelInit();
    taskSample();
    if (ret == LOS_OK)
    {
        LOS_Start();
    }

    while (1) {
        __asm volatile("nop");
    }

}

/*******************************************************************************
* Function Name  : EXTI0_IRQHandler
* Description    : This function handles EXTI0 Handler.
* Input          : None
* Return         : None
*******************************************************************************/
void EXTI0_IRQHandler(void) __attribute__((interrupt(/*"WCH-Interrupt-fast"*/)));
void EXTI0_IRQHandler(void)
{
  /* ??????????????????????main????????????????????????????????????????????????????????????????????????????????
   * ????????????????????????????????????????????????????????????????16??caller????????????????????????????????
   * ??caller????????????????????????????????????????????????????????????????????????????????????????los_exc.S
   * ????ipq_entry????
   *  */
  GET_INT_SP();
  HalIntEnter();
  if(EXTI_GetITStatus(EXTI_Line0)!=RESET)
  {
    g_VlaueSp= __get_SP();
    printf("Run at EXTI:");
    printf("interruption sp:%08x\r\n",g_VlaueSp);
    HalDisplayTaskInfo();
    EXTI_ClearITPendingBit(EXTI_Line0);     /* Clear Flag */
  }
  HalIntExit();
  FREE_INT_SP();
}
