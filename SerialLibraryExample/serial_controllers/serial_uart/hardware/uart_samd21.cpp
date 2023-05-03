/* 
 * Name			:	uart_samd21.cpp
 * Created		:	6/23/2022 11:08:29 AM
 * Author		:	Aaron Reilman
 * Description	:	A UART serial communication low level driver for samd21.
 */

#include "serial_uart/uart_hal.h"

#if (UART_MCU_OPT == OPT_SERCOM_SAMD21)

SERCOMSAMD21::GenericClock uart_genclk = SERCOMSAMD21::GEN_CLK_1;
uint16_t uart_genclk_divisor = 0;
bool uart_genclk_run_stdby = false;


void UARTSAMD21::GetPeripheralDefaults(UARTHAL::Peripheral * peripheral, SERCOMHAL::SercomID sercom_id)
{
	peripheral->pad_config = PadConfig::Tx2_Rx3;
	peripheral->baud_value =  115200;
	peripheral->sample_adjustment = SampleAdjustment::Over16x_7_8_9;
	peripheral->parity = UARTHAL::Parity::PNone;
	peripheral->endianness = UARTHAL::Endian::LSB;
	peripheral->num_stop_bits = UARTHAL::StopBits::OneStopBit;
	peripheral->sercom_id = sercom_id;
	switch(sercom_id)
	{
		case SERCOMSAMD21::SercomID::Sercom0:
			peripheral->tx_pin = (SERCOMHAL::Pinout){2, 0, 10};
			peripheral->rx_pin = (SERCOMHAL::Pinout){2, 0, 11};
			break;
		case SERCOMSAMD21::SercomID::Sercom1:
			peripheral->tx_pin = (SERCOMHAL::Pinout){2, 0, 18};
			peripheral->rx_pin = (SERCOMHAL::Pinout){2, 0, 19};
			break;
		case SERCOMSAMD21::SercomID::Sercom2:
			peripheral->tx_pin = (SERCOMHAL::Pinout){3, 0, 10};
			peripheral->rx_pin = (SERCOMHAL::Pinout){3, 0, 11};
			break;
		case SERCOMSAMD21::SercomID::Sercom3:
			peripheral->tx_pin = (SERCOMHAL::Pinout){3, 0, 18};
			peripheral->rx_pin = (SERCOMHAL::Pinout){3, 0, 19};
			break;
		case SERCOMSAMD21::SercomID::Sercom4:
			peripheral->tx_pin = (SERCOMHAL::Pinout){3, 1, 10};
			peripheral->rx_pin = (SERCOMHAL::Pinout){3, 1, 11};
			break;
		case SERCOMSAMD21::SercomID::Sercom5:
			peripheral->pad_config = PadConfig::Tx0_Rx1;
			peripheral->tx_pin = (SERCOMHAL::Pinout){3, 0, 22};
			peripheral->rx_pin = (SERCOMHAL::Pinout){3, 0, 23};
			break;
		default:
			//do nothing
			break;
	}
}

void UARTSAMD21::SetGenClk(SERCOMSAMD21::GenericClock gen_clk_id, uint16_t clock_divisor, bool run_standby)
{
	uart_genclk = gen_clk_id;
	uart_genclk_divisor = clock_divisor;
	uart_genclk_run_stdby = run_standby;
}

void UARTHAL::GetPeripheralDefaults(UARTHAL::Peripheral * peripheral)
{
	UARTSAMD21::GetPeripheralDefaults(peripheral, SERCOMSAMD21::SercomID::Sercom0);
}

