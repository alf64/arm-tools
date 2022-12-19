/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : usbd_cdc_if.c
  * @version        : v1.0_Cube
  * @brief          : Usb device for Virtual Com Port.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "usbd_cdc_if.h"

/* USER CODE BEGIN INCLUDE */

/* USER CODE END INCLUDE */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

/* USER CODE END PV */

/** @addtogroup STM32_USB_OTG_DEVICE_LIBRARY
  * @brief Usb device library.
  * @{
  */

/** @addtogroup USBD_CDC_IF
  * @{
  */

/** @defgroup USBD_CDC_IF_Private_TypesDefinitions USBD_CDC_IF_Private_TypesDefinitions
  * @brief Private types.
  * @{
  */

/* USER CODE BEGIN PRIVATE_TYPES */

/* USER CODE END PRIVATE_TYPES */

/**
  * @}
  */

/** @defgroup USBD_CDC_IF_Private_Defines USBD_CDC_IF_Private_Defines
  * @brief Private defines.
  * @{
  */

/* USER CODE BEGIN PRIVATE_DEFINES */
/* USER CODE END PRIVATE_DEFINES */

/**
  * @}
  */

/** @defgroup USBD_CDC_IF_Private_Macros USBD_CDC_IF_Private_Macros
  * @brief Private macros.
  * @{
  */

/* USER CODE BEGIN PRIVATE_MACRO */
#define CDC_BLUE_LED_MSG_SIZE 16
#define CDC_RECEIVED_MSG_SIZE 11
#define CDC_RECEIVEDDATA_MSG_SIZE 16
#define CDC_BTSOFDATA_MSG_SIZE 16
#define CDC_NEULINE_MSG_SIZE 2
/* USER CODE END PRIVATE_MACRO */

/**
  * @}
  */

/** @defgroup USBD_CDC_IF_Private_Variables USBD_CDC_IF_Private_Variables
  * @brief Private variables.
  * @{
  */
/* Create buffer for reception and transmission           */
/* It's up to user to redefine and/or remove those define */
/** Received data over USB are stored in this buffer      */
uint8_t UserRxBufferFS[APP_RX_BUFFER_SIZE];

/** Data to send over USB CDC are stored in this buffer   */
uint8_t UserTxBufferFS[APP_TX_BUFFER_SIZE];

/* USER CODE BEGIN PRIVATE_VARIABLES */
#if CDC_BLUE_LED_MSG_SIZE > APP_TX_DATA_SIZE
#error "CDC_BLUE_LED_MSG_SIZE greater than APP_TX_DATA_SIZE is not allowed!"
#endif
static const char CDC_BlueLedMsgs[2][CDC_BLUE_LED_MSG_SIZE] = {
        "Blue LED: OFF \n",
        "Blue LED: ON  \n"
};
static const char CDC_ReceivedMsg[CDC_RECEIVED_MSG_SIZE] = "Received: ";
static const char CDC_BtsOfDataMsg[] = " bytes of data.";
static const char CDC_NeuLineMsg[CDC_NEULINE_MSG_SIZE] = "\n";
static volatile cdc_rcv_status_t cdc_rcv_status = CDC_RCV_STATUS_NO_DATA_AVAILABLE;
static const char CDC_ReceivedDataMsg[CDC_RECEIVEDDATA_MSG_SIZE] = "Received data: ";
static volatile uint8_t CDC_RcvData[APP_RX_BUFFER_SIZE];
static volatile uint32_t CDC_RcvDataLen = 0;
static volatile bool CDC_tx_pending = FALSE; // transfer pending flag
/* USER CODE END PRIVATE_VARIABLES */

/**
  * @}
  */

/** @defgroup USBD_CDC_IF_Exported_Variables USBD_CDC_IF_Exported_Variables
  * @brief Public variables.
  * @{
  */

extern USBD_HandleTypeDef hUsbDeviceFS;

/* USER CODE BEGIN EXPORTED_VARIABLES */

/* USER CODE END EXPORTED_VARIABLES */

/**
  * @}
  */

/** @defgroup USBD_CDC_IF_Private_FunctionPrototypes USBD_CDC_IF_Private_FunctionPrototypes
  * @brief Private functions declaration.
  * @{
  */

