/*
 * spi_chipname.h
 *
 * Created: 6/2/2023 2:31:45 PM
 *  Author: aaron
 */ 


#ifndef SPI_CHIPNAME_H_
#define SPI_CHIPNAME_H_

#include "chipname.h"
#include "serial_spi/spi_hal.h"

#include "serial_common/hardware/common_chipname.h"

namespace SPICHIPNAME {
	enum ExtraParams : uint8_t {
		CLK_NUM,
		CLK_RATE,
		PAD_CONFIG
		};
		
	void HelperSPI(void);
}





#endif /* SPI_CHIPNAME_H_ */