void UARTHAL::InitSercom(UARTHAL::Peripheral * peripheral)
{
	//preset: 8MHz osc set and enabled, fed into clock generator 0
	SERCOMHAL::ConfigPin(peripheral->rx_pin, false, true);
	SERCOMHAL::ConfigPin(peripheral->tx_pin, true, true);
	Sercom *sercom_ptr = SERCOMSAMD21::GetSercom(peripheral->sercom_id);
	SERCOMSAMD21::EnableSercomClock(peripheral->sercom_id, uart_genclk, uart_genclk_divisor, uart_genclk_run_stdby);
	uint8_t tx = 0;
	uint8_t rx = 0;
	switch(peripheral->pad_config)
	{
		case UARTSAMD21::PadConfig::Tx0_Rx1:
			tx=0;
			rx=1;
			break;
		case UARTSAMD21::PadConfig::Tx0_Rx2:
			tx=0;
			rx=2;
			break;
		case UARTSAMD21::PadConfig::Tx0_Rx3:
			tx=0;
			rx=3;
			break;
		case UARTSAMD21::PadConfig::Tx2_Rx0:
			tx=1;
			rx=0;
			break;
		case UARTSAMD21::PadConfig::Tx2_Rx1:
			tx=1;
			rx=1;
			break;
		case UARTSAMD21::PadConfig::Tx2_Rx3:
			tx=1;
			rx=3;
			break;
		default:
			//do nothing
			break;
	}
	//set up tx,rx, parity
	sercom_ptr->USART.CTRLA.reg = SERCOM_USART_CTRLA_RXPO(rx) | SERCOM_USART_CTRLA_TXPO(tx) | SERCOM_USART_CTRLA_IBON | SERCOM_USART_CTRLA_MODE(0x1u) | SERCOM_USART_CTRLA_FORM(!(peripheral->parity == UARTHAL::Parity::PNone));
	sercom_ptr->USART.CTRLA.bit.DORD = (peripheral->endianness == Endian::LSB);
	uint8_t sampr = 0;
	uint8_t sampa = 0;
	switch(peripheral->sample_adjustment)
	{
		case UARTSAMD21::SampleAdjustment::Over16x_7_8_9:
			sampa = 0;
			sampr = 0;
			break;
		case UARTSAMD21::SampleAdjustment::Over16x_9_10_11:
			sampa = 1;
			sampr = 0;
			break;
		case UARTSAMD21::SampleAdjustment::Over16x_11_12_13:
			sampa = 2;
			sampr = 0;
			break;
		case UARTSAMD21::SampleAdjustment::Over16x_13_14_15:
			sampa = 3;
			sampr = 0;
			break;
		case UARTSAMD21::SampleAdjustment::Over8x_3_4_5:
			sampa = 0;
			sampr = 2;
			break;
		case UARTSAMD21::SampleAdjustment::Over8x_4_5_6:
			sampa = 1;
			sampr = 2;
			break;
		case UARTSAMD21::SampleAdjustment::Over8x_5_6_7:
			sampa = 2;
			sampr = 2;
			break;
		case UARTSAMD21::SampleAdjustment::Over8x_6_7_8:
			sampa = 3;
			sampr = 2;
			break;
		default:
			//do nothing
			break;
	}
	sercom_ptr->USART.CTRLA.bit.SAMPA = sampa;
	sercom_ptr->USART.CTRLA.bit.SAMPR = sampr;
	//even/odd parity set, number of data bits and stop bits
	sercom_ptr->USART.CTRLB.reg = SERCOM_USART_CTRLB_RXEN | SERCOM_USART_CTRLB_TXEN;
	sercom_ptr->USART.CTRLB.bit.PMODE = (peripheral->parity == UARTHAL::Parity::POdd);
	sercom_ptr->USART.CTRLB.bit.SBMODE = (peripheral->num_stop_bits == StopBits::TwoStopBits);
	// Baud register value calculated from datasheet Table 25-2
	sercom_ptr->USART.BAUD.reg = (uint32_t)(65536u * (1u - 0.000002 *peripheral->baud_value) + 1u);
	sercom_ptr->USART.CTRLA.bit.ENABLE = 1u;       // Enable SERCOM
	while(sercom_ptr->USART.SYNCBUSY.bit.ENABLE);     // Wait for write to complete
}

