/* 
 * Name			:	common_samd21.cpp
 * Created		:	1/18/2023 4:40:56 PM
 * Author		:	Aaron Reilman
 * Description	:	Common generic hardware functionality for SAMD21.
 */


#include "serial_common/common_hal.h"

#if (SERCOM_MCU_OPT == OPT_SERCOM_SAMD21)

Sercom * SERCOMSAMD21::GetSercom(SERCOMHAL::SercomID sercom_id)
{
	Sercom *sercom_ptr;
	switch(sercom_id) {
		case  SERCOMSAMD21::SercomID::Sercom0:
			sercom_ptr = SERCOM0;
			break;
		case  SERCOMSAMD21::SercomID::Sercom1:
			sercom_ptr = SERCOM1;
			break;
		case  SERCOMSAMD21::SercomID::Sercom2:
			sercom_ptr = SERCOM2;
			break;
		case  SERCOMSAMD21::SercomID::Sercom3:
			sercom_ptr = SERCOM3;
			break;
		case  SERCOMSAMD21::SercomID::Sercom4:
			sercom_ptr = SERCOM4;
			break;
		case  SERCOMSAMD21::SercomID::Sercom5:
			sercom_ptr = SERCOM5;
			break;
		default:
			sercom_ptr = nullptr;
			break;
	}
	return sercom_ptr;
}

void SERCOMSAMD21::EnableSercomClock(SERCOMHAL::SercomID sercom_id, SERCOMSAMD21::GenericClock gen_clk_num, uint16_t clock_divisor, bool run_standby)
{
	GCLK->GENCTRL.reg = GCLK_GENCTRL_ID(gen_clk_num) | GCLK_GENCTRL_SRC(GCLK_SOURCE_OSC8M) | GCLK_GENCTRL_IDC;
	GCLK->GENCTRL.bit.RUNSTDBY = run_standby;
	GCLK->GENCTRL.bit.GENEN = 0x1;
	GCLK->GENDIV.reg = GCLK_GENDIV_DIV(clock_divisor) | GCLK_GENDIV_ID(gen_clk_num);
	while(GCLK->STATUS.bit.SYNCBUSY);
	switch(sercom_id) {
		case SERCOMSAMD21::SercomID::Sercom0:
			PM->APBCMASK.bit.SERCOM0_ = 0x1u;
			GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN(gen_clk_num) | GCLK_CLKCTRL_ID_SERCOM0_CORE;
			NVIC_EnableIRQ(SERCOM0_IRQn);
			break;
		case SERCOMSAMD21::SercomID::Sercom1:
			PM->APBCMASK.bit.SERCOM1_ = 0x1u;
			GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN(gen_clk_num) | GCLK_CLKCTRL_ID_SERCOM1_CORE;
			NVIC_EnableIRQ(SERCOM1_IRQn);
			break;
		case SERCOMSAMD21::SercomID::Sercom2:
			PM->APBCMASK.bit.SERCOM2_ = 0x1u;
			GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN(gen_clk_num) | GCLK_CLKCTRL_ID_SERCOM2_CORE;
			NVIC_EnableIRQ(SERCOM2_IRQn);
			break;
		case SERCOMSAMD21::SercomID::Sercom3:
			PM->APBCMASK.bit.SERCOM3_ = 0x1u;
			GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN(gen_clk_num) | GCLK_CLKCTRL_ID_SERCOM3_CORE;
			NVIC_EnableIRQ(SERCOM3_IRQn);
			break;
		case SERCOMSAMD21::SercomID::Sercom4:
			PM->APBCMASK.bit.SERCOM4_ = 0x1u;
			GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN(gen_clk_num) | GCLK_CLKCTRL_ID_SERCOM4_CORE;
			NVIC_EnableIRQ(SERCOM4_IRQn);
			break;
		case SERCOMSAMD21::SercomID::Sercom5:
			PM->APBCMASK.bit.SERCOM5_ = 0x1u;
			GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN(gen_clk_num) | GCLK_CLKCTRL_ID_SERCOM5_CORE;
			NVIC_EnableIRQ(SERCOM5_IRQn);
			break;
		default:
			//do nothing
			break;
	}
	while(GCLK->STATUS.bit.SYNCBUSY);  // Wait for write to complete
}

void SERCOMHAL::ConfigPin(SERCOMHAL::Pinout pin, bool output, bool multiplexed, SERCOMHAL::PullResistor pull)
{
	PORT->Group[pin.port].PINCFG[pin.pin].reg = PORT_PINCFG_DRVSTR | PORT_PINCFG_INEN;
	if(output)
		PORT->Group[pin.port].DIRSET.reg = 0x1u << pin.pin;
	else
	{
		PORT->Group[pin.port].DIRCLR.reg = 0x1u << pin.pin;
		if(pull != PullResistor::NoPull)
		{
			PORT->Group[pin.port].PINCFG[pin.pin].bit.PULLEN = 0x1;
			if(pull == PullResistor::PinPullUp)
				PORT->Group[pin.port].OUTSET.reg = 0x1u << pin.pin;
		}
	}
	if(multiplexed)
	{
		PORT->Group[pin.port].PINCFG[pin.pin].bit.PMUXEN = 0x1;
		if(pin.pin % 2 == 0)
			PORT->Group[pin.port].PMUX[pin.pin / 2u].bit.PMUXE = pin.peripheral_function;
		else
			PORT->Group[pin.port].PMUX[pin.pin / 2u].bit.PMUXO = pin.peripheral_function;
	}
}

bool SERCOMHAL::GetPinState(SERCOMHAL::Pinout pin)
{
	return ((PORT->Group[pin.port].IN.reg >> pin.pin) & 0x1);
}

void SERCOMHAL::OutputHigh(SERCOMHAL::Pinout output_pin)
{
	PORT->Group[output_pin.port].OUTSET.reg = 0x1 << output_pin.pin;
}

void SERCOMHAL::OutputLow(SERCOMHAL::Pinout output_pin)
{
	PORT->Group[output_pin.port].OUTCLR.reg = 0x1 << output_pin.pin;
}

#endif