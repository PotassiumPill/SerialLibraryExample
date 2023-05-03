/*
 * Name			:	serial_communication.h
 * Created		:	1/20/2023 12:05:36 PM
 * Author		:	Aaron Reilman
 * Description	:	Header for serial communication library. 
 */ 

#ifndef __SERIAL_COMMUNICATION_H__
#define __SERIAL_COMMUNICATION_H__

#include "serial_comm_config.h"

#if (SERCOM_MCU_OPT == OPT_SERCOM_NONE)
	#warning "No MCU defined. Please define an MCU."
#endif
#if (SERCOM_MODULE_OPT & OPT_SERCOM_UART)
	#include "serial_uart/serial_uart.h"
#endif
#if (SERCOM_MODULE_OPT & OPT_SERCOM_SPI)
	#include "serial_spi/serial_spi.h"
#endif
#if (SERCOM_MODULE_OPT & OPT_SERCOM_USB)
	#include "serial_usb/serial_usb.h"
#endif

#endif //__SERIAL_COMMUNICATION_H__
