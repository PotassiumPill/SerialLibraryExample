# SerialLibraryExample
Example project to showcase custom C++ serial communication library, configured for an example on the SAMD21 using Microchip Studio. 

## Features
1) [Serial UART](/SerialLibraryExample/serial_controllers/serial_uart) library.
2) [Serial SPI](/SerialLibraryExample/serial_controllers/serial_spi) library.
3) [Serial USB](/SerialLibraryExample/serial_controllers/serial_usb) library using [tinyUSB](https://github.com/hathach/tinyusb/tree/master/src) stack. 
4) Python scripts for generating finite state machine code, which is very useful for streamlining code and maximizing functionality. Refer to [README](/state_machine_generator/README.txt) for more information on how to use.
5) Generic ring buffer template and higher level serial buffer class which can be used directly for data storage or other serial functionality. 
6) Refer to [README](/SerialLibraryExample/serial_controllers/README.txt) for more information about configuring and using the library.

## Running the Example Project
1) Refer to the necessary datasheets to set up the SAMD21 and your debugger of choice. 
2) The regular example uses a serial CDC USB connection.
3) Open a serial terminal with 115200 baud rate and ensure it is connected to the right port.
4) If properly connected you should see text indicating the USB is plugged in.
5) You can now type the following commands:
   - `hello world`  (response `World: hello!`)
   - `integer_#`    (replace # with ASCII digits)
   - `square_#!`    (replace # with ASCII digits)
   - `off`          (moves into an "off" state)
   - `echo`         (echos all data sent to chip on terminal but ceases regular function in example)
6) In the off state, command `on` can be sent to go back into "on" state.

## Adding New Hardware
1) Create a new c++ project in Microchip Studio configured for your new chip.
2) Refer to the `main.cpp` included headers and note the name of the header file it uses for your chip. 
3) Add the entire [serial_controllers](/SerialLibraryExample/serial_controllers) directory to your project and add it to the C and C++ include paths. 
3) Navigate to [serial_comm_options.h](/SerialLibraryExample/serial_controllers/serial_comm_options.h).
2) Add a new macro defining your new chip, ie `#define OPT_SERCOM_CHIP_NAME	2`. Make sure to give it a unique number.

### Add Functionality to USB Stack
1) Navigate to [tusb_config.h](/SerialLibraryExample/serial_controllers/tusb_config.h).
2) Find the block of code which looks similar to this:
```
#ifndef CFG_TUSB_MCU
#include "serial_comm_config.h"
	#if (SERCOM_MCU_OPT == OPT_SERCOM_SAMD21)
		#define CFG_TUSB_MCU		OPT_MCU_SAMD21
	#else
		#define CFG_TUSB_MCU		OPT_MCU_NONE
	#endif	
#endif
```
3) Before the line `#else` and after the last line which looks similar to `#define CFG_TUSB_MCU        OPT_MCU_DEFINED_CHIP_NAME`, add the following code for your new chip:
```
#elif (SERCOM_MCU_OPT == OPT_SERCOM_CHIP_NAME)
	#define CFG_TUSB_MCU        OPT_MCU_CHIP_NAME
```
where `OPT_SERCOM_CHIP_NAME` is the macro you defined in [serial_comm_options.h](/SerialLibraryExample/serial_controllers/serial_comm_options.h) and `OPT_MCU_CHIP_NAME` is the tinyUSB macro definition for your chip as found in [tusb_option.h](/SerialLibraryExample/serial_controllers/tusb_option.h). Refer to [tinyUSB repository](https://github.com/hathach/tinyusb/tree/master/src) or skip this step and do not use USB functionality for this chip.
4) Navigate to [serial_usb.cpp](/SerialLibraryExample/serial_controllers/serial_usb/serial_usb.cpp) and find the code block which looks similar to this:
```
#if CFG_TUSB_MCU == OPT_MCU_SAMD21
#include "sam.h"

void SerialUSB::USBFeedIOClocks(void)
{
	...
}

void SerialUSB::ResetUSB(void)
{
	...
}
#else
void SerialUSB::USBFeedIOClocks(void) {}
void SerialUSB::ResetUSB(void) {}
#endif
```
5) Before the line `#else` and after the `}`, add the following code for your new chip:
```
#elif CFG_TUSB_MCU == OPT_MCU_CHIP_NAME
#include "chip.h"

void SerialUSB::USBFeedIOClocks(void)
{
}

void SerialUSB::ResetUSB(void)
{
}
```
where `"chip.h"` is the header file for your chip as noted earlier.

6) Implement code to initialize the USB clock and the reset the USB registers in the respective functions.

