# SerialLibraryExample
Example project to showcase custom C++ serial communication library, configured for an example on the SAMD21 using Microchip Studio. 

## Features
1) [Serial UART](/SerialLibraryExample/serial_controllers/serial_uart) library.
2) [Serial SPI](/SerialLibraryExample/serial_controllers/serial_spi) library.
3) [Serial USB](/SerialLibraryExample/serial_controllers/serial_usb) library using [tinyUSB](https://github.com/hathach/tinyusb/tree/master/src) stack. 
4) Python scripts for generating finite state machine code, which is very useful for streamlining code and maximizing functionality. Refer to [README](/state_machine_generator/README.md) for more information on how to use.
5) Generic ring buffer template and higher level serial buffer class which can be used directly for data storage or other serial functionality. 
6) Refer to [README](/SerialLibraryExample/serial_controllers/README.md) for more information about configuring and using the library.
7) Refer to [documentation page](https://potassiumpill.github.io/SerialLibraryExample/) for documentation on serial library.

## Running the Example Project (SAMD21G18A)
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
7) To use UART functionality instead of USB functionality, navigate to [example_state_machine.cpp](/SerialLibraryExample/example_state_machine.cpp) and uncomment the line: `//#define USING_UART`
   - On the SAMD21 Feather M0, you should connect to the TX and RX pins. 
   - The default baud rate is 115200. To change this, navigate to [example_state_machine.h](/SerialLibraryExample/example_state_machine.h)
 and change the line `#define BAUD_RATE		115200` to the desired baud rate. 
10) To try this project out a different chip, you will need to create a new C++ project in Microchip Studio based on your chip, add all files under the [SerialLibraryExample](/SerialLibraryExample) subdirectory to it, then refer below to learn how add new chips to the library. You will also need to navigate to [example_state_machine.cpp](/SerialLibraryExample/example_state_machine.cpp) and change the code in the `ExampleStateMachine::InitializingStateAction(void)` function definition between the `Util::enterCriticalSection();` and `Util::exitCriticalSection();` calls to configure the clocks on your chip.

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

4) Navigate to the [hardware](/SerialLibraryExample/serial_controllers/serial_usb/hardware) directory under the [serial_usb](/SerialLibraryExample/serial_controllers/serial_usb) directory.
5) Add a new cpp file to this directory for your chip, ie `usb_chipname.cpp`.
6) At the top, include this code block:
```
#include "serial_usb/serial_usb.h"

#if (CFG_TUSB_MCU == OPT_MCU_CHIP_NAME)
#include "chip.h"
```
where `"chip.h"` is the header file for your chip and `OPT_MCU_CHIP_NAME' is the tinyUSB macro definition for your chip, as noted earlier.

7) Here, you will define all functions in [serial_usb.h](/SerialLibraryExample/serial_controllers/serial_usb/serial_usb.h), which includes just `void SerialUSB::USBFeedIOClocks(void)` and `void SerialUSB::ResetUSB(void)`.
8) Make sure that the last line in this file is `#endif`!

### Add General Functionality for SPI and UART Stacks
1) SPI and UART stacks require some common functions to run. 
2) Navigate to the [hardware](/SerialLibraryExample/serial_controllers/serial_common/hardware) directory under the [serial_common](/SerialLibraryExample/serial_controllers/serial_common) directory.
3) Add a new header to this directory for your chip, ie `common_chipname.h`.
4) Define a namespace for your hardware specific functions, any hardware specific enums, and enums of type uint8_t `SercomID`, `PinPort`, and `PeripheralFunction`, if applicable. For example:
```
#ifndef __COMMON_CHIPNAME_H__
#define __COMMON_CHIPNAME_H__

#include "serial_common/common_hal.h"

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
	enum PeripheralFunction : uint8_t {
		PF_A, PF_B, PF_C
	};
	enum PinPort : uint8_t {
		PORT_A, PORT_B
	};
	void ExampleFunction(void);
	void AnotherExampleFunction(SERCOMHAL::SercomID sercom_id);
}; 

#endif //__COMMON_CHIPNAME_H__
```
5) Add a new cpp file to this directory for your chip, ie `common_chipname.cpp`.
6) At the top, include this code block:
```
#include "serial_comm_config.h"

#if (SERCOM_MCU_OPT == OPT_SERCOM_CHIP_NAME)

#include "serial_common/hardware/common_chipname.h"
```
7) Here, you will define all functions in [common_hal.h](/SerialLibraryExample/serial_controllers/serial_common/common_hal.h) and any functions you defined in `common_chipname.h`.
8) Make sure that the last line in this file is `#endif`!