void UARTHAL::DeinitSercom(SERCOMHAL::SercomID sercom_id)
{
	Sercom *sercom_ptr = SERCOMSAMD21::GetSercom(sercom_id);
	sercom_ptr->USART.STATUS.reg = SERCOM_USART_STATUS_RESETVALUE;
	sercom_ptr->USART.CTRLB.reg = SERCOM_USART_CTRLB_RESETVALUE;
	while(sercom_ptr->USART.SYNCBUSY.bit.CTRLB);
	sercom_ptr->USART.CTRLA.bit.ENABLE = 0x0u;
	while(sercom_ptr->USART.SYNCBUSY.bit.ENABLE);		//Wait for write to complete
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

bool UARTHAL::ReadyToTransmit(SERCOMHAL::SercomID sercom_id)
{
	Sercom *sercom_ptr = SERCOMSAMD21::GetSercom(sercom_id);
	return sercom_ptr->USART.INTFLAG.bit.DRE & sercom_ptr->USART.INTENSET.bit.DRE;
}

bool UARTHAL::ReadyToReceive(SERCOMHAL::SercomID sercom_id)
{
	Sercom *sercom_ptr = SERCOMSAMD21::GetSercom(sercom_id);
	return sercom_ptr->USART.INTFLAG.bit.RXC & sercom_ptr->USART.INTENSET.bit.RXC;
}

uint8_t UARTHAL::GetSercomRX(SERCOMHAL::SercomID sercom_id)
{ 
	Sercom *sercom_ptr = SERCOMSAMD21::GetSercom(sercom_id);
	return sercom_ptr->USART.DATA.reg;
}

void UARTHAL::SetSercomTX(uint8_t input, SERCOMHAL::SercomID sercom_id)
{
	Sercom *sercom_ptr = SERCOMSAMD21::GetSercom(sercom_id);
	sercom_ptr->USART.DATA.reg = input;
}

void UARTHAL::EnableTxEmpty(SERCOMHAL::SercomID sercom_id, bool enable)
{
	Sercom *sercom_ptr = SERCOMSAMD21::GetSercom(sercom_id);
	if(enable) sercom_ptr->USART.INTENSET.bit.DRE = 0x1u;
	else if(sercom_ptr->USART.INTENCLR.bit.DRE) sercom_ptr->USART.INTENCLR.reg &= SERCOM_USART_INTENCLR_DRE;
}

void UARTHAL::EnableRxFull(SERCOMHAL::SercomID sercom_id, bool enable)
{
	Sercom *sercom_ptr = SERCOMSAMD21::GetSercom(sercom_id);
	if(enable) sercom_ptr->USART.INTENSET.bit.RXC = 0x1u;
	else if(sercom_ptr->USART.INTENCLR.bit.RXC) sercom_ptr->USART.INTENCLR.reg &= SERCOM_USART_INTENCLR_RXC;
}

bool UARTHAL::TransmitComplete(SERCOMHAL::SercomID sercom_id)
{
	Sercom *sercom_ptr = SERCOMSAMD21::GetSercom(sercom_id);
	volatile uint8_t transmit_complete = sercom_ptr->USART.INTFLAG.bit.TXC & sercom_ptr->USART.INTENSET.bit.TXC;
	if (transmit_complete) sercom_ptr->USART.INTFLAG.bit.TXC = 0x1u;
	return transmit_complete;
}

bool UARTHAL::ReceiveStart(SERCOMHAL::SercomID sercom_id)
{
	Sercom *sercom_ptr = SERCOMSAMD21::GetSercom(sercom_id);
	volatile uint8_t receive_start = sercom_ptr->USART.INTFLAG.bit.RXS & sercom_ptr->USART.INTENSET.bit.RXS;
	if (receive_start) sercom_ptr->USART.INTFLAG.bit.RXS = 0x1u;
	return receive_start;
}

bool UARTHAL::ClearToSend(SERCOMHAL::SercomID sercom_id)
{
	Sercom *sercom_ptr = SERCOMSAMD21::GetSercom(sercom_id);
	volatile uint8_t ctsic_irq = sercom_ptr->USART.INTFLAG.bit.CTSIC & sercom_ptr->USART.INTENSET.bit.CTSIC;
	if (ctsic_irq) sercom_ptr->USART.INTFLAG.bit.CTSIC = 0x1u;
	return ctsic_irq;
}

void UARTHAL::EnableTxComplete(SERCOMHAL::SercomID sercom_id, bool enable)
{
	Sercom *sercom_ptr = SERCOMSAMD21::GetSercom(sercom_id);
	if(enable) sercom_ptr->USART.INTENSET.bit.TXC = 0x1u;
	else if(sercom_ptr->USART.INTENCLR.bit.TXC) sercom_ptr->USART.INTENCLR.reg &= SERCOM_USART_INTENCLR_TXC;
}

void UARTHAL::EnableRxStart(SERCOMHAL::SercomID sercom_id, bool enable)
{
	Sercom *sercom_ptr = SERCOMSAMD21::GetSercom(sercom_id);
	if(enable) sercom_ptr->USART.INTENSET.bit.RXS = 0x1u;
	else if(sercom_ptr->USART.INTENCLR.bit.RXS) sercom_ptr->USART.INTENCLR.reg &= SERCOM_USART_INTENCLR_RXS;
}

void UARTHAL::EnableClearToSend(SERCOMHAL::SercomID sercom_id, bool enable)
{
	Sercom *sercom_ptr = SERCOMSAMD21::GetSercom(sercom_id);
	if(enable) sercom_ptr->USART.INTENSET.bit.CTSIC = 0x1u;
	else if(sercom_ptr->USART.INTENCLR.bit.CTSIC) sercom_ptr->USART.INTENCLR.reg &= SERCOM_USART_INTENCLR_CTSIC;
}

void UARTHAL::EnableSercomErrors(SERCOMHAL::SercomID sercom_id, bool enable)
{
	Sercom *sercom_ptr = SERCOMSAMD21::GetSercom(sercom_id);
	if(enable) sercom_ptr->USART.INTENSET.bit.ERROR = 0x1u;
	else if(sercom_ptr->USART.INTENCLR.bit.ERROR) sercom_ptr->USART.INTENCLR.reg &= SERCOM_USART_INTENCLR_ERROR;
}

bool UARTHAL::SercomHasErrors(SERCOMHAL::SercomID sercom_id)
{
	Sercom *sercom_ptr = SERCOMSAMD21::GetSercom(sercom_id);
	uint8_t result = sercom_ptr->USART.INTFLAG.bit.ERROR & sercom_ptr->USART.INTENSET.bit.ERROR;
	if(result) sercom_ptr->USART.INTFLAG.bit.ERROR = 0x1u;
	return result;
}

bool UARTHAL::CheckCollision(SERCOMHAL::SercomID sercom_id)
{
	Sercom *sercom_ptr = SERCOMSAMD21::GetSercom(sercom_id);
	volatile uint8_t has_collision = sercom_ptr->USART.STATUS.bit.COLL;
	if (has_collision)
	{
		while(sercom_ptr->USART.SYNCBUSY.bit.CTRLB);
		sercom_ptr->USART.STATUS.bit.COLL = 0x1u;
		sercom_ptr->USART.INTFLAG.bit.ERROR = 0x1u;
		sercom_ptr->USART.INTFLAG.bit.TXC = 0x1u;
		sercom_ptr->USART.CTRLB.bit.TXEN = 0x1u;
		while(sercom_ptr->USART.SYNCBUSY.bit.CTRLB);
	}
	return has_collision;
}

bool UARTHAL::CheckSyncError(SERCOMHAL::SercomID sercom_id)
{
	Sercom *sercom_ptr = SERCOMSAMD21::GetSercom(sercom_id);
	volatile uint8_t has_sync_err = sercom_ptr->USART.STATUS.bit.ISF;
	if (has_sync_err) sercom_ptr->USART.STATUS.bit.ISF = 0x1u;
	return has_sync_err;
}

bool UARTHAL::CheckOverflowError(SERCOMHAL::SercomID sercom_id)
{
	Sercom *sercom_ptr = SERCOMSAMD21::GetSercom(sercom_id);
	volatile uint8_t has_overflow_err = sercom_ptr->USART.STATUS.bit.BUFOVF;
	if (has_overflow_err) sercom_ptr->USART.STATUS.bit.BUFOVF = 0x1u;
	return has_overflow_err;
}

bool UARTHAL::CheckFrameError(SERCOMHAL::SercomID sercom_id)
{
	Sercom *sercom_ptr = SERCOMSAMD21::GetSercom(sercom_id);
	volatile uint8_t has_frame_err = sercom_ptr->USART.STATUS.bit.FERR;
	if (has_frame_err) sercom_ptr->USART.STATUS.bit.FERR = 0x1u;
	return has_frame_err;
}

bool UARTHAL::CheckParityError(SERCOMHAL::SercomID sercom_id)
{
	Sercom *sercom_ptr = SERCOMSAMD21::GetSercom(sercom_id);
	volatile uint8_t has_parity_err = sercom_ptr->USART.STATUS.bit.PERR;
	if (has_parity_err) sercom_ptr->USART.STATUS.bit.PERR = 0x1u;
	return has_parity_err;
}

#endif