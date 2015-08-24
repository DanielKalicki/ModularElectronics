#include "mod_comm.h"
#include "..\ucPeripheralDrivers\timer_.h"
#include "..\ucPeripheralDrivers\RTC_.h"
#include "mod_comm_gpio_opt.h"

#ifdef DEBUG
    #include "..\ucPeripheralDrivers\leuart_connection.h"
    #include <stdio.h>
#endif

ModComm_GPIO_Settings_t g_ModComm_GPIO_Settings;
/* TODO check if those variables required volatile modifier */
volatile bool g_Master;
volatile uint8_t g_ModAddr;
volatile ModComm_Message_t  g_ModComm_Message;

uint8_t message[MOD_MESSAGE_BUFF_SIZE];
uint8_t i_ByteMessage;
uint8_t i_BitMessage;

volatile bool g_clkHigh;

void ModComm_Enable_BusyIRQ(void);

/* +----------------------------------------------------------------------------------------------+
 * |                                  Initialized functions                                       |
 * +----------------------------------------------------------------------------------------------+
 */

inline void ModComm_Init_ReadData(void)
{
    ModComm_Data_Input(1);
    ModComm_Clk_Input(1);
    ModComm_Busy_Input(1);

    /* Stop timer */
    Timer_DisableIRQ();
}

void ModComm_Init(ModComm_GPIO_Settings_t ModComm_GPIO_Settings, uint8_t module_addr, bool Master)
{
    g_ModComm_GPIO_Settings = ModComm_GPIO_Settings;
    g_ModComm_Message.inProgress = false;
    g_ModComm_Message.newMessageAvailable = false;
    g_ModAddr = module_addr;
    g_Master = Master;
    ModComm_Init_ReadData();
    Timer_Init();
    ModComm_Enable_BusyIRQ();
}

/* +----------------------------------------------------------------------------------------------+
 * |                                  Message status functions                                    |
 * +----------------------------------------------------------------------------------------------+
 */
bool ModComm_Progress(void)
{
    return (g_ModComm_Message.inProgress);
}

bool ModComm_NewMessageAvailable(void)
{
    return (g_ModComm_Message.newMessageAvailable);
}
ModComm_Message_t* ModComm_GetMessage(void)
{
    return (&g_ModComm_Message);
}
void ModComm_MessageDone(void)
{
    g_ModComm_Message.newMessageAvailable = false;
}
/* +----------------------------------------------------------------------------------------------+
 * |                                      GPIO interrupts                                         |
 * +----------------------------------------------------------------------------------------------+
 */
void ModComm_Enable_BusyIRQ(void)
{
    if ((g_ModComm_GPIO_Settings.busy_pin & 0x01) == 0)
    {
        /* Enable GPIO_EVEN interrupt in NVIC */
        NVIC_EnableIRQ(GPIO_EVEN_IRQn);
    }
    else
    {
        /* Enable GPIO_ODD interrupt in NVIC */
        NVIC_EnableIRQ(GPIO_ODD_IRQn);
    }
    GPIO_IntConfig(g_ModComm_GPIO_Settings.busy_port, g_ModComm_GPIO_Settings.busy_pin, false, true, true);
}
void ModComm_Disable_BusyIRQ(void)
{
    if ((g_ModComm_GPIO_Settings.busy_pin & 0x01) == 0)
    {
        /* Enable GPIO_EVEN interrupt in NVIC */
        NVIC_DisableIRQ(GPIO_EVEN_IRQn);
    }
    else
    {
        /* Enable GPIO_ODD interrupt in NVIC */
        NVIC_DisableIRQ(GPIO_ODD_IRQn);
    }
}

