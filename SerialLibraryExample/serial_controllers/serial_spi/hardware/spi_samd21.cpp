/* 
 * Name				:	spi_samd21.cpp
 * Created			:	10/13/2022 5:39:06 PM
 * Author			:	Aaron Reilman
 * Description		:	A SPI serial communication low level driver for samd21.
 */


#include "serial_spi/spi_config.h"

#if (SPI_MCU_OPT == OPT_SERCOM_SAMD21)

#include "serial_spi/hardware/spi_samd21.h"

void SPISAMD21::GetPeripheralDefaults(SPIHAL::Peripheral * peripheral, SERCOMHAL::SercomID sercom_id)
{
	peripheral->extra_spi_params[ExtraParams::PAD_CONFIG] = PadConfig::DO2_DI0_SCK3_CSS1;
	peripheral->baud_value =  50000;
	peripheral->clock_mode =  SPIHAL::ClockMode::Mode0;
	peripheral->endianess = SPIHAL::Endian::MSB;
	peripheral->sercom_id = sercom_id;
	peripheral->extra_spi_params[ExtraParams::GEN_CLK] = SERCOMSAMD21::GEN_CLK_2;
	peripheral->extra_spi_params[ExtraParams::GEN_CLK_DIVISOR_BITS_9_16] = 0;
	peripheral->extra_spi_params[ExtraParams::GEN_CLK_DIVISOR_BITS_1_8] = 0;
	peripheral->extra_spi_params[ExtraParams::GEN_CLK_RUN_STANDY] = false;
	switch(sercom_id)
	{
		case SERCOMSAMD21::SercomID::Sercom0:
			peripheral->mosi_pin = (SERCOMHAL::Pinout){SERCOMSAMD21::PeripheralFunction::PF_D, SERCOMSAMD21::Port::PORT_A, 6};
			peripheral->miso_pin = (SERCOMHAL::Pinout){SERCOMSAMD21::PeripheralFunction::PF_D, SERCOMSAMD21::Port::PORT_A, 4};
			peripheral->sck_pin = (SERCOMHAL::Pinout){SERCOMSAMD21::PeripheralFunction::PF_D, SERCOMSAMD21::Port::PORT_A, 7};
			peripheral->ssl_pin = (SERCOMHAL::Pinout){SERCOMSAMD21::PeripheralFunction::PF_D, SERCOMSAMD21::Port::PORT_A, 5};
			break;
		case SERCOMSAMD21::SercomID::Sercom1:
			peripheral->mosi_pin = (SERCOMHAL::Pinout){SERCOMSAMD21::PeripheralFunction::PF_C, SERCOMSAMD21::Port::PORT_A, 18};
			peripheral->miso_pin = (SERCOMHAL::Pinout){SERCOMSAMD21::PeripheralFunction::PF_C, SERCOMSAMD21::Port::PORT_A, 16};
			peripheral->sck_pin = (SERCOMHAL::Pinout){SERCOMSAMD21::PeripheralFunction::PF_C, SERCOMSAMD21::Port::PORT_A, 19};
			peripheral->ssl_pin = (SERCOMHAL::Pinout){SERCOMSAMD21::PeripheralFunction::PF_C, SERCOMSAMD21::Port::PORT_A, 17};
			break;	
		case SERCOMSAMD21::SercomID::Sercom2:
			peripheral->mosi_pin = (SERCOMHAL::Pinout){SERCOMSAMD21::PeripheralFunction::PF_D, SERCOMSAMD21::Port::PORT_A, 10};
			peripheral->miso_pin = (SERCOMHAL::Pinout){SERCOMSAMD21::PeripheralFunction::PF_D, SERCOMSAMD21::Port::PORT_A, 8};
			peripheral->sck_pin = (SERCOMHAL::Pinout){SERCOMSAMD21::PeripheralFunction::PF_D, SERCOMSAMD21::Port::PORT_A, 11};
			peripheral->ssl_pin = (SERCOMHAL::Pinout){SERCOMSAMD21::PeripheralFunction::PF_D, SERCOMSAMD21::Port::PORT_A, 9};
			break;
		case SERCOMSAMD21::SercomID::Sercom3:
			peripheral->mosi_pin = (SERCOMHAL::Pinout){SERCOMSAMD21::PeripheralFunction::PF_D, SERCOMSAMD21::Port::PORT_A, 18};
			peripheral->miso_pin = (SERCOMHAL::Pinout){SERCOMSAMD21::PeripheralFunction::PF_D, SERCOMSAMD21::Port::PORT_A, 16};
			peripheral->sck_pin = (SERCOMHAL::Pinout){SERCOMSAMD21::PeripheralFunction::PF_D, SERCOMSAMD21::Port::PORT_A, 19};
			peripheral->ssl_pin = (SERCOMHAL::Pinout){SERCOMSAMD21::PeripheralFunction::PF_D, SERCOMSAMD21::Port::PORT_A, 17};
			break;
		case SERCOMSAMD21::SercomID::Sercom4:
			peripheral->mosi_pin = (SERCOMHAL::Pinout){SERCOMSAMD21::PeripheralFunction::PF_D, SERCOMSAMD21::Port::PORT_B, 10};
			peripheral->miso_pin = (SERCOMHAL::Pinout){SERCOMSAMD21::PeripheralFunction::PF_D, SERCOMSAMD21::Port::PORT_A, 12};
			peripheral->sck_pin = (SERCOMHAL::Pinout){SERCOMSAMD21::PeripheralFunction::PF_D, SERCOMSAMD21::Port::PORT_B, 11};
			peripheral->ssl_pin = (SERCOMHAL::Pinout){SERCOMSAMD21::PeripheralFunction::PF_D, SERCOMSAMD21::Port::PORT_B, 9};
			break;
		case SERCOMSAMD21::SercomID::Sercom5:
			peripheral->mosi_pin = (SERCOMHAL::Pinout){SERCOMSAMD21::PeripheralFunction::PF_D, SERCOMSAMD21::Port::PORT_B, 22};
			peripheral->miso_pin = (SERCOMHAL::Pinout){SERCOMSAMD21::PeripheralFunction::PF_D, SERCOMSAMD21::Port::PORT_A, 22};
			peripheral->sck_pin = (SERCOMHAL::Pinout){SERCOMSAMD21::PeripheralFunction::PF_D, SERCOMSAMD21::Port::PORT_B, 23};
			peripheral->ssl_pin = (SERCOMHAL::Pinout){SERCOMSAMD21::PeripheralFunction::PF_D, SERCOMSAMD21::Port::PORT_A, 23};
			break;
		default:
			//do nothing
			break;
	}
}

