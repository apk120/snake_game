/** \file protectedlcd.h
*
* @brief Reentrant LCD driver.
*/

#ifndef _PROTECTEDLCD_H
#define _PROTECTEDLCD_H

#define NUM_MOVES (8)
#define INIT_LENGTH (20)
#define MAX_SNAKE_LENGTH (150)
#define SNAKE_INCREASE  (5)

volatile int8_t dirX;
volatile int8_t dirY;
volatile uint8_t xFruit, yFruit, isFruit;
volatile uint16_t snakeLength;

typedef struct
{
  uint8_t x;
  uint8_t y;
} snakePoint;

void init_snake(void);
void protected_lcd_init(void);
void protected_lcd_display(uint8_t, char const *);
void protected_lcd_draw_pixel(uint8_t, uint8_t, uint8_t);
void protected_lcd_clear(void);

#endif  /* _PROTECTEDLCD_H */
