/*
 * spi_chipname.cpp
 *
 * Created: 6/2/2023 2:35:59 PM
 *  Author: aaron
 */ 


#include "serial_spi/spi_config.h"

#if (SPI_MCU_OPT == OPT_SERCOM_CHIPNAME)

#include "serial_spi/hardware/spi_chipname.h"


void SPICHIPNAME::HelperSPI(void)
{
	
}

void SPIHAL::GetPeripheralDefaults(SPIHAL::Peripheral * peripheral)
{
	(void)peripheral;
}

void SPIHAL::InitSercom(SPIHAL::Peripheral * peripheral, bool is_client)
{
	(void)peripheral;
	(void)is_client;
}

void SPIHAL::DeinitSercom(SERCOMHAL::SercomID sercom_id)
{
	(void)sercom_id;
}

bool SPIHAL::ReadyToTransmit(SERCOMHAL::SercomID sercom_id)
{
	(void)sercom_id;
	return false;
}

bool SPIHAL::ReadyToReceive(SERCOMHAL::SercomID sercom_id)
{
	(void)sercom_id;
	return false;
}

uint8_t SPIHAL::GetSercomRX(SERCOMHAL::SercomID sercom_id)
{
	(void)sercom_id;
	return 0x00;
}

void SPIHAL::SetSercomTX(uint8_t input, SERCOMHAL::SercomID sercom_id)
{
	(void)input;
	(void)sercom_id;
}

void SPIHAL::EnableTxEmpty(SERCOMHAL::SercomID sercom_id, bool enable)
{
	(void)sercom_id;
	(void)enable;
}

void SPIHAL::EnableRxFull(SERCOMHAL::SercomID sercom_id, bool enable)
{
	(void)sercom_id;
	(void)enable;
}

bool SPIHAL::TransmitComplete(SERCOMHAL::SercomID sercom_id)
{
	(void)sercom_id;
	return false;
}

void SPIHAL::EnableTxComplete(SERCOMHAL::SercomID sercom_id, bool enable)
{
	(void)sercom_id;
	(void)enable;
}

bool SPIHAL::SPISelectLow(SERCOMHAL::SercomID sercom_id)
{
	(void)sercom_id;
	return false;
}

void SPIHAL::EnableSPISelectLow(SERCOMHAL::SercomID sercom_id, bool enable)
{
	(void)sercom_id;
	(void)enable;
}

void SPIHAL::EnableSercomErrors(SERCOMHAL::SercomID sercom_id, bool enable)
{
	(void)sercom_id;
	(void)enable;
}

bool SPIHAL::SercomHasErrors(SERCOMHAL::SercomID sercom_id)
{
	(void)sercom_id;
	return false;
}

bool SPIHAL::CheckOverflowError(SERCOMHAL::SercomID sercom_id)
{
	(void)sercom_id;
	return false;
}

#endif