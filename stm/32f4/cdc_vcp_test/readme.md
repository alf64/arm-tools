## **cdc_vcp_test**

The program acts like the USB CDC (Communication Device Class).
The following functionalities are being used here:
* USB stack configured as Device-only, CDC (Communication Device Class)
* EXTI0 interrupt configured to be triggered on-raising-edge (for button press handling)
* V-bus sensing enabled
* GPIO
* Delay

This project behavior upon loading is as follows:
1. Flash all the user available LEDS upon program start/reset.
2. Perform action on Blue Button press.
3. Perform action when USB connection is detected (device plugged in to PC).
4. Perform action on data reception over USB (which acts like COM port from OS point of view).
5. Perform action when USB connection is lost (device unplugged from PC).

To communicatate with the device:
* please use some kind of a terminal app on your PC, i.e. RealTerm.
* please install Virtual Com Port drivers. FTDI drivers should work.
The communication is in general handled by the USB layer stack, which means that com port parameters (baud, format, parity) does not play a role here,
however it's preferred to set them to 115200, 1 stop bit, no parity, 8 data bits as the program has been tested under these.

Connecting the board via user USB causes program to enable green user LED.
Disconnecting the board causes this LED disablement.

Pressing Blue Button on the board causes blue LED to be toggled and sends the message via COM port informing if the blue LED is in OFF or in ON state.
Sending data from PC over COM port causes program to capture it and re-send it back to PC with additional information on how much and what data has been received.
If there is too much data send from PC, program will try to inform about it by sending specific message via COM port, and then it will halt itself forever,
blinking the red user LED infinitely.

----- V-bus sensing info -----
From USB specification, USB device shall use VBUS sensing detection: 
When host connection is detected by the device, device connects pull-up resistor either to D+ or to D- data signal, allowing host to detect device presence on the bus.
There are two use cases:
* If USB device is bus-powered, VBUS sensing is NOT mandatory: USB is connected all the time when device is powered,
* If USB device is self-powered, VBUS sensing is mandatory.

----- Known problems -----
You should not use HAL_Delay() in CDC_Receive_FS(), as program will stuck forever waiting for the SysTick to occur. It seems like SysTick interrupt is unable to preempt the CDC_Receive_FS() interrupt/thread.
CDC_Control_FS() function needed CDC_GET_LINE_CODING case to be filled with appropriate code in order for the host to properly open the COM port. Without this code, there were occasional problems with COM port opening.
