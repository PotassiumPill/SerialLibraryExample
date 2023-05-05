# Embedded %Serial Communicaton Library for C++

Library for serial communication in C++ using various APIs and hardware for use in Microchip Studio projects.

## Current APIs
1) SerialUART\n 
2) SerialUSB (CDC) using tinyUSB\n 
3) SerialSPI\n 

## Additional Utilities
1) GenericBuffer (a C++ ring buffer template)\n
2) Serial (integer to ASCII function and char buffer with advanced parsing features)

## Current Hardware
1) SAMD21 Series ARM Microcontroller

## How To Use
1) Setup project for desired hardware.\n 
2) In your C and C++ compilers, add ../serial_controllers to the include path.\n 
3) Open serial_comm_config.h, and define the SERCOM_MCU_OPT macro to your desired hardware (refer to serial_comm_options.h for all defined hardware).\n 
4) Define the SERCOM_MODULE_OPT as all desired protocols for your project (OPT_SERCOM_UART | OPT_SERCOM_SPI | OPT_SERCOM_USB defines all protocols).\n 
5) include serial_communication.h in your project to use serial protocols.\n 
6) Initialize hardware clocks.\n 
7) Initialize and implement all ISRs to ensure proper functionality!\n 

## Serial USB Code Setup Example
```
#include "serial_communication.h"
...
char usb_buf[USB_DEFAULT_FIFO_SIZE];			//call outside of function calls so variable stays active
SerialUSB::USBController usb_controller;
...
usb_controller.Init(usb_buf, sizeof(usb_buf));		//call after clock setup (this will set clock to 48 MHz)
usb_controller.Task();						//must be called often, preferrably inside of any smaller loop or blocking function
...
void USB_Handler()						//use the proper ISR Handler according to your hardware!
{
	usb_controller.ISR();	
}
```

*NOTE: Call USBController.Task() in every loop in your main program so USB communication works. USBController.Task() can also be used to flash received data onto serial terminal by passing true into Task parameters.*

### USB Descriptors for compatibility in host applications
* VID: 0xCafe
* PID: 0x4001
* BCD: 0x0200


## Serial UART Code Setup Example
```
#include "serial_communication.h"
...
char rx_buf[DEFAULT_RX_BUFFER_SIZE];
char tx_buf[DEFAULT_TX_BUFFER_SIZE];
SerialUART::UARTController uart_controller;
...
UARTHAL::Peripheral uart_peripheral;
UARTHAL::GetPeripheralDefaults(&uart_peripheral);								//Can also call hardware specific peripheral initalizing functions or manually initialize peripheral 
uart_controller.Init(&uart_peripheral, tx_buf, sizeof(tx_buf), rx_buf, sizeof(rx_buf));		//call after clock setup
uart_controller.EnableErrorIRQ();											//optional IRQ setups
...
void SERCOM0_Handler()													//use the proper ISR Handler according to your hardware!
{
	uart_controller.ISR();
	//uart_controller.EchoRx();											//call EchoRx() to debug by flashing all received characters onto terminal, exclude for actual use
}
```

## Serial SPI Code Setup Example
```
#include "serial_communication.h"
...
char miso_buf[DEFAULT_MISO_BUFFER_SIZE];
char mosi_buf[DEFAULT_MOSI_BUFFER_SIZE];
SerialSPI::SPIController spi_controller;
...
SPIHAL::Peripheral spi_peripheral;
SPIHAL::GetPeripheralDefaults(&spi_peripheral);										//Can also call hardware specific peripheral initalizing functions or manually initialize peripheral 
spi_controller.Init(&spi_peripheral, mosi_buf, sizeof(mosi_buf), miso_buf, sizeof(miso_buf));		//call after clock setup
spi_controller.EnableErrorIRQ();												//optional IRQ setups
...
void SERCOM4_Handler()														//use the proper ISR Handler according to your hardware!
{
	spi_controller.ISR();
	//spi_controller.EchoMISO();												//call EchoMISO() to debug by flashing all received characters onto terminal, exclude for actual use
}
```

*NOTE: you will primarily call SPIController.SPIHostProcedure() when transmitting opcodes to SPI client, refer to doxygen docs to learn more about its usage.*