void SPIHAL::GetPeripheralDefaults(SPIHAL::Peripheral * peripheral)
{
	SPISAMD21::GetPeripheralDefaults(peripheral, SERCOMSAMD21::SercomID::Sercom4);	
}

void SPISAMD21::SetGenClkParam(SPIHAL::Peripheral * peripheral, SERCOMSAMD21::GenericClock generic_clock)
{
	peripheral->extra_spi_params[ExtraParams::GEN_CLK] = generic_clock;
}

void SPISAMD21::SetGenClkDivisorParam(SPIHAL::Peripheral * peripheral, uint16_t divisor)
{
	peripheral->extra_spi_params[ExtraParams::GEN_CLK_DIVISOR_BITS_9_16] = (uint8_t)((divisor >> 8) & 0xFF);
	peripheral->extra_spi_params[ExtraParams::GEN_CLK_DIVISOR_BITS_1_8] = (uint8_t)(divisor & 0xFF);
}

void SPISAMD21::SetGenClkRunStandbyParam(SPIHAL::Peripheral * peripheral, bool run_standy)
{
	peripheral->extra_spi_params[ExtraParams::GEN_CLK_RUN_STANDY] = (uint8_t)run_standy;
}

void SPISAMD21::SetPadConfigParam(SPIHAL::Peripheral * peripheral, SPISAMD21::PadConfig pad_config)
{
	peripheral->extra_spi_params[ExtraParams::PAD_CONFIG] = pad_config;
}

