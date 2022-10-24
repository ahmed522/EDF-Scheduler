/*
 * FreeRTOS Kernel V10.2.0
 * Copyright (C) 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */

/* 
	NOTE : Tasks run in system mode and the scheduler runs in Supervisor mode.
	The processor MUST be in supervisor mode when vTaskStartScheduler is 
	called.  The demo applications included in the FreeRTOS.org download switch
	to supervisor mode prior to main being called.  If you are not using one of
	these demo application projects then ensure Supervisor mode is used.
*/


/*
 * Creates all the demo application tasks, then starts the scheduler.  The WEB
 * documentation provides more details of the demo application tasks.
 * 
 * Main.c also creates a task called "Check".  This only executes every three 
 * seconds but has the highest priority so is guaranteed to get processor time.  
 * Its main function is to check that all the other tasks are still operational.
 * Each task (other than the "flash" tasks) maintains a unique count that is 
 * incremented each time the task successfully completes its function.  Should 
 * any error occur within such a task the count is permanently halted.  The 
 * check task inspects the count of each task to ensure it has changed since
 * the last time the check task executed.  If all the count variables have 
 * changed all the tasks are still executing error free, and the check task
 * toggles the onboard LED.  Should any task contain an error at any time 
 * the LED toggle rate will change from 3 seconds to 500ms.
 *
 */

/* Standard includes. */
#include <stdlib.h>
#include <stdio.h>

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "lpc21xx.h"

/* Peripheral includes. */
#include "serial.h"
#include "GPIO.h"


/*-----------------------------------------------------------*/

/* Constants to setup I/O and processor. */
#define mainBUS_CLK_FULL	( ( unsigned char ) 0x01 )

/* Constants for the ComTest demo application tasks. */
#define mainCOM_TEST_BAUD_RATE	( ( unsigned long ) 115200 )


/*
 * Configure the processor for use with the Keil demo board.  This is very
 * minimal as most of the setup is managed by the settings in the project
 * file.
 */
/*---------------------------------------------------------------------------------*
 *                               FUNCTION PROTOTYPES                               *
 *---------------------------------------------------------------------------------*/
static void prvSetupHardware( void );
void Tsk_Button_1_Monitor 		(void * period );
void Tsk_Button_2_Monitor 		(void * period );
void Tsk_Periodic_Transmitter (void * period );
void Tsk_Uart_Receiver 				(void * period );
void Tsk_Load_1_Simulation 		(void * period );
void Tsk_Load_2_Simulation 		(void * period );

void vApplicationTickHook( void );
void vApplicationIdleHook( void );
/*-----------------------------------------------------------*/
TickType_t Tsk_Button_1_Monitor_Period = 10;
TickType_t Tsk_Button_2_Monitor_Period = 50;
TickType_t Tsk_Periodic_Transmitter_Period = 100;
TickType_t Tsk_Uart_Receiver_Period = 20;
TickType_t Tsk_Load_1_Simulation_Period = 10;
TickType_t Tsk_Load_2_Simulation_Period = 100;
char* g_eventHolder;
unsigned short g_eventHolder_Length;
char runtime_Stat_Buffer[190];
/*
 * Application entry point:
 * Starts all the other tasks, then starts the scheduler. 
 */
int main( void )
{
	/* Setup the hardware for use with the Keil demo board. */
	prvSetupHardware();

	/* Create Tasks here */
    xTaskCreate(Tsk_Button_1_Monitor,"Button_1_Monitor",configMINIMAL_STACK_SIZE,&Tsk_Button_1_Monitor_Period, NULL,(void *) NULL);
    xTaskCreate(Tsk_Button_2_Monitor,"Button_2_Monitor",configMINIMAL_STACK_SIZE,&Tsk_Button_2_Monitor_Period, NULL,(void *) NULL);
    xTaskCreate(Tsk_Periodic_Transmitter,"Periodic_Transmitter",configMINIMAL_STACK_SIZE,&Tsk_Periodic_Transmitter_Period, NULL,(void *) NULL);
    xTaskCreate(Tsk_Uart_Receiver,"Uart_Receiver",configMINIMAL_STACK_SIZE,&Tsk_Uart_Receiver_Period, NULL,(void *) NULL);
    xTaskCreate(Tsk_Load_1_Simulation,"Load_1_Simulation",configMINIMAL_STACK_SIZE,&Tsk_Load_1_Simulation_Period, NULL,(void *) NULL);
    xTaskCreate(Tsk_Load_2_Simulation,"Load_2_Simulation",configMINIMAL_STACK_SIZE,&Tsk_Load_2_Simulation_Period, NULL,(void *) NULL);

	/* Now all the tasks have been started - start the scheduler.

	NOTE : Tasks run in system mode and the scheduler runs in Supervisor mode.
	The processor MUST be in supervisor mode when vTaskStartScheduler is 
	called.  The demo applications included in the FreeRTOS.org download switch
	to supervisor mode prior to main being called.  If you are not using one of
	these demo application projects then ensure Supervisor mode is used here. */
	vTaskStartScheduler();

	/* Should never reach here!  If you do then there was not enough heap
	available for the idle task to be created. */
	for( ;; );
}
/*-----------------------------------------------------------*/

