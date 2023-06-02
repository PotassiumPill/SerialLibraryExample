/* 
 * Name				:	uart_config.h
 * Created			:	8/10/2022 12:39:16 PM
 * Author			:	Aaron Reilman
 * Description		:	A UART serial communication configuration file.
 */


#ifndef __UART_CONFIG_H__
#define __UART_CONFIG_H__

#include "serial_comm_config.h"

//DEFINE ENVIRONMENT HERE
#define DEFAULT_TX_BUFFER_SIZE	128
#define DEFAULT_RX_BUFFER_SIZE	128

#ifndef UART_MCU_OPT
	#if (SERCOM_MCU_OPT == OPT_SERCOM_SAMD21)
		#define UART_MCU_OPT		OPT_SERCOM_SAMD21
	#else
		#define UART_MCU_OPT		OPT_SERCOM_NONE
	#endif
#endif
//-----------------------

#if (UART_MCU_OPT == OPT_SERCOM_SAMD21)
	#define NUM_EXTRA_UART_PARAMS 6
	#include "serial_uart/hardware/uart_samd21.h"
#else
	#define NUM_EXTRA_UART_PARAMS 1
	#include "serial_uart/uart_hal.h"
#endif

#endif //__UART_CONFIG_H__