### Add General Functionality for SPI and UART Stacks
1) SPI and UART stacks require some common functions to run. 
2) Navigate to the [hardware](/SerialLibraryExample/serial_controllers/serial_common/hardware) directory under the [serial_common](/SerialLibraryExample/serial_controllers/serial_common) directory.
3) Add a new header to this directory for your chip, ie `common_chipname.h`.
4) Define a namespace for your hardware specific functions, any hardware specific enums, and an enum of type uint8_t `SercomID`, if applicable. For example:
```
#ifndef __COMMON_CHIPNAME_H__
#define __COMMON_CHIPNAME_H__

#include "chip.h"

namespace SERCOMCHIPNAME
{
	enum SercomID : uint8_t {
		Sercom0,
		Sercom1,
		Sercom2,
		Sercom3,
		Sercom4,
		Sercom5
	};
	void ExampleFunction(void);
	void AnotherExampleFunction(SERCOMHAL::SercomID sercom_id);
}; 

#endif //__COMMON_CHIPNAME_H__
```
5) Add a new cpp file to this directory for your chip, ie `common_chipname.cpp`.
6) At the top, include this code block:
```
#include "serial_common/common_hal.h"

#if (SERCOM_MCU_OPT == OPT_SERCOM_CHIP_NAME)
```
7) Here, you will define all functions in [common_hal.h](/SerialLibraryExample/serial_controllers/serial_common/common_hal.h) and any functions you defined in `common_chipname.h`.
8) Make sure that the last line in this file is `#endif`!
9) Navigate to [common_hal.h](/SerialLibraryExample/serial_controllers/serial_common/common_hal.h) and find the code block which looks similar to this:
```
#if (SERCOM_MCU_OPT == OPT_SERCOM_SAMD21)
#include "serial_common/hardware/common_samd21.h"
#endif
```
10) Before the last `#endif` line and before the last `#include` line, add the following code for your new chip:
```
#elif (SERCOM_MCU_OPT == OPT_SERCOM_CHIP_NAME)
#include "serial_common/hardware/common_chipname.h"
```

### Add Functionality to UART Stack
1) Navigate to the [hardware](/SerialLibraryExample/serial_controllers/serial_uart/hardware) directory under the [serial_uart](/SerialLibraryExample/serial_controllers/serial_uart) directory.
2) Add a new header to this directory for your chip, ie `uart_chipname.h`.
3) Define a namespace for your hardware specific functions, any hardware specific enums, and enums of type uint8_t `SampleAdjustment` and `PadConfig`. For example:
```
#ifndef __UART_CHIPNAME_H__
#define __UART_CHIPNAME_H__

#include "chip.h"

namespace UARTCHIPNAME
{
	/*!
	 * \brief An enum type for over-sampling rate and sampling adjustment
	 */
	enum SampleAdjustment : uint8_t {
		Over16x_7_8_9,
		Over16x_9_10_11,
		Over16x_11_12_13,
		Over16x_13_14_15,
		Over8x_3_4_5,
		Over8x_4_5_6,
		Over8x_5_6_7,
		Over8x_6_7_8
	};
	enum PadConfig : uint8_t {
		Tx0_Rx1,
		Tx0_Rx2,
		Tx0_Rx3,
		Tx2_Rx0,
		Tx2_Rx1,
		Tx2_Rx3
	};
	
	void GetPeripheralDefaults(UARTHAL::Peripheral * peripheral, SERCOMHAL::SercomID sercom_id = SERCOMCHIPNAME::SercomID::Sercom0);
	void ExampleFunction(void);
}

#endif //__UART_CHIPNAME_H__
```
4) Add a new cpp file to this directory for your chip, ie `uart_chipname.cpp`.
5) At the top, include this code block:
```
#include "serial_uart/uart_hal.h"

#if (UART_MCU_OPT == OPT_UART_CHIP_NAME)
```
6) Here, you will define all functions in [uart_hal.h](/SerialLibraryExample/serial_controllers/serial_uart/uart_hal.h) and any functions you defined in `uart_chipname.h`.
7) Make sure that the last line in this file is `#endif`!
8) Navigate to [uart_config.h](/SerialLibraryExample/serial_controllers/serial_uart/uart_config.h).
9) Find the code block which looks similar to this:
```
#ifndef UART_MCU_OPT
	#if (SERCOM_MCU_OPT == OPT_SERCOM_SAMD21)
		#define UART_MCU_OPT		OPT_SERCOM_SAMD21
	#else
		#define UART_MCU_OPT		OPT_SERCOM_NONE
	#endif
#endif
```
10) Before the `#else` line and after the `#define` line before it, add the following code for your new chip:
```
#elif (SERCOM_MCU_OPT == OPT_SERCOM_CHIP_NAME)
	#define UART_MCU_OPT		OPT_SERCOM_CHIP_NAME
```
11) Navigate to [uart_hal.h](/SerialLibraryExample/serial_controllers/serial_uart/uart_hal.h) and find the code block which looks similar to this:
```
#if (UART_MCU_OPT == OPT_SERCOM_SAMD21)
#include "serial_uart/hardware/uart_samd21.h"
#endif
```
12) Before the last `#endif` line and before the last `#include` line, add the following code for your new chip:
```
#elif (UART_MCU_OPT == OPT_SERCOM_CHIP_NAME)
#include "serial_uart/hardware/uart_chipname.h"
```
