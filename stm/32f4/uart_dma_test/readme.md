## **uart_dma_test**

A project that shows how to perform simple operations on UART with DMA.
HAL_UART, HAL_DMA, HAL_GPIO, HAL_Delay functionalities are used here.

Upon loading, program:
 - inits DMA and UART
 - enables reception from UART assosiated with DMA
 DMA for UART RX is configured in mode: circular. This means when the reception buffer is filled, DMA starts
 to put subsequent received bytes starting from the beginning of this buffer. In the normal mode, DMA would simply stop when buffer is filled.
 This gives an endless reception in a circular fashion.
 - sends every 1300 miliseconds half of the circular buffer via UART with DMA. DMA for UART TX is configured in mode: normal.
 This means that when the send of the given data (half of circular buffer) is done, DMA stops putting data into UART.
 That's why send is triggered manually every 1300 miliseconds.
 If DMA for UART TX would be configured in circular mode, DMA would keep sending the data forever in circular fashion,
 until you tell it to stop i.e. by using HAL_UART_DMA_Pause()
 
In general, this program:
Has 32 Bytes of cirular buffer, which is treated as 2-halfs each being 16 Bytes.
Program starts by sending data from the 2nd half of this buffer, while receiving data into the 1st half.
When the 1st half of the circular buffer is filled with received data, program switches to send the 1st half from now on, and fill 2nd half.
When the 2nd half of the circular buffer is filled with received data, program switches to send the 2nd half from now on, and fill 1st half.
This is done in an infinite loop.
HAL_UART_RxHalfCpltCallback() is used to handle interrupt when half of buffer is filled.
Please note that this callback can only be used when UART is configured with DMA, as it's being called by UART_DMARxHalfCplt().

Parameters of the UART are as follows:
Baudrate: 115200
Data bits: 8
Parity bit: None
Stop bits: 1
Asynchronous transmission (each device uses its internal clock for the transmission)

