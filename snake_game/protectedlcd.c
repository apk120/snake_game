/** \file protectedlcd.c
*
* @brief Reentrant LCD driver.
*/

#include <assert.h>
#include <stdint.h>

#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/gates/GateMutexPri.h>
#include <ti/grlib/grlib.h>
#include "LcdDriver/Crystalfontz128x128_ST7735.h"
#include "protectedlcd.h"

// Private mutex
static GateMutexPri_Handle  g_lcd_mutex;

// Graphic library context
static Graphics_Context g_context;

static snakePoint snake[MAX_SNAKE_LENGTH];
static int8_t moves[NUM_MOVES] = {-1, -1, -1, -1, 1, 1, 1, 1};//{-2, -2, -1, -1, 2, 2, 1, 1};
/*!
* @brief Initialize the reentrant LCD driver.
*/
void protected_lcd_init(void)
{
    GateMutexPri_Params mutex_params;

    // Create the mutex that protects the hardware from race conditions.
    /* Obtain instance handle */
    GateMutexPri_Params_init(&mutex_params);
    g_lcd_mutex = GateMutexPri_create(&mutex_params, NULL);

    /* Initializes display */
    Crystalfontz128x128_Init();

    /* Set default screen orientation */
    Crystalfontz128x128_SetOrientation(LCD_ORIENTATION_UP);

    /* Initializes graphics context */
    Graphics_initContext(&g_context, &g_sCrystalfontz128x128, &g_sCrystalfontz128x128_funcs);
    Graphics_setForegroundColor(&g_context, GRAPHICS_COLOR_WHITE_SMOKE);
    Graphics_setBackgroundColor(&g_context, GRAPHICS_COLOR_BLACK);
    GrContextFontSet(&g_context, &g_sFontFixed6x8);
    Graphics_clearDisplay(&g_context);

}

/*!
* @brief Display data on the LCD, safely.
*
* @param position line position
* @param data pointer to the data to display
*/
void protected_lcd_display(uint8_t position, char const *data)
{
    int key;

    // Try to acquire the mutex.
    key = GateMutexPri_enter(g_lcd_mutex);

    // Safely inside the critical section.

    // Call the non-reentrant driver.
    Graphics_drawString(&g_context,
                            (int8_t *)data,
                            AUTO_STRING_LENGTH,
                            10,
                            position*10 + 10,
                            OPAQUE_TEXT);

    // Release the mutex.
    GateMutexPri_leave(g_lcd_mutex, key);
}

void init_snake()
{
    uint8_t s = 0, i;

    dirX = 1;
    dirY = 0;
    snakeLength = INIT_LENGTH;
    snake[0].x = 60; snake[0].y = 60;

    for (i = 1; i < INIT_LENGTH; i++)
    {
        snake[i].x = snake[i - 1].x + moves[s];
        snake[i].y = (snake[i - 1].y + 2) % 128;
        s = (s + 1) % NUM_MOVES;
    }
}

void draw_snake(uint8_t state, uint8_t start_x, uint8_t start_y)
{

    uint8_t i = 0, die = 0;

    for (i = snakeLength + SNAKE_INCREASE - 1; i > 0; i--)
    {
        if (i < snakeLength)
        {
            Graphics_fillCircle(&g_context, snake[i].x, snake[i].y, 1);
            if (abs(snake[0].x - snake[i].x) < 2 && abs(snake[0].y - snake[i].y) < 2 && i > 2) die = 1;

        }
        snake[i].x = snake[i - 1].x;
        snake[i].y = snake[i - 1].y;
    }

    // Check if head is in radius of Fruit
    if (abs(snake[0].x - xFruit) <= 3 && abs(snake[0].y - yFruit) <= 3)
    {
        isFruit = 0;
        if (snakeLength < MAX_SNAKE_LENGTH - SNAKE_INCREASE) snakeLength += SNAKE_INCREASE;
    }

    // Head
    Graphics_fillCircle(&g_context, snake[0].x, snake[0].y, 2);

    // dirX is 0 means snake moves in up (dirY = 1) or down (dirY = -1) direction
    if (dirX == 0)
    {
        snake[0].x = (snake[0].x + moves[state]) % 128;
        snake[0].y = (snake[0].y + 2 * dirY) % 128;
    }
    else // either dirX or dirY is non zero at one time
    {
        snake[0].x = (snake[0].x + 2 * dirX) % 128;
        snake[0].y = (snake[0].y + moves[state]) % 128;
    }
    if (1 == die)
    {
       init_snake();
       die = 0;
    }
}

void draw_fruit()
{
    if (1 == isFruit)
    {
        Graphics_fillCircle(&g_context, xFruit, yFruit, 3);
    }
}

void protected_lcd_draw_pixel(uint8_t state, uint8_t x, uint8_t y)
{
    int key;

    // Try to acquire the mutex.
    key = GateMutexPri_enter(g_lcd_mutex);

    // Safely inside the critical section.

    // Call the non-reentrant driver.
//    Graphics_fillRectangle(&g_context, rect);
     draw_snake(state, x, y);
     draw_fruit();
    // Release the mutex.
    GateMutexPri_leave(g_lcd_mutex, key);
}



/*!
* @brief Clear the LCD, safely.
*/
void protected_lcd_clear(void)
{
    int key;

    // Try to acquire the mutex.
    key = GateMutexPri_enter(g_lcd_mutex);

    // Safely inside the critical section.

    // Call the non-reentrant driver.
    Graphics_clearDisplay(&g_context);

    // Release the mutex.
    GateMutexPri_leave(g_lcd_mutex, key);
}
