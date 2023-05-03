/*
 * Name			:	serial_comm_config.h
 * Created		:	1/20/2023 10:37:50 AM
 * Author		:	Aaron Reilman
 * Description	:	Config file for serial communication library. Configure hardware and modules to use here.
 */ 

#ifndef __SERIAL_COMM_CONFIG_H__
#define __SERIAL_COMM_CONFIG_H__

#include "serial_comm_options.h"

//define here
#ifndef SERCOM_MCU_OPT
#define SERCOM_MCU_OPT		OPT_SERCOM_SAMD21
#endif

#ifndef SERCOM_MODULE_OPT
#define SERCOM_MODULE_OPT	(OPT_SERCOM_UART | OPT_SERCOM_SPI | OPT_SERCOM_USB)
#endif
//-----------------------

#endif //__SERIAL_COMM_CONFIG_H__
