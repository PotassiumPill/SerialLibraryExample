/*
 * uart_chipname.h
 *
 * Created: 6/2/2023 2:40:08 PM
 *  Author: aaron
 */ 


#ifndef UART_CHIPNAME_H_
#define UART_CHIPNAME_H_

#include "chipname.h"
#include "serial_uart/uart_hal.h"

#include "serial_common/hardware/common_chipname.h"

namespace UARTCHIPNAME {
	
	enum ExtraParams : uint8_t {
		CLK_NUM,
		CLK_RATE,
		PAD_CONFIG
		};
	
	void HelperUART(void);
}




#endif /* UART_CHIPNAME_H_ */