/* 
 * Name			:	serial_spi.cpp
 * Created		:	10/13/2022 4:59:36 PM
 * Author		:	Aaron Reilman
 * Description	:	A SPI serial communication driver.
 */


#include "serial_spi/serial_spi.h"

SerialSPI::SPIController::SPIController(void)
{
	error_on = false;
	status.spi_on = false;
}

SerialSPI::SPIController::~SPIController(void)
{
	Deinit();
}

void SerialSPI::SPIController::Init(SPIHAL::Peripheral * p_config, char * mosi_buf, uint32_t mosi_size, char * miso_buf, uint32_t miso_size)
{
	if(!status.spi_on)
	{
		ssl_pin = p_config->ssl_pin;
		SPIHAL::InitSercom(p_config);
		sercom_id = p_config->sercom_id;
		ResetMOSIBuffer(mosi_buf, mosi_size);
		ResetMISOBuffer(miso_buf, miso_size);
		//configure optional interrupts
		EnableErrorIRQ(error_on);
		status = (Status){TXIRQState::None, RXIRQState::None, SPIError::ENone, true};
	}
}

void SerialSPI::SPIController::Deinit(void)
{
	if(status.spi_on)
	{
		ClearBuffers(true, true);
		SPIHAL::DeinitSercom(sercom_id);
		SPIHAL::EnableRxFull(sercom_id, false);
		SPIHAL::EnableTxEmpty(sercom_id, false);
		status.spi_on = false;
	}
}

void SerialSPI::SPIController::ResetMOSIBuffer(char * mosi_buf, uint32_t mosi_size)
{
	mosi_buffer.ResetBuffer(mosi_buf, mosi_size);
	SPIHAL::EnableTxEmpty(sercom_id, false);
	status.tx_interrupt = TXIRQState::None;
}

void SerialSPI::SPIController::ResetMISOBuffer(char * miso_buf, uint32_t miso_size)
{
	miso_buffer.ResetBuffer(miso_buf, miso_size);
	SPIHAL::EnableRxFull(sercom_id, true);
	status.rx_interrupt = RXIRQState::None;
	status.error_state = SPIError::ENone;
}

void SerialSPI::SPIController::ClearBuffers(bool clear_mosi, bool clear_miso)
{
	if(clear_mosi)
	{
		 mosi_buffer.Clear();
		 SPIHAL::EnableTxEmpty(sercom_id, false);
		 status.tx_interrupt = TXIRQState::None;
	}
	if(clear_miso)
	{
		miso_buffer.Clear();
		SPIHAL::EnableRxFull(sercom_id, true);
		status.rx_interrupt = RXIRQState::None;
		status.error_state = SPIError::ENone;
	}
}

void SerialSPI::SPIController::ISR(void)
{
	//transmit isr
	if(SPIHAL::ReadyToTransmit(sercom_id))
	{
		if(mosi_buffer.GetBufferState() == GenericBuffer::BufferState::Empty)
		{
			status.tx_interrupt = TXIRQState::TXComplete;
			SPIHAL::EnableTxEmpty(sercom_id, false);
		} else {	
			SPIHAL::SetSercomTX((uint8_t)(GetMOSIBuffer()),sercom_id);
			status.tx_interrupt = TXIRQState::TXSuccess;
		}
	}
	//receive and error isr
	if(error_on && SPIHAL::SercomHasErrors(sercom_id))
	{
		status.rx_interrupt = RXIRQState::RXError;
		HandleErrors();
	}
	else if(SPIHAL::ReadyToReceive(sercom_id))
	{
		if(miso_buffer.GetBufferState() == GenericBuffer::BufferState::Full)
		{
			SPIHAL::EnableRxFull(sercom_id, false);
			status.rx_interrupt = RXIRQState::RXComplete;
		} else {
			PutMISOBuffer((char)(SPIHAL::GetSercomRX(sercom_id)));
			status.rx_interrupt = RXIRQState::RXSuccess;
		}
	}
}

char SerialSPI::SPIController::SPIHostProcedure(uint32_t num_bytes, const char * bytes, uint32_t num_read)
{
	SSLow();
	char status_val = '\0';
	for(uint32_t i = 0; i < num_bytes; i++)
	{
		while(!Transmit(bytes[i]));
		while(!Receive(&status_val));
		
	}
	for(uint32_t i = 0; i < num_read; i++)
	{
		ClearMISOInterrupt();
		while(!Transmit(0x00));
		while(status.rx_interrupt == SerialSPI::RXIRQState::None);
		if(status.rx_interrupt == RXIRQState::RXComplete) i = num_read;	//breaks loop if miso buffer fills
	}
	SSHigh();
	ClearMOSIInterrupt();
	ClearMISOInterrupt();
	return status_val;
}

bool SerialSPI::SPIController::Transmit(char input)
{
	bool success = false;
	if(status.spi_on)
	{
		SPIHAL::EnableTxEmpty(sercom_id, false);
		success = mosi_buffer.Put(input);
		SPIHAL::EnableTxEmpty(sercom_id, true);
	}
	return success;
}

bool SerialSPI::SPIController::Receive(char * output)
{
	bool success;
	SPIHAL::EnableRxFull(sercom_id, false);
	success = miso_buffer.Get(output);
	SPIHAL::EnableRxFull(sercom_id, true);
	return success;
}

//private helper function
void SerialSPI::SPIController::PutMISOBuffer(char input)
{
	miso_buffer.Put(input);
}

//private helper function
char SerialSPI::SPIController::GetMOSIBuffer(void)
{
	char output = 0;
	mosi_buffer.Get(&output);
	return output;
}

//private helper function
void SerialSPI::SPIController::HandleErrors(void)
{
	if(SPIHAL::CheckOverflowError(sercom_id)) 
		status.error_state = SPIError::EOverflow;
}

//getters
uint32_t SerialSPI::SPIController::GetMISOAvailable(void) const
{
	return miso_buffer.GetBufferAvailable();
}

uint32_t SerialSPI::SPIController::GetMOSIEmpty(void) const
{
	return mosi_buffer.GetSize() - mosi_buffer.GetBufferAvailable();
}

SerialSPI::BufferStates SerialSPI::SPIController::GetBufferStates(void) const
{
	return (BufferStates){
		.mosi_buffer_state = mosi_buffer.GetBufferState(),
		.miso_buffer_state = miso_buffer.GetBufferState()
	};
}

SerialSPI::Status SerialSPI::SPIController::GetStatus(void) const
{
	return status;
}

//status clears
void SerialSPI::SPIController::ClearMOSIInterrupt(void)
{
	status.tx_interrupt = TXIRQState::None;
}

void SerialSPI::SPIController::ClearMISOInterrupt(void)
{
	status.rx_interrupt = RXIRQState::None;
}

void SerialSPI::SPIController::ClearErrors(void)
{
	status.error_state = SPIError::ENone;
}

void SerialSPI::SPIController::EnableErrorIRQ(bool enable)
{
	error_on = enable;
	SPIHAL::EnableSercomErrors(sercom_id, enable);
}

void SerialSPI::SPIController::SSHigh(void)
{
	SERCOMHAL::OutputHigh(ssl_pin);	
}

void SerialSPI::SPIController::SSLow(void)
{
	SERCOMHAL::OutputLow(ssl_pin);	
}

void SerialSPI::SPIController::ChangeSSL(SERCOMHAL::Pinout ssl_pinout)
{
	ssl_pin = ssl_pinout;
	SERCOMHAL::ConfigPin(ssl_pinout, true, false);
	SERCOMHAL::OutputHigh(ssl_pinout);
}