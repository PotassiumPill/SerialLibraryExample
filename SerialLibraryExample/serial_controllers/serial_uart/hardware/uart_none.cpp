/*
 * Name			:	uart_none.cpp
 * Created		:	1/23/2023 11:42:59 AM
 * Author		:	Aaron Reilman
 * Description	:	Blank template for hardware implementations of UART.
 */

#include "serial_uart/uart_hal.h"

#if (UART_MCU_OPT == OPT_SERCOM_NONE)

void UARTHAL::GetPeripheralDefaults(UARTHAL::Peripheral * peripheral)
{
	(void)peripheral;
}

void UARTHAL::InitSercom(UARTHAL::Peripheral * peripheral)
{
	(void)peripheral;
}

void UARTHAL::DeinitSercom(SERCOMHAL::SercomID sercom_id)
{
	(void)sercom_id;
}

bool UARTHAL::ReadyToTransmit(SERCOMHAL::SercomID sercom_id)
{
	(void)sercom_id;
	return false;
}

bool UARTHAL::ReadyToReceive(SERCOMHAL::SercomID sercom_id)
{
	(void)sercom_id;
	return false;
}

uint8_t UARTHAL::GetSercomRX(SERCOMHAL::SercomID sercom_id)
{
	(void)sercom_id;
	return 0x00;
}

void UARTHAL::SetSercomTX(uint8_t input, SERCOMHAL::SercomID sercom_id)
{
	(void)input;
	(void)sercom_id;
}

void UARTHAL::EnableTxEmpty(SERCOMHAL::SercomID sercom_id, bool enable)
{
	(void)sercom_id;
	(void)enable;
}

void UARTHAL::EnableRxFull(SERCOMHAL::SercomID sercom_id, bool enable)
{
	(void)sercom_id;
	(void)enable;
}

bool UARTHAL::TransmitComplete(SERCOMHAL::SercomID sercom_id)
{
	(void)sercom_id;
	return false;
}

bool UARTHAL::ReceiveStart(SERCOMHAL::SercomID sercom_id)
{
	(void)sercom_id;
	return false;
}

bool UARTHAL::ClearToSend(SERCOMHAL::SercomID sercom_id)
{
	(void)sercom_id;
	return false;
}

void UARTHAL::EnableTxComplete(SERCOMHAL::SercomID sercom_id, bool enable)
{
	(void)sercom_id;
	(void)enable;
}

void UARTHAL::EnableRxStart(SERCOMHAL::SercomID sercom_id, bool enable)
{
	(void)sercom_id;
	(void)enable;
}

void UARTHAL::EnableClearToSend(SERCOMHAL::SercomID sercom_id, bool enable)
{
	(void)sercom_id;
	(void)enable;
}


void UARTHAL::EnableSercomErrors(SERCOMHAL::SercomID sercom_id, bool enable)
{
	(void)sercom_id;
	(void)enable;
}

bool UARTHAL::SercomHasErrors(SERCOMHAL::SercomID sercom_id)
{
	(void)sercom_id;
	return false;
}

bool UARTHAL::CheckCollision(SERCOMHAL::SercomID sercom_id)
{
	(void)sercom_id;
	return false;
}

bool UARTHAL::CheckSyncError(SERCOMHAL::SercomID sercom_id)
{
	(void)sercom_id;
	return false;
}

bool UARTHAL::CheckOverflowError(SERCOMHAL::SercomID sercom_id)
{
	(void)sercom_id;
	return false;
}

bool UARTHAL::CheckFrameError(SERCOMHAL::SercomID sercom_id)
{
	(void)sercom_id;
	return false;
}

bool UARTHAL::CheckParityError(SERCOMHAL::SercomID sercom_id)
{
	(void)sercom_id;
	return false;
}

#endif
