#ifndef MOD_COMM_GPIO_OPT_H_
#define MOD_COMM_GPIO_OPT_H_

#include <stdint.h>
#include "em_gpio.h"
#include "em_device.h"

#define MOD_COMM_DATA_PORT  gpioPortC
#define MOD_COMM_CLK_PORT   gpioPortC
#define MOD_COMM_BUSY_PORT  gpioPortB

#define MOD_COMM_DATA_PIN  0
#define MOD_COMM_CLK_PIN   1
#define MOD_COMM_BUSY_PIN  7

inline unsigned int ModComm_Data_Read(void)
{
    return((unsigned int)((GPIO->P[MOD_COMM_DATA_PORT].DIN >> MOD_COMM_DATA_PIN) & 0x1));
}
inline unsigned int ModComm_Clk_Read(void)
{
    return((unsigned int)((GPIO->P[MOD_COMM_CLK_PORT].DIN >> MOD_COMM_CLK_PIN) & 0x1));
}
inline unsigned int ModComm_Busy_Read(void)
{
    return((unsigned int)((GPIO->P[MOD_COMM_BUSY_PORT].DIN >> MOD_COMM_BUSY_PIN) & 0x1));
}

inline void ModComm_Data_Set(void)
{
    GPIO->P[MOD_COMM_DATA_PORT].DOUTSET = 1 << MOD_COMM_DATA_PIN;
}
inline void ModComm_Clk_Set(void)
{
    GPIO->P[MOD_COMM_CLK_PORT].DOUTSET = 1 << MOD_COMM_CLK_PIN;
}
inline void ModComm_Busy_Set(void)
{
    GPIO->P[MOD_COMM_BUSY_PORT].DOUTSET = 1 << MOD_COMM_BUSY_PIN;
}

inline void ModComm_Data_Clr(void)
{
    GPIO->P[MOD_COMM_DATA_PORT].DOUTCLR = 1 << MOD_COMM_DATA_PIN;
}
inline void ModComm_Clk_Clr(void)
{
    GPIO->P[MOD_COMM_CLK_PORT].DOUTCLR = 1 << MOD_COMM_CLK_PIN;
}
inline void ModComm_Busy_Clr(void)
{
    GPIO->P[MOD_COMM_BUSY_PORT].DOUTCLR = 1 << MOD_COMM_BUSY_PIN;
}

inline void ModComm_Data_Input(uint8_t out)
{
    if (out)
    {
        GPIO ->P[MOD_COMM_DATA_PORT].DOUTSET = 1 << MOD_COMM_DATA_PIN;
    }
    else
    {
        GPIO ->P[MOD_COMM_DATA_PORT].DOUTCLR = 1 << MOD_COMM_DATA_PIN;
    }

    GPIO ->P[MOD_COMM_DATA_PORT].MODEL = (GPIO ->P[MOD_COMM_DATA_PORT].MODEL & ~(0xF << (MOD_COMM_DATA_PIN * 4))) |
            (gpioModeInput << (MOD_COMM_DATA_PIN * 4));
}
inline void ModComm_Clk_Input(uint8_t out)
{
    if (out)
    {
        GPIO ->P[MOD_COMM_CLK_PORT].DOUTSET = 1 << MOD_COMM_CLK_PIN;
    }
    else
    {
        GPIO ->P[MOD_COMM_CLK_PORT].DOUTCLR = 1 << MOD_COMM_CLK_PIN;
    }

    GPIO ->P[MOD_COMM_CLK_PORT].MODEL = (GPIO ->P[MOD_COMM_CLK_PORT].MODEL & ~(0xF << (MOD_COMM_CLK_PIN * 4))) |
            (gpioModeInput << (MOD_COMM_CLK_PIN * 4));
}
inline void ModComm_Busy_Input(uint8_t out)
{
    if (out)
    {
        GPIO ->P[MOD_COMM_BUSY_PORT].DOUTSET = 1 << MOD_COMM_BUSY_PIN;
    }
    else
    {
        GPIO ->P[MOD_COMM_BUSY_PORT].DOUTCLR = 1 << MOD_COMM_BUSY_PIN;
    }

    GPIO ->P[MOD_COMM_BUSY_PORT].MODEL = (GPIO ->P[MOD_COMM_BUSY_PORT].MODEL & ~(0xF << (MOD_COMM_BUSY_PIN * 4))) |
            (gpioModeInput << (MOD_COMM_BUSY_PIN * 4));
}

inline void ModComm_Data_Output(uint8_t out)
{
    if (out)
    {
        GPIO ->P[MOD_COMM_DATA_PORT].DOUTSET = 1 << MOD_COMM_DATA_PIN;
    }
    else
    {
        GPIO ->P[MOD_COMM_DATA_PORT].DOUTCLR = 1 << MOD_COMM_DATA_PIN;
    }

    GPIO ->P[MOD_COMM_DATA_PORT].MODEL = (GPIO ->P[MOD_COMM_DATA_PORT].MODEL & ~(0xF << (MOD_COMM_DATA_PIN * 4))) |
            (gpioModePushPull << (MOD_COMM_DATA_PIN * 4));
}
inline void ModComm_Clk_Output(uint8_t out)
{
    if (out)
    {
        GPIO ->P[MOD_COMM_CLK_PORT].DOUTSET = 1 << MOD_COMM_CLK_PIN;
    }
    else
    {
        GPIO ->P[MOD_COMM_CLK_PORT].DOUTCLR = 1 << MOD_COMM_CLK_PIN;
    }

    GPIO ->P[MOD_COMM_CLK_PORT].MODEL = (GPIO ->P[MOD_COMM_CLK_PORT].MODEL & ~(0xF << (MOD_COMM_CLK_PIN * 4))) |
            (gpioModePushPull << (MOD_COMM_CLK_PIN * 4));
}
inline void ModComm_Busy_Output(uint8_t out)
{
    if (out)
    {
        GPIO ->P[MOD_COMM_BUSY_PORT].DOUTSET = 1 << MOD_COMM_BUSY_PIN;
    }
    else
    {
        GPIO ->P[MOD_COMM_BUSY_PORT].DOUTCLR = 1 << MOD_COMM_BUSY_PIN;
    }

    GPIO ->P[MOD_COMM_BUSY_PORT].MODEL = (GPIO ->P[MOD_COMM_BUSY_PORT].MODEL & ~(0xF << (MOD_COMM_BUSY_PIN * 4))) |
            (gpioModePushPull << (MOD_COMM_BUSY_PIN * 4));
}

#endif /* MOD_COMM_GPIO_OPT_H_ */