static int8_t CDC_Init_FS(void);
static int8_t CDC_DeInit_FS(void);
static int8_t CDC_Control_FS(uint8_t cmd, uint8_t* pbuf, uint16_t length);
static int8_t CDC_Receive_FS(uint8_t* pbuf, uint32_t *Len);
static int8_t CDC_TransmitCplt_FS(uint8_t *pbuf, uint32_t *Len, uint8_t epnum);

/* USER CODE BEGIN PRIVATE_FUNCTIONS_DECLARATION */

/* USER CODE END PRIVATE_FUNCTIONS_DECLARATION */

/**
  * @}
  */

USBD_CDC_ItfTypeDef USBD_Interface_fops_FS =
{
  CDC_Init_FS,
  CDC_DeInit_FS,
  CDC_Control_FS,
  CDC_Receive_FS,
  CDC_TransmitCplt_FS
};

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Initializes the CDC media low layer over the FS USB IP
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_Init_FS(void)
{
  /* USER CODE BEGIN 3 */
    /*
     * ----- Brief -----
     * CDC_Init_FS() is a kind of callback called whenever USB connection is established.
     */
    HAL_GPIO_WritePin(LD4_GPIO_Port, LD4_Pin, GPIO_PIN_SET);
  /* Set Application Buffers */
  USBD_CDC_SetTxBuffer(&hUsbDeviceFS, UserTxBufferFS, 0);
  USBD_CDC_SetRxBuffer(&hUsbDeviceFS, UserRxBufferFS);
  return (USBD_OK);
  /* USER CODE END 3 */
}

/**
  * @brief  DeInitializes the CDC media low layer
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_DeInit_FS(void)
{
  /* USER CODE BEGIN 4 */
    /*
     * ----- Brief -----
     * CDC_DeInit_FS() is a kind of callback called whenever USB connection is lost.
     */
    HAL_GPIO_WritePin(LD4_GPIO_Port, LD4_Pin, GPIO_PIN_RESET);
  return (USBD_OK);
  /* USER CODE END 4 */
}

/**
  * @brief  Manage the CDC class requests
  * @param  cmd: Command code
  * @param  pbuf: Buffer containing command data (request parameters)
  * @param  length: Number of data to be sent (in bytes)
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_Control_FS(uint8_t cmd, uint8_t* pbuf, uint16_t length)
{
  /* USER CODE BEGIN 5 */
  switch(cmd)
  {
    case CDC_SEND_ENCAPSULATED_COMMAND:

    break;

    case CDC_GET_ENCAPSULATED_RESPONSE:

    break;

    case CDC_SET_COMM_FEATURE:

    break;

    case CDC_GET_COMM_FEATURE:

    break;

    case CDC_CLEAR_COMM_FEATURE:

    break;

  /*******************************************************************************/
  /* Line Coding Structure                                                       */
  /*-----------------------------------------------------------------------------*/
  /* Offset | Field       | Size | Value  | Description                          */
  /* 0      | dwDTERate   |   4  | Number |Data terminal rate, in bits per second*/
  /* 4      | bCharFormat |   1  | Number | Stop bits                            */
  /*                                        0 - 1 Stop bit                       */
  /*                                        1 - 1.5 Stop bits                    */
  /*                                        2 - 2 Stop bits                      */
  /* 5      | bParityType |  1   | Number | Parity                               */
  /*                                        0 - None                             */
  /*                                        1 - Odd                              */
  /*                                        2 - Even                             */
  /*                                        3 - Mark                             */
  /*                                        4 - Space                            */
  /* 6      | bDataBits  |   1   | Number Data bits (5, 6, 7, 8 or 16).          */
  /*******************************************************************************/
    case CDC_SET_LINE_CODING:

        /*
         * We could configure UART parameters here in case our system work as USB -> UART converter,
         * as in this case hosts sends us preferred Line Coding Structure.
         */

    break;

    case CDC_GET_LINE_CODING:
    {
        /*
         * This part of code is being executed when host asks for UART transmission parameters at usb startup.
         * Our system does not act as USB -> UART converter, so we can tell host whatever parameters as long as
         * they make sense.
         * These parameters does only makes true sense if UART would be configured on our side.
         * You should not fill LineCoding with random data or just leave it be, as it may cause errors
         * on OS side (unable to open COM port).
         */
        USBD_CDC_LineCodingTypeDef LineCoding;
        LineCoding.bitrate = 115200;
        LineCoding.format = 0; // 1 stop bit
        LineCoding.paritytype = 0; // no parity bits
        LineCoding.datatype = 8; // 8 data bits
        memcpy(pbuf, &LineCoding, sizeof(LineCoding));

        break;
    }

    case CDC_SET_CONTROL_LINE_STATE:

    break;

    case CDC_SEND_BREAK:

    break;

  default:
    break;
  }

  return (USBD_OK);
  /* USER CODE END 5 */
}

