## **uart_test**

A project that shows how to perform simple operations on UART with interrupts.
HAL_UART, HAL_GPIO, HAL_Delay functionalities are used here.

Upon loading, program:
 - toggles LED6 every 1 second and via UART sends message "LED ON" or "LED OFF" depending on LED6 state
 - toggles LED4 each time the message via UART has been send (TxCallback)
 - toggles LED3 each time 4 bytes of data has been received via UART

Parameters of the UART are as follows:
Baudrate: 115200
Data bits: 8
Parity bit: None
Stop bits: 1
Asynchronous transmission (each device uses its internal clock for the transmission)

Worth noting:
In STM32Cube IDE Device Configuration Tool, when you have to set USART parameters, "Word Length" is a little confusing.
It says it "includes Parity".
Yes it includes it, but only if Parity is not None.
I.e. if you set 8 bits with Parity None, you have 8 data bits (and no parity bit).
I.e. if you set 8 bits with Parity other than None, you have 7 data bits (+ Parity Bit).