static void prvSetupHardware( void )
{
	/* Perform the hardware setup required.  This is minimal as most of the
	setup is managed by the settings in the project file. */

	/* Configure UART */
	xSerialPortInitMinimal(mainCOM_TEST_BAUD_RATE);

	/* Configure GPIO */
	GPIO_init();
	
	/* Config trace timer 1 and read T1TC to get current tick */

	/* Setup the peripheral bus to be the same as the PLL output. */
	VPBDIV = mainBUS_CLK_FULL;
}
/*-----------------------------------------------------------*/

void vApplicationTickHook( void )
{
		GPIO_write(PORT_0,PIN5,PIN_IS_HIGH);
		GPIO_write(PORT_0,PIN5,PIN_IS_LOW);
}
void vApplicationIdleHook( void )
{
		GPIO_write(PORT_0,PIN7,PIN_IS_LOW);
    GPIO_write(PORT_0,PIN7,PIN_IS_HIGH);
}
void Tsk_Button_1_Monitor(void * period )
{
		uint8_t risingFlag = 1;
	while(1)
	{		 
			if ( ( GPIO_read(PORT_0,PIN0) == PIN_IS_HIGH ) && risingFlag )  /* rising edge */
			{
				g_eventHolder = "button 1 rising edge";
				risingFlag = 0;
			}
			else if ( ( GPIO_read(PORT_0,PIN0) == PIN_IS_HIGH ) && !risingFlag ) /* logic High */
			{
				g_eventHolder = "no event";
			}
			else if ( ( GPIO_read(PORT_0,PIN0) == PIN_IS_LOW ) && !risingFlag )  /* falling edge */
			{
				g_eventHolder = "button 1 falling edge";
				risingFlag = 1;
			}
			else  /* logic Low */
			{
				g_eventHolder = "no event";
			}
			GPIO_write(PORT_0,PIN8,PIN_IS_LOW);
			vTaskDelay(*((TickType_t *) period));
			GPIO_write(PORT_0,PIN8,PIN_IS_HIGH);

	}
}
void Tsk_Button_2_Monitor(void * period )
{
	uint8_t risingFlag = 1;
	while(1)
	{		 		
			if ( ( GPIO_read(PORT_0,PIN1) == PIN_IS_HIGH ) && risingFlag )  /* rising edge */
			{
				g_eventHolder = "button 2 rising edge";
				risingFlag = 0;
			}
			else if ( ( GPIO_read(PORT_0,PIN1) == PIN_IS_HIGH ) && !risingFlag ) /* logic High */
			{
				g_eventHolder = "no event";
			}
			else if ( ( GPIO_read(PORT_0,PIN1) == PIN_IS_LOW ) && !risingFlag )  /* falling edge */
			{
				g_eventHolder = "button 2 falling edge";
				risingFlag = 1;
			}
			else  /* logic Low */
			{
				g_eventHolder = "no event";
			}
			GPIO_write(PORT_0,PIN9,PIN_IS_LOW);
	  	vTaskDelay(*((TickType_t *) period));
			GPIO_write(PORT_0,PIN9,PIN_IS_HIGH);
	}
}
void Tsk_Periodic_Transmitter (void * period )
{
	unsigned short i;
	while(1)
	{
		i = 0;
		if(g_eventHolder != NULL)
		{		
			while(g_eventHolder[i] != '\0')
			{
				++i;
			}
			g_eventHolder_Length = i;
	  }
		GPIO_write(PORT_0,PIN10,PIN_IS_LOW);
		vTaskDelay(*((TickType_t *) period));
		GPIO_write(PORT_0,PIN10,PIN_IS_HIGH);

	}
}
void Tsk_Uart_Receiver 	(void * period )
{
	while(1)
	{
			if(g_eventHolder_Length != NULL)
			{
					vSerialPutString(g_eventHolder,g_eventHolder_Length);
				  xSerialPutChar('\n');
			}
			GPIO_write(PORT_0,PIN11,PIN_IS_LOW);
			vTaskDelay(*((TickType_t *) period));
			GPIO_write(PORT_0,PIN11,PIN_IS_HIGH);
	}
}
void Tsk_Load_1_Simulation 		(void * period )
{
	while(1)
	{
		int x = 0;
		for(x=0;x<33100;x++)    /* 5 ms execution time*/ 
		{
			x=x;
		}
    GPIO_write(PORT_0,PIN12,PIN_IS_LOW);
		vTaskDelay(*((TickType_t *) period));
		GPIO_write(PORT_0,PIN12,PIN_IS_HIGH);
	}
}
void Tsk_Load_2_Simulation 		(void * period )
{
while(1)
	{
		int x = 0;
		for(x=0;x<70440;x++)    /* 12 ms execution time*/ 
		{
			x=x;
		}
		vTaskGetRunTimeStats(runtime_Stat_Buffer);
		vSerialPutString(runtime_Stat_Buffer,190);
    xSerialPutChar('\n');
    GPIO_write(PORT_0,PIN13,PIN_IS_LOW);
		vTaskDelay(*((TickType_t *) period));
		GPIO_write(PORT_0,PIN13,PIN_IS_HIGH);
	}
}


