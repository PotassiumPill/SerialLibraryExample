/*
 * common_chipname.cpp
 *
 * Created: 6/2/2023 2:26:22 PM
 *  Author: aaron
 */ 


#include "serial_comm_config.h"

#if (SERCOM_MCU_OPT == OPT_SERCOM_CHIPNAME)

#include "serial_common/hardware/common_chipname.h"


void SERCOMCHIPNAME::HelperFunction(void)
{
	
}

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