/**
  * @brief  Data received over USB OUT endpoint are sent over CDC interface
  *         through this function.
  *
  *         @note
  *         This function will issue a NAK packet on any OUT packet received on
  *         USB endpoint until exiting this function. If you exit this function
  *         before transfer is complete on CDC interface (ie. using DMA controller)
  *         it will result in receiving more data while previous ones are still
  *         not sent.
  *
  * @param  Buf: Buffer of data to be received
  * @param  Len: Number of data received (in bytes)
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_Receive_FS(uint8_t* Buf, uint32_t *Len)
{
  /* USER CODE BEGIN 6 */
  USBD_CDC_SetRxBuffer(&hUsbDeviceFS, &Buf[0]);
  USBD_CDC_ReceivePacket(&hUsbDeviceFS);

  /*
   * ----- Brief -----
   * CDC_Receive_FS() is a kind of callback called upon receiving data.
   *
   * ----- Hint -----:
   * Buf should be our UserRxBufferFS,
   * since USBD_CDC_SetRxBuffer() already set it in CDC_Init_FS() function.
   *
   * ----- Important: -----
   * Debug shows that if i.e. *Len=3, actual data size written under UserRxBufferFS is 4.
   * That is why UserRxBufferFS is of size: APP_RX_BUFFER_SIZE, not APP_RX_DATA_SIZE.
   * Exceeding APP_RX_DATA_SIZE causes memory leak (data after UserRxBufferFS being overwritten),
   * thus we check for such situation and treat it as critical memory fault.
   */
  if(*Len > APP_RX_DATA_SIZE)
  {
      // possible memory leak - system data may be corrupted!
      cdc_rcv_status = CDC_RCV_STATUS_CRITICAL_ERROR;
  }
  else
  {
      cdc_rcv_status = CDC_RCV_STATUS_DATA_AVAILABLE;
      memcpy((void*)CDC_RcvData, Buf, *Len);
      CDC_RcvDataLen = *Len;
  }

  return (USBD_OK);
  /* USER CODE END 6 */
}

/**
  * @brief  CDC_Transmit_FS
  *         Data to send over USB IN endpoint are sent over CDC interface
  *         through this function.
  *         @note
  *
  *
  * @param  Buf: Buffer of data to be sent
  * @param  Len: Number of data to be sent (in bytes)
  * @retval USBD_OK if all operations are OK else USBD_FAIL or USBD_BUSY
  */
uint8_t CDC_Transmit_FS(uint8_t* Buf, uint16_t Len)
{
  uint8_t result = USBD_OK;
  /* USER CODE BEGIN 7 */
  USBD_CDC_HandleTypeDef *hcdc = (USBD_CDC_HandleTypeDef*)hUsbDeviceFS.pClassData;
  if (hcdc->TxState != 0){
    return USBD_BUSY;
  }
  USBD_CDC_SetTxBuffer(&hUsbDeviceFS, Buf, Len);
  CDC_tx_pending = TRUE;
  result = USBD_CDC_TransmitPacket(&hUsbDeviceFS);
  if(result != USBD_OK)
  {
      CDC_tx_pending = FALSE;
  }
  /* USER CODE END 7 */
  return result;
}

/**
  * @brief  CDC_TransmitCplt_FS
  *         Data transmited callback
  *
  *         @note
  *         This function is IN transfer complete callback used to inform user that
  *         the submitted Data is successfully sent over USB.
  *
  * @param  Buf: Buffer of data to be received
  * @param  Len: Number of data received (in bytes)
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_TransmitCplt_FS(uint8_t *Buf, uint32_t *Len, uint8_t epnum)
{
  uint8_t result = USBD_OK;
  /* USER CODE BEGIN 13 */
  CDC_tx_pending = FALSE;
  UNUSED(Buf);
  UNUSED(Len);
  UNUSED(epnum);
  /* USER CODE END 13 */
  return result;
}

/* USER CODE BEGIN PRIVATE_FUNCTIONS_IMPLEMENTATION */
/*
 * @brief
 * Sends message via USB CDC about current Blue LED state.
 *
 * @details
 * If en is 1 - will send message informing that blue LED is ON.
 * If en is 0 - will send message informing that blue LED is OFF.
 *
 * @returns error code
 * @retval 0 if succeeded
 * @retval !0 if failed
 */
