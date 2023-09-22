/*
 * Copyright (c) 2016-2019, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,

 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  ======== main_tirtos.c ========
 */

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <time.h>

// RTOS header files
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <xdc/runtime/Error.h>
#include <ti/drivers/ADC.h>
#include <ti/sysbios/knl/Mailbox.h>
#include <ti/grlib/grlib.h>

#include "main.h"
#include "common.h"
#include "protectedlcd.h"
#include "bsp.h"
/* Driver configuration */
#include <ti/boards/MSP_EXP432P401R/Board.h>

static void startup_task(unsigned int, unsigned int);
static void adc_task(unsigned int, unsigned int);
static void display_task(unsigned int, unsigned int);
static void fruit_task(unsigned int, unsigned int);

Mailbox_Handle g_mail_adc, g_mail_adc_x;
/*!
* @brief Main entry point to the program.
*/
int main(void)
{

    /* Call driver init functions */
    Board_init();

    // Initialize the reentrant LCD drivers.
    protected_lcd_init();

    //! @todo initialize startup task parameters, stack size, priority
    Task_Params tp_start;
    Error_Block eb_start;
    Task_Handle p_task_handle_start;

    Task_Params_init(&tp_start);
    tp_start.priority = 2;
    tp_start.stackSize = 1024;
    //! @todo initialize error block
    Error_init(&eb_start);
    //! @todo create the startup task
    p_task_handle_start = Task_create(startup_task, &tp_start, &eb_start);
    //! @todo check for problems using SYS_ASSERT()
    SYS_ASSERT((p_task_handle_start != NULL) && !Error_check(&eb_start));
    //! @todo start the kernel
    BIOS_start();
    // should never get here!
    SYS_FAIL();
    assert(0);

    return (0);
}

/**
 *  SYS/BIOS task. Startup task.
 *
 *  @param[in] task_arg0 unused, argument from SYS/BIOS task create
 *  @param[in] task_arg1 unused, argument from SYS/BIOS task create
 */
static void startup_task(unsigned int t1, unsigned int t2)
{
    // Initialize task parameters
    Task_Params tp_disp, tp_adc, tp_fruit;
    Error_Block eb;
    Task_Handle p_task_handle_disp, p_task_handle_adc, p_task_handle_fruit;
    Mailbox_Params mail_adc;

    Task_Params_init(&tp_disp);
    Task_Params_init(&tp_adc);
    Task_Params_init(&tp_fruit);
    Mailbox_Params_init(&mail_adc);

    tp_disp.priority = 4;
    tp_disp.stackSize = 2048;

    tp_adc.priority = 3;
    tp_adc.stackSize = 512;

    tp_fruit.priority = 3;
    tp_fruit.stackSize = 512;

    // Initialize error blocks
    Error_init(&eb);

    // Create mailbox
    g_mail_adc = Mailbox_create(4, 10, &mail_adc, &eb);
    SYS_ASSERT((g_mail_adc != NULL) && !Error_check(&eb));
    g_mail_adc_x = Mailbox_create(4, 10, &mail_adc, &eb);
    SYS_ASSERT((g_mail_adc_x != NULL) && !Error_check(&eb));

    // Create the tasks
    p_task_handle_disp = Task_create(display_task, &tp_disp, &eb);
    SYS_ASSERT((p_task_handle_disp != NULL) && !Error_check(&eb));
    p_task_handle_adc = Task_create(adc_task, &tp_adc, &eb);
    SYS_ASSERT((p_task_handle_adc != NULL) && !Error_check(&eb));
    p_task_handle_fruit = Task_create(fruit_task, &tp_fruit, &eb);
    SYS_ASSERT((p_task_handle_fruit != NULL) && !Error_check(&eb));

    for (;;);
}

static void adc_task(unsigned int t1, unsigned int t2)
{
    ADC_Handle adc, adcx;
    ADC_Params adcParams;
    int_fast16_t res;
    uint16_t adcVal;
//    uint32_t adcValuV;
//    Bool ok = 0;

    ADC_init();
    ADC_Params_init(&adcParams);
    adc = ADC_open(2, &adcParams);
    adcx = ADC_open(1, &adcParams);
    for (;;)
    {
        res = ADC_convert(adc, &adcVal);
        if (res == ADC_STATUS_SUCCESS);
        {
//            adcValuV = ADC_convertToMicroVolts(adc, adcVal);
            Mailbox_post(g_mail_adc, &adcVal, BIOS_NO_WAIT);
        }
        res = ADC_convert(adcx, &adcVal);
        if (res == ADC_STATUS_SUCCESS);
        {
//            adcValuV = ADC_convertToMicroVolts(adc, adcVal);
            Mailbox_post(g_mail_adc_x, &adcVal, BIOS_NO_WAIT);
        }
        Task_sleep(100);
    }
}

static void display_task(unsigned int t1, unsigned int t2)
{
    uint32_t adcVal;
    Bool ok;
    uint16_t adc7bits;
    uint8_t x = 60, y = 60, state = 0;
    volatile int8_t turn = 0,turnx = 0; // -1 -> left, 0 -> No turn, 1 -> right

    init_snake();
    for (;;)
    {
        ok = Mailbox_pend(g_mail_adc, &adcVal, BIOS_NO_WAIT);

        if (ok)
        {
            adc7bits = ((adcVal >> 5) & (0x3FF));

            if (adc7bits > 80 && turn == 0)
            {
                if (dirX == 1) turn = 1;
                else if (dirX == -1) turn = -1;
            }
            else if (adc7bits < 30 && turn == 0)
            {
                if (dirX == 1) turn = -1;
                else if (dirX == -1) turn = 1;
            }
            else
            {
                turn = 0;
            }
        }

        ok = Mailbox_pend(g_mail_adc_x, &adcVal, BIOS_NO_WAIT);

        if (ok)
        {
            adc7bits = ((adcVal >> 5) & (0x3FF));

            if (adc7bits > 80 && turnx == 0)
            {
                if (dirY == 1) turnx = 1;
                else if (dirY == -1) turnx = -1;
            }
            else if (adc7bits < 30 && turnx == 0)
            {
                if (dirY == 1) turnx = -1;
                else if (dirY == -1) turnx = 1;
            }
            else
            {
                turnx = 0;
            }
        }

        if (turn == 1 || turnx == 1)
        {
            if (dirX == 0)
            {
                dirX = dirY; // if up(1) go right(1), else go left(-1)
                dirY = 0;
            }
            else
            {
                dirY = -1 * dirX; // if right(1) go down(-1), else go up(1)
                dirX = 0;
            }
        }
        else if (turn == -1 || turnx == -1)
        {
            if (dirX == 0)
            {
                dirX = -1 * dirY; // if up(1) go left(-1), else go right(1)
                dirY = 0;
            }
            else
            {
                dirY = 1 * dirX; // if right(1) go up(-1), else go down(-1)
                dirX = 0;
            }
        }


        protected_lcd_clear();
        protected_lcd_draw_pixel(state, x, y);
        state = (state + 1) % NUM_MOVES;
        Task_sleep(20);
    }
}

static void fruit_task(unsigned int t1, unsigned int t2)
{
    int r;
    isFruit = 0;
    srand(time(NULL));

    for(;;)
    {
        if (0 == isFruit)
        {
            r = rand();
            xFruit = r % 123 + 3;
            r = rand();
            yFruit = r % 123 + 3;
            isFruit = 1;
        }

        Task_sleep(30);
    }
}