void SPIHAL::InitSercom(SPIHAL::Peripheral * peripheral, bool is_client)
{
	bool miso_pin_type = false;
	bool mosi_pin_type = true;
	bool sck_pin_type = true;
	unsigned char spi_mode = 0x3;
	if(is_client)
	{
		miso_pin_type = true;
		mosi_pin_type = false;
		sck_pin_type = false;
		spi_mode = 0x2;
	}
	//preset: 8MHz osc set and enabled, fed into clock generator 0
	SERCOMHAL::ConfigPin(peripheral->miso_pin, miso_pin_type, true);
	SERCOMHAL::ConfigPin(peripheral->mosi_pin, mosi_pin_type, true);
	SERCOMHAL::ConfigPin(peripheral->sck_pin, sck_pin_type, true);
	if(is_client)
		SERCOMHAL::ConfigPin(peripheral->ssl_pin, false, true);	
	else
	{
		SERCOMHAL::ConfigPin(peripheral->ssl_pin, true, false);
		SERCOMHAL::OutputHigh(peripheral->ssl_pin);
	}
	Sercom *sercom_ptr = SERCOMSAMD21::GetSercom(peripheral->sercom_id);
	uint16_t gen_clk_divisor = (((uint16_t)peripheral->extra_spi_params[SPISAMD21::ExtraParams::GEN_CLK_DIVISOR_BITS_9_16] << 8) & 0xFF00) | ((uint16_t)peripheral->extra_spi_params[SPISAMD21::ExtraParams::GEN_CLK_DIVISOR_BITS_1_8] & 0xFF);
	SERCOMSAMD21::EnableSercomClock(peripheral->sercom_id, (SERCOMSAMD21::GenericClock)peripheral->extra_spi_params[SPISAMD21::ExtraParams::GEN_CLK], gen_clk_divisor, (bool)peripheral->extra_spi_params[SPISAMD21::ExtraParams::GEN_CLK_RUN_STANDY]);
	uint8_t mosi = 0;
	uint8_t miso = 0;
	switch(peripheral->extra_spi_params[SPISAMD21::ExtraParams::PAD_CONFIG])
	{
		case SPISAMD21::PadConfig::DO0_DI2_SCK1_CSS2:
			miso = 0x2u;
			mosi = 0x0u;
			break;
		case SPISAMD21::PadConfig::DO0_DI3_SCK1_CSS2:
			miso = 0x3u;	
			mosi = 0x0u;
			break;
		case SPISAMD21::PadConfig::DO0_DI1_SCK3_CSS1:
			miso = 0x1u;
			mosi = 0x3u;
			break;
		case SPISAMD21::PadConfig::DO0_DI2_SCK3_CSS1:
			miso = 0x2u;
			mosi = 0x3u;
			break;
		case SPISAMD21::PadConfig::DO2_DI0_SCK3_CSS1:
			miso = 0x0u;
			mosi = 0x1u;
			break;
		case SPISAMD21::PadConfig::DO2_DI1_SCK3_CSS1:
			miso = 0x1u;
			mosi = 0x1u;		
			break;
		case SPISAMD21::PadConfig::DO3_DI0_SCK1_CSS2:
			miso = 0x0u;
			mosi = 0x2u;
			break;
		case SPISAMD21::PadConfig::DO3_DI2_SCK1_CSS2:
			miso = 0x2u;
			mosi = 0x2u;
			break;
		default:
			//do nothing
			break;
	}
	//set up miso,mosi
	sercom_ptr->SPI.CTRLA.reg = SERCOM_SPI_CTRLA_DIPO(miso) | SERCOM_SPI_CTRLA_DOPO(mosi) | SERCOM_SPI_CTRLA_IBON | SERCOM_SPI_CTRLA_MODE(spi_mode);
	switch(peripheral->clock_mode) {
		case ClockMode::Mode0:
			sercom_ptr->SPI.CTRLA.bit.CPOL = 0;
			sercom_ptr->SPI.CTRLA.bit.CPHA = 0;
			break;
		case ClockMode::Mode1:
			sercom_ptr->SPI.CTRLA.bit.CPOL = 0;
			sercom_ptr->SPI.CTRLA.bit.CPHA = 1;
			break;
		case ClockMode::Mode2:
			sercom_ptr->SPI.CTRLA.bit.CPOL = 1;
			sercom_ptr->SPI.CTRLA.bit.CPHA = 0;
			break;
		case ClockMode::Mode3:
			sercom_ptr->SPI.CTRLA.bit.CPOL = 1;
			sercom_ptr->SPI.CTRLA.bit.CPHA = 1;
			break;
	}
	sercom_ptr->SPI.CTRLA.bit.DORD = (peripheral->endianess == Endian::LSB);
	if(is_client)
	{
		sercom_ptr->SPI.CTRLB.bit.PLOADEN = 0x1;
		sercom_ptr->SPI.CTRLB.bit.SSDE = 0x1;
	} else
		sercom_ptr->SPI.BAUD.reg = (uint8_t)(8000000/(2 * peripheral->baud_value) - 1);
	sercom_ptr->SPI.CTRLB.bit.RXEN = 0x1;
	sercom_ptr->SPI.CTRLA.bit.ENABLE = 0x1;
	while(sercom_ptr->SPI.SYNCBUSY.bit.ENABLE);
}