### Add Functionality to UART Stack
1) Navigate to the [hardware](/SerialLibraryExample/serial_controllers/serial_uart/hardware) directory under the [serial_uart](/SerialLibraryExample/serial_controllers/serial_uart) directory.
2) Add a new header to this directory for your chip, ie `uart_chipname.h`.
3) Define a namespace for your hardware specific functions, any hardware specific enums, and an enum for naming the extra parameters. For example:
```
#ifndef __UART_CHIPNAME_H__
#define __UART_CHIPNAME_H__

#include "chip.h"
#include "serial_uart/uart_hal.h"

#include "serial_common/hardware/common_chipname.h"

namespace UARTCHIPNAME
{
	enum ExtraParameters : uint8_t {
		GEN_CLK,
		CLK_DIVISOR,
		PAD_CONFIG,
		SAMPLE_ADJUSTMENT
	};
	void ExampleGetPeripheralDefaults(UARTHAL::Peripheral * peripheral, SERCOMHAL::SercomID sercom_id = SERCOMCHIPNAME::SercomID::Sercom0);
	void AnotherExampleFunction(void);
}

#endif //__UART_CHIPNAME_H__
```
4) Add a new cpp file to this directory for your chip, ie `uart_chipname.cpp`.
5) At the top, include this code block:
```
#include "serial_uart/uart_config.h"

#if (UART_MCU_OPT == OPT_SERCOM_CHIP_NAME)

#include "serial_uart/hardware/uart_chipname.h"
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
11) Next, find the code block which looks similar to this:
```
#if (UART_MCU_OPT == OPT_SERCOM_SAMD21)
	#define NUM_EXTRA_UART_PARAMS 6
	#include "serial_uart/hardware/uart_samd21.h"
#endif
```
12) Before the `#endif` line and after the last `#include` statement, add the following code for your new chip:
```
#elif (UART_MCU_OPT == OPT_SERCOM_CHIPNAME)
	#define NUM_EXTRA_UART_PARAMS #
	#include "serial_uart/hardware/uart_chipname.h"
```
where # is the number of extra parameters you defined/need for your new hardware (this should line up with the number of elements defined in your ExtraParameters enum).

### Add Functionality to SPI Stack
1) Navigate to the [hardware](/SerialLibraryExample/serial_controllers/serial_spi/hardware) directory under the [serial_spi](/SerialLibraryExample/serial_controllers/serial_spi) directory.
2) Add a new header to this directory for your chip, ie `spi_chipname.h`.
3) Define a namespace for your hardware specific functions, any hardware specific enums, and an enum for naming the extra parameters. For example:
```
#ifndef __SPI_CHIPNAME_H__
#define __SPI_CHIPNAME_H__

#include "chip.h"
#include "serial_spi/spi_hal.h"

#include "serial_common/hardware/common_chipname.h"

namespace SPICHIPNAME
{
	enum ExtraParameters : uint8_t {
		GEN_CLK,
		CLK_DIVISOR,
		PAD_CONFIG
	};
	void ExampleGetPeripheralDefaults(SPIHAL::Peripheral * peripheral, SERCOMHAL::SercomID sercom_id = SERCOMCHIPNAME::SercomID::Sercom3);
	void AnotherExampleFunction(void);
}

#endif //__SPI_CHIPNAME_H__
```
4) Add a new cpp file to this directory for your chip, ie `spi_chipname.cpp`.
5) At the top, include this code block:
```
#include "serial_spi/spi_config.h"

#if (SPI_MCU_OPT == OPT_SERCOM_CHIP_NAME)

#include "serial_spi/hardware/spi_chipname.h"
```
6) Here, you will define all functions in [spi_hal.h](/SerialLibraryExample/serial_controllers/serial_spi/spi_hal.h) and any functions you defined in `spi_chipname.h`.
7) Make sure that the last line in this file is `#endif`!
8) Navigate to [spi_config.h](/SerialLibraryExample/serial_controllers/serial_spi/spi_config.h).
9) Find the code block which looks similar to this:
```
#ifndef SPI_MCU_OPT
	#if (SERCOM_MCU_OPT == OPT_SERCOM_SAMD21)
		#define SPI_MCU_OPT		OPT_SERCOM_SAMD21
	#else
		#define SPI_MCU_OPT		OPT_SERCOM_NONE
	#endif
#endif
```
10) Before the `#else` line and after the `#define` line before it, add the following code for your new chip:
```
#elif (SERCOM_MCU_OPT == OPT_SERCOM_CHIP_NAME)
	#define SPI_MCU_OPT		OPT_SERCOM_CHIP_NAME
```
11) Next, find the code block which looks similar to this:
```
#if (SPI_MCU_OPT == OPT_SERCOM_SAMD21)
	#define NUM_EXTRA_SPI_PARAMS 5
	#include "serial_spi/hardware/spi_samd21.h"
#endif
```
12) Before the `#endif` line and after the last `#include` statement, add the following code for your new chip: 
```
#elif (SPI_MCU_OPT == OPT_SERCOM_CHIPNAME)
	#define NUM_EXTRA_UART_PARAMS #
	#include "serial_spi/hardware/spi_chipname.h"
```
where # is the number of extra parameters you defined/need for your new hardware (this should line up with the number of elements defined in your ExtraParameters enum).
