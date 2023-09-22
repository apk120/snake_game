/**
 * @file bsp.h
 *
 * @brief Holds public declarations for the BSP module.
 */
#ifndef _BSP_H
#define _BSP_H

#ifdef __cplusplus
extern "C" {
#endif

// system assert macros
#define SYS_ASSERT(exp)                \
    if(!(exp))                         \
    {                                  \
        bsp_halt(__FILE__, __LINE__);  \
    }

#define SYS_FAIL(void)  bsp_halt(__FILE__, __LINE__);

// user controlled LEDs
#define LED1_RED_PIN   ((uint16_t) 0x0001)
#define LED2_RED_PIN   ((uint16_t) 0x0001)
#define LED2_GREEN_PIN ((uint16_t) 0x0002)
#define LED2_BLUE_PIN  ((uint16_t) 0x0004)

extern void bsp_init(void);
extern void bsp_led1_on(uint16_t const);
extern void bsp_led1_off(uint16_t const);
extern void bsp_led2_on(uint16_t const);
extern void bsp_led2_off(uint16_t const);
extern void bsp_halt(char const *, const int16_t);

#ifdef __cplusplus
}
#endif

#endif

