/*
 * Name				:	usb_none.cpp
 * Created			:	5/5/2023 2:01:52 PM
 * Author			:	Aaron Reilman
 * Description		:	Blank template for hardware implementations of clock drivers for USB. 
 */ 


#include "serial_usb/serial_usb.h"

#if CFG_TUSB_MCU == OPT_MCU_NONE

void SerialUSB::USBFeedIOClocks(void) {}
void SerialUSB::ResetUSB(void) {}
	
#endif