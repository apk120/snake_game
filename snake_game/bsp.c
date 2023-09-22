/**
 * @file bsp.c
 *
 * @brief Holds board support package routines for the MSP432 launchpad.
 */
#include <stdint.h>
#include "bsp.h"

// hardware register write
#define HWREG16(x)     (*((volatile uint16_t *)(x)))

// LED1, LED2 constants
#define LED1_BASE_ADDR ((uint32_t) 0x40004c00)
#define LED2_BASE_ADDR ((uint32_t) 0x40004c01)
#define OFS_PASEL0     ((uint16_t) 0x000A)         // Port A Select 0
#define OFS_PASEL1     ((uint16_t) 0x000C)         // Port A Select 1
#define OFS_PADIR      ((uint16_t) 0x0004)         // Port A Direction
#define OFS_PAOUT      ((uint16_t) 0x0002)         // Port A output

// if a system assert fails, flash the red led
#define LED_FAST_FLASH_PAUSE ((uint32_t) 0xfffff)

// static function declarations
static void     safe_mode(void);
static void     led_blocking_wait(void);
static uint32_t bsp_master_int_disable(void);

/**
 * Initialize the BSP module. Must be called during system initialization.
 */
void bsp_init(void)
{
    // configure user controlled LEDs as outputs
    uint32_t base_addr = LED1_BASE_ADDR;
    HWREG16(base_addr + OFS_PASEL0) &= ~LED1_RED_PIN;
    HWREG16(base_addr + OFS_PASEL1) &= ~LED1_RED_PIN;
    HWREG16(base_addr + OFS_PADIR)  |=  LED1_RED_PIN;

    base_addr = LED2_BASE_ADDR;
    HWREG16(base_addr + OFS_PASEL0) &= ~LED2_BLUE_PIN;
    HWREG16(base_addr + OFS_PASEL1) &= ~LED2_BLUE_PIN;
    HWREG16(base_addr + OFS_PADIR)  |=  LED2_BLUE_PIN;

    HWREG16(base_addr + OFS_PASEL0) &= ~LED2_GREEN_PIN;
    HWREG16(base_addr + OFS_PASEL1) &= ~LED2_GREEN_PIN;
    HWREG16(base_addr + OFS_PADIR)  |=  LED2_GREEN_PIN;

    HWREG16(base_addr + OFS_PASEL0) &= ~LED2_RED_PIN;
    HWREG16(base_addr + OFS_PASEL1) &= ~LED2_RED_PIN;
    HWREG16(base_addr + OFS_PADIR)  |=  LED2_RED_PIN;

    // turn the LEDs off
    HWREG16(LED1_BASE_ADDR + OFS_PAOUT) &= ~LED1_RED_PIN;
    HWREG16(LED2_BASE_ADDR + OFS_PAOUT) &=
            ~(LED2_BLUE_PIN | LED2_GREEN_PIN | LED2_RED_PIN);
}

/**
 *
 * Turn the specified pin(s) on.
 *
 * @param pins is the bit-packed representation of the pin(s).
 */
void bsp_led1_on (uint16_t const pins)
{
    HWREG16(LED1_BASE_ADDR + OFS_PAOUT) |= pins;
}

/**
 *
 * Turn the specified pin(s) off.
 *
 * @param pins is the bit-packed representation of the pin(s).
 */
void bsp_led1_off (uint16_t const pins)
{
    HWREG16(LED1_BASE_ADDR + OFS_PAOUT) &= ~pins;
}

/**
 *
 * Turn the specified pin(s) on.
 *
 * @param pins is the bit-packed representation of the pin(s).
 */
void bsp_led2_on (uint16_t const pins)
{
    HWREG16(LED2_BASE_ADDR + OFS_PAOUT) |= pins;
}

/**
 *
 * Turn the specified pin(s) off.
 *
 * @param pins is the bit-packed representation of the pin(s).
 */
void bsp_led2_off (uint16_t const pins)
{
    HWREG16(LED2_BASE_ADDR + OFS_PAOUT) &= ~pins;
}

/**
 * Disable processor interrupts.
 */
static uint32_t bsp_master_int_disable (void)
{
    //
    // Read PRIMASK and disable interrupts.
    //
    __asm("    mrs     r0, PRIMASK\n"
            "    cpsid   i\n"
            "    bx      lr\n");

    //
    // The following keeps the compiler happy, because it wants to see a
    // return value from this function.  It will generate code to return
    // a zero.  However, the real return is the "bx lr" above, so the
    // return(0) is never executed and the function returns with the value
    // you expect in R0.
    //
    return(0);
}

/**
 * An unrecoverable error has occurred. This function is called when an
 * assert has failed.
 *
 * @param[in] file_name - file name where the assert failed
 * @param[in] line_num - line number of the failed assert
 */
//lint -e{715} unused arguments
void bsp_halt (char const *file_name, int16_t const line_num)
{
    //! @todo if possible, log the filename and line number

    safe_mode();
}

/**
 * We have encountered a serious system error. Take the appropriate action to
 * get the system into a safe state.
 */
static void safe_mode (void)
{
    // we are here forever!
    (void) bsp_master_int_disable();

    for(;;)
    {
        // flash the red led
        bsp_led1_on(LED1_RED_PIN);
        led_blocking_wait();
        bsp_led1_off(LED1_RED_PIN);
        led_blocking_wait();
    }
}

/**
 * Blocking call used to flash an LED.
 */
static void led_blocking_wait (void)
{
    volatile uint32_t loop = LED_FAST_FLASH_PAUSE;

    while (loop-- > 0)
    {
    };
}

