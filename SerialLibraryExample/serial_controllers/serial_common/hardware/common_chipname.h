/*
 * common_chipname.h
 *
 * Created: 6/2/2023 2:23:25 PM
 *  Author: aaron
 */ 


#ifndef COMMON_CHIPNAME_H_
#define COMMON_CHIPNAME_H_

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
		PF_A, PF_B, PF_C, PF_D, PF_E, PF_F, PF_G, PF_H
	};
	/*!
	 * \brief An enum type for a pin's port on SAMD21.
	 */	
	enum Port : uint8_t {
		PORT_A, PORT_B
	};
	
	void HelperFunction(void);
}





#endif /* COMMON_CHIPNAME_H_ */