void SPIHAL::DeinitSercom(SERCOMHAL::SercomID sercom_id)
{
	Sercom *sercom_ptr = SERCOMSAMD21::GetSercom(sercom_id);
	sercom_ptr->SPI.STATUS.reg = SERCOM_SPI_STATUS_RESETVALUE;
	sercom_ptr->SPI.CTRLB.reg = SERCOM_SPI_CTRLB_RESETVALUE;
	while(sercom_ptr->SPI.SYNCBUSY.bit.CTRLB);
	sercom_ptr->SPI.CTRLA.bit.ENABLE = 0x0u;
	while(sercom_ptr->SPI.SYNCBUSY.bit.ENABLE);		//Wait for write to complete
	switch(sercom_id)
	{
		case SERCOMSAMD21::SercomID::Sercom0:
			NVIC_DisableIRQ(SERCOM0_IRQn);
			break;
		case SERCOMSAMD21::SercomID::Sercom1:
			NVIC_DisableIRQ(SERCOM1_IRQn);
			break;
		case SERCOMSAMD21::SercomID::Sercom2:
			NVIC_DisableIRQ(SERCOM2_IRQn);
			break;
		case SERCOMSAMD21::SercomID::Sercom3:
			NVIC_DisableIRQ(SERCOM3_IRQn);
			break;
		case SERCOMSAMD21::SercomID::Sercom4:
			NVIC_DisableIRQ(SERCOM4_IRQn);
			break;
		case SERCOMSAMD21::SercomID::Sercom5:
			NVIC_DisableIRQ(SERCOM5_IRQn);
			break;
		default:
			//do nothing
			break;
	}	
}

bool SPIHAL::ReadyToTransmit(SERCOMHAL::SercomID sercom_id)
{
	Sercom *sercom_ptr = SERCOMSAMD21::GetSercom(sercom_id);
	return sercom_ptr->SPI.INTFLAG.bit.DRE & sercom_ptr->SPI.INTENSET.bit.DRE;
}

bool SPIHAL::ReadyToReceive(SERCOMHAL::SercomID sercom_id)
{
	Sercom *sercom_ptr = SERCOMSAMD21::GetSercom(sercom_id);
	return sercom_ptr->SPI.INTFLAG.bit.RXC & sercom_ptr->SPI.INTENSET.bit.RXC;
}

uint8_t SPIHAL::GetSercomRX(SERCOMHAL::SercomID sercom_id)
{
	Sercom *sercom_ptr = SERCOMSAMD21::GetSercom(sercom_id);
	return sercom_ptr->SPI.DATA.reg;
}

void SPIHAL::SetSercomTX(uint8_t input, SERCOMHAL::SercomID sercom_id)
{
	Sercom *sercom_ptr = SERCOMSAMD21::GetSercom(sercom_id);
	sercom_ptr->SPI.DATA.reg = input;	
}