void ModComm_GPIO_IRQ(void)
{
    /* --------------------------
     * Read interface line states
     * -------------------------- */
    bool dataPin = ModComm_Data_Read();
    bool busyPin = ModComm_Busy_Read();

    static bool modIdChecked = false;

    /* Clk line irq */
    if (g_ModComm_Message.inProgress)
    {
        if (!busyPin)
        {
            /* -------------------------------------
             * Set the read bit value in the message
             * ------------------------------------- */
            if (dataPin)
            {
                message[i_ByteMessage] |= (1 << i_BitMessage);
            }
            else
            {
                message[i_ByteMessage] &= ~(1 << i_BitMessage);
            }

            /* -------------------------
             * Saturate the bit index
             * Increment the byte index
             * ------------------------- */
            if (i_BitMessage == 0)
            {
                i_BitMessage = 7;
                i_ByteMessage++;

                /* ---------------------------------------------------
                 * Check the module addr from the package
                 * If the module works as a master dont check the addr
                 * --------------------------------------------------- */
                if (!modIdChecked && !g_Master)
                {
                    if (message[0] == g_ModAddr)
                    {
                        modIdChecked = true;
                    }
                    else
                    {
                        /* ------------------------------------------------------------------------
                         * This package is not for this module so all other interrupts are disabled
                         *  and the only interrupt left is on the falling edge on the Busy line
                         *--------------------------------------------------------------------------*/
                        g_ModComm_Message.inProgress = false;
                        g_ModComm_Message.newMessageAvailable = false;
                        /* Disable CLk interrupts */
                        GPIO_IntConfig(g_ModComm_GPIO_Settings.clk_port, g_ModComm_GPIO_Settings.clk_pin, false, false, true);
                        /* Set busy pin interrupts to falling edge */
                        GPIO_IntConfig(g_ModComm_GPIO_Settings.busy_port, g_ModComm_GPIO_Settings.busy_pin, false, true, true);
                        /* Turn on RTC interrupts */
                        Rtc_EnableInt();
                    }
                }
            }
            /* -----------------------------------------------------------------
             * No bit saturation required. The index need only to be decremented
             * ----------------------------------------------------------------- */
            else
            {
                i_BitMessage--;
            }
        }
        /* ------------------------------------------------------
         * Busy pin set to Vcc this mean the transmition is over
         * ------------------------------------------------------ */
        else
        {
            /* Disable CLk interrupts */
            GPIO_IntConfig(g_ModComm_GPIO_Settings.clk_port, g_ModComm_GPIO_Settings.clk_pin, false, false, true);
            /* Set busy pin interrupts to falling edge */
            GPIO_IntConfig(g_ModComm_GPIO_Settings.busy_port, g_ModComm_GPIO_Settings.busy_pin, false, true, true);

            /* Turn on RTC interrupts */
            Rtc_EnableInt();

            /* -----------------------
             * Set the message fields
             * ----------------------- */
            g_ModComm_Message.inProgress = false;
            g_ModComm_Message.finished = true;
            g_ModComm_Message.newMessageAvailable = true;
            g_ModComm_Message.mod_addr = message[0];
            g_ModComm_Message.message = &message[1];
            g_ModComm_Message.length = i_ByteMessage;
        }
    }
    /* ---------------------------------
     * Busy line falling edge interrupt
     * New transmition has started
     * --------------------------------- */
    else
    {
        bool clkPin = ModComm_Clk_Read();
        bool messageOk = false;
        /* ---------------------------------------------------
         * Read the message type based on the Clk, Data states
         * ---------------------------------------------------  */
        if (!clkPin)
        {
            if (!dataPin)
            {
                /* Read command send */
                g_ModComm_Message.mess_type = MOD_MESS_READ;
                messageOk = true;
            }
            else
            {
                /* Write command send */
                g_ModComm_Message.mess_type = MOD_MESS_WRITE;
                messageOk = true;
            }
        }
        else
        {
            if (!dataPin)
            {
                /* Broadcast command send */
                g_ModComm_Message.mess_type = MOD_MESS_BROADCAST;

                /* Master wants to read broadcast messages */
                if (g_Master)
                {
                    messageOk = true;
                }

            }
            else
            {
                /* Error state this should not happen */
                g_ModComm_Message.mess_type = MOD_MESS_ERROR;
                /* messageOk = false; */
            }
        }
        /* ---------------------------------------------------
         * If message is intended for this module to be read
         * g_ModCom_Message fields are set,
         * all indexed for receiveing the message are reseted,
         * and the interrupts are started
         * --------------------------------------------------- */
        if (messageOk)
        {
            g_ModComm_Message.inProgress = true;
            g_ModComm_Message.finished = false;
            g_ModComm_Message.newMessageAvailable = false;

            i_ByteMessage = 0;
            i_BitMessage = 7;
            modIdChecked = false;

            Rtc_DisableInt();

            /* Set Busy pin interrupts to rising edge */
            GPIO_IntConfig(g_ModComm_GPIO_Settings.busy_port, g_ModComm_GPIO_Settings.busy_pin, true, false, true);
            /* Init Clk interrupts on rising edge */
            GPIO_IntConfig(g_ModComm_GPIO_Settings.clk_port, g_ModComm_GPIO_Settings.clk_pin, true, false, true);
        }
    }
}

/* +----------------------------------------------------------------------------------------------+
 * |                                     Timer interrupts                                         |
 * +----------------------------------------------------------------------------------------------+
 */
