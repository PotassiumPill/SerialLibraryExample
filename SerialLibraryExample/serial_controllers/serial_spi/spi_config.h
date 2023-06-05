/* 
 * Name				:	spi_config.h
 * Created			:	10/13/2022 4:58:47 PM
 * Author			:	Aaron Reilman
 * Description		:	A SPI serial communication configuration file.
 */


#ifndef __SPI_CONFIG_H__
#define __SPI_CONFIG_H__

#include "serial_comm_config.h"

//DEFINE ENVIRONMENT HERE
#define DEFAULT_MOSI_BUFFER_SIZE	128
#define DEFAULT_MISO_BUFFER_SIZE	128

#ifndef SPI_MCU_OPT
	#if (SERCOM_MCU_OPT == OPT_SERCOM_SAMD21)
		#define SPI_MCU_OPT		OPT_SERCOM_SAMD21
	#else
		#define SPI_MCU_OPT		OPT_SERCOM_NONE
	#endif
#endif
//-----------------------

#if (SPI_MCU_OPT == OPT_SERCOM_SAMD21)
	#include "serial_spi/hardware/spi_samd21.h"
#else 
	#warning "SPI not defined for this MCU!"
#endif

#endif //__SPI_CONFIG_H__
