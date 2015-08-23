#include "mod_comm.h"
#include "..\ucPeripheralDrivers\timer_.h"
#include "..\ucPeripheralDrivers\RTC_.h"
#include "mod_comm_gpio_opt.h"

#ifdef DEBUG
    #include "..\ucPeripheralDrivers\leuart_connection.h"
    #include <stdio.h>
#endif

ModComm_Settings_t g_ModComm_Settings;
volatile ModComm_Message_t  g_ModComm_Message;

uint8_t message[MOD_MESSAGE_BUFF_SIZE];
uint8_t i_ByteMessage;
uint8_t i_BitMessage;

bool g_Master;

volatile bool g_clkHigh;

/* -----------------------------------------------------*/
/*                     Initialize                       */
/* -----------------------------------------------------*/
void ModComm_Init(ModComm_Settings_t ModComm_Settings, bool Master)
{
    g_ModComm_Settings = ModComm_Settings;
    g_ModComm_Message.inProgress = false;
    g_Master = Master;
    ModComm_Init_ReadData();
    Timer_Init();
}

void ModComm_Init_ReadData(void)
{
    ModComm_Data_Input(1);
    ModComm_Clk_Input(1);
    ModComm_Busy_Input(1);

    /* Stop timer */
    Timer_DisableIRQ();
}


/* -----------------------------------------------------*/
/*                          IRQ                         */
/* -----------------------------------------------------*/
void ModComm_Enable_BusyIRQ(void)
{
    if ((g_ModComm_Settings.busy_pin & 0x01) == 0)
    {
        /* Enable GPIO_EVEN interrupt in NVIC */
        NVIC_EnableIRQ(GPIO_EVEN_IRQn);
    }
    else
    {
        /* Enable GPIO_ODD interrupt in NVIC */
        NVIC_EnableIRQ(GPIO_ODD_IRQn);
    }
    GPIO_IntConfig(g_ModComm_Settings.busy_port, g_ModComm_Settings.busy_pin, false, true, true);
}
void ModComm_Disable_BusyIRQ(void)
{
    if ((g_ModComm_Settings.busy_pin & 0x01) == 0)
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

bool ModComm_Progress(void)
{
    return (g_ModComm_Message.inProgress);
}

void ModComm_GPIO_IRQ(void)
{
    bool dataPin =  ModComm_Data_Read();
    bool busyPin =  ModComm_Busy_Read();

    /* Clk line irq */
    if (g_ModComm_Message.inProgress)
    {
        if (!busyPin)
        {
            if (dataPin)
            {
                message[i_ByteMessage] |= (1 << i_BitMessage);
            }
            else
            {
                message[i_ByteMessage] &= ~(1 << i_BitMessage);
            }

            if (i_BitMessage == 0)
            {
                i_BitMessage = 7;
                i_ByteMessage++;
            }
            else
            {
                i_BitMessage--;
            }
        }
        /* Busy pin set to Vcc this mean the transmition is over */
        else
        {
            /* Disable CLk interrupts */
            GPIO_IntConfig(g_ModComm_Settings.clk_port, g_ModComm_Settings.clk_pin, false, false, true);

            /* Set busy pin interrupts to falling edge */
            GPIO_IntConfig(g_ModComm_Settings.busy_port, g_ModComm_Settings.busy_pin, false, true, true);

            g_ModComm_Message.inProgress = false;
            g_ModComm_Message.finished = true;
            g_ModComm_Message.mod_id = message[0];
            g_ModComm_Message.message = &message[1];
            g_ModComm_Message.length = i_ByteMessage;

#ifdef DEBUG
            char buff[30];
            LeUart_SendChar('\n');
            for (int i = 0; i < i_ByteMessage; i++)
            {
                sprintf(buff, "\t0x%02x\t",message[i]);
                LeUart_SendText(buff);
            }
            LeUart_SendText("\n\n");
#endif
        }
    }
    /* Busy line irq */
    else
    {
        bool clkPin = ModComm_Clk_Read();
        bool messageOk = false;
        if (!clkPin)
        {
            if (!dataPin)
            {
                /* Read command send */
#ifdef DEBUG
                LeUart_SendChar('r');
#endif
                g_ModComm_Message.mess_type = MOD_MESS_READ;
                messageOk = true;
            }
            else
            {
                /* Write command send */
#ifdef DEBUG
                LeUart_SendChar('w');
#endif
                g_ModComm_Message.mess_type = MOD_MESS_WRITE;
                messageOk = true;
            }
        }
        else
        {
            if (!dataPin)
            {
                /* Broadcast command send */
#ifdef DEBUG
                LeUart_SendChar('b');
#endif
                g_ModComm_Message.mess_type = MOD_MESS_BROADCAST;
                /* Master is reading broadcast messages */
                if (g_Master)
                {
                    messageOk = true;
                }

            }
            else
            {
                /* Error state this should not happen */
#ifdef DEBUG
                LeUart_SendChar('e');
#endif
                g_ModComm_Message.mess_type = MOD_MESS_ERROR;
                /* messageOk = false; */
            }
        }
        if (messageOk)
        {
            g_ModComm_Message.inProgress = true;
            g_ModComm_Message.finished = false;

            i_ByteMessage = 0;
            i_BitMessage = 7;

            /* Set busy pin interrupts to rising edge */
            GPIO_IntConfig(g_ModComm_Settings.busy_port, g_ModComm_Settings.busy_pin, true, false, true);
            /* Init CLK interrupts */
            GPIO_IntConfig(g_ModComm_Settings.clk_port, g_ModComm_Settings.clk_pin, true, false, true);
        }
    }
}

void TIMER1_IRQHandler(void)
{
    static uint8_t i_BitToSend = 0x07;
    static uint8_t i_ByteToSend = 0x00;
    /* on the falling edge the data is set */
    GPIO_PinOutToggle(g_ModComm_Settings.clk_port, g_ModComm_Settings.clk_pin);
    if (g_clkHigh)
    {
        uint8_t ByteToSend = 0x00;
        switch (g_ModComm_Message.partToSend)
        {
        case MOD_SEND_MESSAGE:
            ByteToSend = g_ModComm_Message.message[i_ByteToSend];
            break;
        case MOD_SEND_ID:
            ByteToSend = g_ModComm_Message.mod_id;
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
    /* on the rising edge of clk the data is read */
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
        case MOD_SEND_ID:
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

/* -----------------------------------------------------*/
/*                   Broadcast message                  */
/* -----------------------------------------------------*/
inline void ModComm_SetPinsInBroadcastMode(void)
{
    /* Disable busy pin interrupts */
    GPIO_IntConfig(g_ModComm_Settings.busy_port, g_ModComm_Settings.busy_pin, false, false, true);

    /* To indicate the BROADCAST command
     * Data     line is set to GND
     * Clk      line is set to Vcc
     * And Busy line is pulled down */
    ModComm_Data_Output(0);
    ModComm_Clk_Output(1);
    ModComm_Busy_Output(0);
    g_clkHigh = true;
}

void ModComm_Broadcast(uint8_t id, uint8_t *message, uint8_t len, uint8_t *CRC)
{
    g_ModComm_Message.mess_type = MOD_MESS_BROADCAST;
    g_ModComm_Message.mod_id = id;
    g_ModComm_Message.message = message;
    g_ModComm_Message.length = len;
    g_ModComm_Message.crc = CRC;
    g_ModComm_Message.partToSend = MOD_SEND_ID;
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

void ModComm_ReceiveBroadcasts(void)
{
    ModComm_Init_ReadData();
}
