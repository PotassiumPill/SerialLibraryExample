/*
 * Name				:	common_none.cpp
 * Created			:	1/23/2023 12:32:02 PM
 * Author			:	Aaron Reilman
 * Description		:	Blank template for hardware implementations of serial communication interfaces.
 */

#include "serial_comm_config.h"

#if (SERCOM_MCU_OPT == OPT_SERCOM_NONE)

#include "serial_common/common_hal.h"

void SERCOMHAL::ConfigPin(SERCOMHAL::Pinout pin, bool output, bool multiplexed, SERCOMHAL::PullResistor pull)
{
	(void)pin;
	(void)output;
	(void)multiplexed;
	(void)pull;
}

bool SERCOMHAL::GetPinState(SERCOMHAL::Pinout pin)
{
	(void)pin;
	return false;
}

void SERCOMHAL::OutputHigh(SERCOMHAL::Pinout output_pin)
{
	(void)output_pin;
}

void SERCOMHAL::OutputLow(SERCOMHAL::Pinout output_pin)
{
	(void)output_pin;
}
	
#endif
