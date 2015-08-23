#ifndef MOD_COMM_H_
#define MOD_COMM_H_

#include "em_gpio.h"
#include <stdint.h>
#include <stdbool.h>

#define MOD_MESSAGE_BUFF_SIZE    128
#define MOD_CRC_SIZE             4

typedef enum
{
    MOD_MESS_READ                   =   0, /* Clk = 0, Data = 0 */
    MOD_MESS_WRITE                  =   1, /* Clk = 0, Data = 1 */
    MOD_MESS_BROADCAST              =   2, /* Clk = 1, Data = 0 */
    MOD_MESS_ERROR                  =   3  /* Clk = 1, Data = 1 */
} Mod_Mess_Type_t;

typedef enum
{
    /*MOD_SEND_TYPE                 = 0, */
    MOD_SEND_ID                     = 1,
    /*MOD_SEND_FRAME_ID             = 2, */
    MOD_SEND_MESSAGE                = 3,
    MOD_SEND_CRC                    = 4
} Mod_Mess_PartToSend_t;

typedef struct
{
    Mod_Mess_Type_t mess_type;
    uint8_t mod_id;
    uint8_t *message; /* Message maximum size is 128 bytes */
    uint8_t length;
    uint8_t *crc; /* CRC is always 4 bytes */

    Mod_Mess_PartToSend_t partToSend;

    bool finished;
    bool inProgress;
} ModComm_Message_t;

typedef struct
{
    GPIO_Port_TypeDef   data_port;
    unsigned int        data_pin;
    GPIO_Port_TypeDef   clk_port;
    unsigned int        clk_pin;
    GPIO_Port_TypeDef   busy_port;
    unsigned int        busy_pin;
} ModComm_Settings_t;

void ModComm_Init(ModComm_Settings_t modComm_Settings, bool Master);

void ModComm_Init_ReadData(void);

void ModComm_GPIO_IRQ(void);

bool ModComm_Progress(void);

void ModComm_Broadcast(uint8_t id, uint8_t *message, uint8_t len, uint8_t *CRC);

void ModComm_ReceiveBroadcasts(void);

#endif /* MOD_COMM_H_ */