void TIMER1_IRQHandler(void)
{
    static uint8_t i_BitToSend = 0x07;
    static uint8_t i_ByteToSend = 0x00;
    /* ------------------------------------------
     * On the falling edge of clk the data is set
     * ------------------------------------------ */
    GPIO_PinOutToggle(g_ModComm_GPIO_Settings.clk_port, g_ModComm_GPIO_Settings.clk_pin);
    if (g_clkHigh)
    {
        uint8_t ByteToSend = 0x00;
        switch (g_ModComm_Message.partToSend)
        {
        case MOD_SEND_MESSAGE:
            ByteToSend = g_ModComm_Message.message[i_ByteToSend];
            break;
        case MOD_SEND_ADDR:
            ByteToSend = g_ModComm_Message.mod_addr;
            break;
        /* TODO implement MOD_SEND_CRC */
        default:
            break;
        }

        if ((ByteToSend & (1 << i_BitToSend)) > 0)
        {
            ModComm_Data_Set();
        }
        else
        {
            ModComm_Data_Clr();
        }
    }
    /* -------------------------------------------
     * On the rising edge of clk the data is read
     * ------------------------------------------- */
    else
    {
        if (i_BitToSend == 0)
        {
            i_BitToSend = 7;
            i_ByteToSend++;
        }
        else
        {
            i_BitToSend--;
        }

        switch (g_ModComm_Message.partToSend)
        {
        case MOD_SEND_MESSAGE:
            if (i_ByteToSend >= g_ModComm_Message.length)
            {
                g_ModComm_Message.finished    = true;
                g_ModComm_Message.inProgress  = false;
                i_BitToSend = 7;
                i_ByteToSend = 0;

                /* The message is finished */
                ModComm_Init_ReadData();
                ModComm_Enable_BusyIRQ();
                Timer_DisableIRQ();
                Rtc_EnableInt();
            }
            break;
        case MOD_SEND_ADDR:
            if (i_BitToSend == 7)
            {
                g_ModComm_Message.partToSend  = MOD_SEND_MESSAGE;
                i_BitToSend = 7;
                i_ByteToSend = 0;
            }
            break;
        /* TODO implement MOD_SEND_CRC */
        default:
            break;
        }
    }
    g_clkHigh = !g_clkHigh;
    Timer_ClrIRQ();
}

/* +----------------------------------------------------------------------------------------------+
 * |                                 Broadcast message functions                                  |
 * +----------------------------------------------------------------------------------------------+
 */
inline void ModComm_SetPinsInBroadcastMode(void)
{
    /* Disable busy pin interrupts */
    GPIO_IntConfig(g_ModComm_GPIO_Settings.busy_port, g_ModComm_GPIO_Settings.busy_pin, false, false, true);

    /* To indicate the BROADCAST command
     * Data     line is set to GND
     * Clk      line is set to Vcc
     * And Busy line is pulled down */
    ModComm_Data_Output(0);
    ModComm_Clk_Output(1);
    ModComm_Busy_Output(0);
    g_clkHigh = true;
}

void ModComm_Broadcast(uint8_t addr, uint8_t *message, uint8_t len, uint8_t *CRC)
{
    g_ModComm_Message.mess_type = MOD_MESS_BROADCAST;
    g_ModComm_Message.mod_addr = addr;
    g_ModComm_Message.message = message;
    g_ModComm_Message.length = len;
    g_ModComm_Message.crc = CRC;
    g_ModComm_Message.partToSend = MOD_SEND_ADDR;
    g_ModComm_Message.finished = false;
    g_ModComm_Message.inProgress = true;

    ModComm_Disable_BusyIRQ();

    ModComm_Init_ReadData();

    /* Busy High = bus is free */
    if (   1 == ModComm_Busy_Read()
        && 1 == ModComm_Clk_Read()
        && 1 == ModComm_Data_Read()
       )
    {
        ModComm_SetPinsInBroadcastMode();
        Timer_EnableIRQ();
        Rtc_DisableInt();
    }
    else
    {
        /* Wait for the busy free interrupts */
        /* TODO this */
    }
}

/* +----------------------------------------------------------------------------------------------+
 * |                                  Write message functions                                     |
 * +----------------------------------------------------------------------------------------------+
 */
inline void ModComm_SetPinsInWriteMode(void)
{
    /* Disable busy pin interrupts */
    GPIO_IntConfig(g_ModComm_GPIO_Settings.busy_port, g_ModComm_GPIO_Settings.busy_pin, false, false, true);

    /* To indicate the WRITE command
     * Data     line is set to Vcc
     * Clk      line is set to GND
     * And Busy line is pulled down */
    ModComm_Data_Output(1);
    ModComm_Clk_Output(0);
    ModComm_Busy_Output(0);
    g_clkHigh = true;
}

void ModComm_Write(uint8_t addr, uint8_t *message, uint8_t len, uint8_t *CRC)
{
    g_ModComm_Message.mess_type = MOD_MESS_WRITE;
    g_ModComm_Message.mod_addr = addr;
    g_ModComm_Message.message = message;
    g_ModComm_Message.length = len;
    g_ModComm_Message.crc = CRC;
    g_ModComm_Message.partToSend = MOD_SEND_ADDR;
    g_ModComm_Message.finished = false;
    g_ModComm_Message.inProgress = true;

    ModComm_Disable_BusyIRQ();

    ModComm_Init_ReadData();

    /* Busy High = bus is free */
    if (   1 == ModComm_Busy_Read()
        && 1 == ModComm_Clk_Read()
        && 1 == ModComm_Data_Read()
       )
    {
        ModComm_SetPinsInWriteMode();
        Timer_EnableIRQ();
        Rtc_DisableInt();
    }
    else
    {
        /* Wait for the busy free interrupts */
        /* TODO this */
    }
}