void SPIHAL::EnableTxEmpty(SERCOMHAL::SercomID sercom_id, bool enable)
{
	Sercom *sercom_ptr = SERCOMSAMD21::GetSercom(sercom_id);
	if(enable) sercom_ptr->SPI.INTENSET.bit.DRE = 0x1u;
	else if(sercom_ptr->SPI.INTENCLR.bit.DRE) sercom_ptr->SPI.INTENCLR.reg &= SERCOM_SPI_INTENCLR_DRE;
}

void SPIHAL::EnableRxFull(SERCOMHAL::SercomID sercom_id, bool enable)
{
	Sercom *sercom_ptr = SERCOMSAMD21::GetSercom(sercom_id);
	if(enable) sercom_ptr->SPI.INTENSET.bit.RXC = 0x1u;
	else if(sercom_ptr->SPI.INTENCLR.bit.RXC) sercom_ptr->SPI.INTENCLR.reg &= SERCOM_SPI_INTENCLR_RXC;
}

bool SPIHAL::TransmitComplete(SERCOMHAL::SercomID sercom_id)
{
	Sercom *sercom_ptr = SERCOMSAMD21::GetSercom(sercom_id);
	uint8_t result = sercom_ptr->SPI.INTFLAG.bit.TXC & sercom_ptr->SPI.INTENSET.bit.TXC;
	if(result) sercom_ptr->SPI.INTFLAG.bit.TXC = 0x1u;
	return result;
}

void SPIHAL::EnableTxComplete(SERCOMHAL::SercomID sercom_id, bool enable)
{
	Sercom *sercom_ptr = SERCOMSAMD21::GetSercom(sercom_id);
	if(enable) sercom_ptr->SPI.INTENSET.bit.TXC = 0x1u;
	else if(sercom_ptr->SPI.INTENCLR.bit.TXC) sercom_ptr->SPI.INTENCLR.reg &= SERCOM_SPI_INTENCLR_TXC;
}

bool SPIHAL::SPISelectLow(SERCOMHAL::SercomID sercom_id)
{
	Sercom *sercom_ptr = SERCOMSAMD21::GetSercom(sercom_id);
	uint8_t result = sercom_ptr->SPI.INTFLAG.bit.SSL & sercom_ptr->SPI.INTENSET.bit.SSL;
	if(result) sercom_ptr->SPI.INTFLAG.bit.SSL = 0x1u;
	return result;
}

void SPIHAL::EnableSPISelectLow(SERCOMHAL::SercomID sercom_id, bool enable)
{
	Sercom *sercom_ptr = SERCOMSAMD21::GetSercom(sercom_id);
	if(enable) sercom_ptr->SPI.INTENSET.bit.SSL = 0x1u;
	else if(sercom_ptr->SPI.INTENCLR.bit.SSL) sercom_ptr->SPI.INTENCLR.reg &= SERCOM_SPI_INTENCLR_SSL;
}

void SPIHAL::EnableSercomErrors(SERCOMHAL::SercomID sercom_id, bool enable)
{
	Sercom *sercom_ptr = SERCOMSAMD21::GetSercom(sercom_id);
	if(enable) sercom_ptr->SPI.INTENSET.bit.ERROR = 0x1u;
	else if(sercom_ptr->SPI.INTENCLR.bit.ERROR) sercom_ptr->SPI.INTENCLR.reg &= SERCOM_SPI_INTENCLR_ERROR;
}

bool SPIHAL::SercomHasErrors(SERCOMHAL::SercomID sercom_id)
{
	Sercom *sercom_ptr = SERCOMSAMD21::GetSercom(sercom_id);
	uint8_t result = sercom_ptr->SPI.INTFLAG.bit.ERROR & sercom_ptr->SPI.INTENSET.bit.ERROR;
	if(result) sercom_ptr->SPI.INTFLAG.bit.ERROR = 0x1u;
	return result;
}

bool SPIHAL::CheckOverflowError(SERCOMHAL::SercomID sercom_id)
{
	Sercom *sercom_ptr = SERCOMSAMD21::GetSercom(sercom_id);
	volatile uint8_t has_overflow_error = sercom_ptr->SPI.STATUS.bit.BUFOVF;
	if (has_overflow_error) sercom_ptr->SPI.STATUS.bit.BUFOVF = 0x1u;
	return has_overflow_error;
}

#endif