uint8_t SendBlueLEDMsg(bool en)
{
    if(en > 1)
    {
        return 1;
    }
    memcpy(UserTxBufferFS, &(CDC_BlueLedMsgs[en][0]), CDC_BLUE_LED_MSG_SIZE);
    return CDC_Transmit_FS(UserTxBufferFS, CDC_BLUE_LED_MSG_SIZE);
}

void CDC_ToggleBlueLedMsg()
{
    static bool en = 0;
    en ^= 1;
    SendBlueLEDMsg(en);
}

void CDC_CriticalErrorHandler()
{
    const uint8_t maxmsgsize = 100;
    char toomuchmsg[maxmsgsize];
    int len = sprintf(
            toomuchmsg,
            "CDC Critical Error!\nToo much data received. Max is: %lu bytes.\nPlease reset the system.\n",
            (uint32_t)APP_RX_DATA_SIZE);
    if(len < maxmsgsize)
    {
        CDC_Transmit_FS((uint8_t*)toomuchmsg, len);
    }
    HAL_NVIC_DisableIRQ(OTG_FS_IRQn); // disable usb irq
    while(1)
    {
        HAL_GPIO_TogglePin(LD5_GPIO_Port, LD5_Pin);
        HAL_Delay(300);
    }
}

void CDC_ReplyToUser(uint8_t* ReceivedData, uint32_t ReceivedDataSize)
{
    uint32_t tx_data_len = CDC_RECEIVED_MSG_SIZE;
    char len_str[11]; // UINT32_MAX: 4294967295 is of 10 digits + 1 for NULL
    uint8_t len_str_size = sprintf(len_str, "%lu", ReceivedDataSize); // convert *Len to str
    tx_data_len += len_str_size + CDC_BTSOFDATA_MSG_SIZE + CDC_NEULINE_MSG_SIZE;
    if(tx_data_len <= APP_TX_DATA_SIZE)
    {
        memset(UserTxBufferFS, 0, APP_TX_BUFFER_SIZE);
        memcpy(UserTxBufferFS, CDC_ReceivedMsg, sizeof(CDC_ReceivedMsg));
        strcat((char*)UserTxBufferFS, len_str);
        strcat((char*)UserTxBufferFS, CDC_BtsOfDataMsg);
        strcat((char*)UserTxBufferFS, CDC_NeuLineMsg);
        CDC_Transmit_FS(UserTxBufferFS, tx_data_len);
    }

    while(CDC_tx_pending){}; // wait for previous transfer completion

    tx_data_len = CDC_RECEIVEDDATA_MSG_SIZE + CDC_RcvDataLen + CDC_NEULINE_MSG_SIZE;
    if(tx_data_len <= APP_TX_DATA_SIZE)
    {
        memset(UserTxBufferFS, 0, APP_TX_BUFFER_SIZE);
        memcpy(UserTxBufferFS, CDC_ReceivedDataMsg, sizeof(CDC_ReceivedDataMsg));
        memcpy(
                (UserTxBufferFS+sizeof(CDC_ReceivedDataMsg)),
                ReceivedData,
                ReceivedDataSize);
        memcpy(
                (UserTxBufferFS+sizeof(CDC_ReceivedDataMsg)+ReceivedDataSize),
                CDC_NeuLineMsg,
                sizeof(CDC_NeuLineMsg));
        CDC_Transmit_FS(UserTxBufferFS, tx_data_len);
    }
}

void CDC_OnReceivedData()
{
    // save global cdc rcv status and reset it
    cdc_rcv_status_t cdc_status = cdc_rcv_status;
    cdc_rcv_status = CDC_RCV_STATUS_NO_DATA_AVAILABLE;

    switch(cdc_status)
    {
        case CDC_RCV_STATUS_DATA_AVAILABLE:
        {
            CDC_ReplyToUser((uint8_t*)CDC_RcvData, CDC_RcvDataLen);
            break;
        }
        case CDC_RCV_STATUS_CRITICAL_ERROR:
        {
            CDC_CriticalErrorHandler();
            break;
        }
        case CDC_RCV_STATUS_NO_DATA_AVAILABLE:
        default:
        {
            break;
        }
    }
}
/* USER CODE END PRIVATE_FUNCTIONS_IMPLEMENTATION */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
