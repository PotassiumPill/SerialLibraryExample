/*
 * Name				:	serial_uart.cpp
 * Created			:	6/8/2022 11:48:18 AM
 * Author			:	Aaron Reilman
 * Description		:	A UART serial communication driver.
 */ 


#include "serial_uart/serial_uart.h"

//Definition of Controller Class
SerialUART::UARTController::UARTController(void)
{
	error_on = false;
	status.uart_on = false;
}

SerialUART::UARTController::~UARTController(void)
{
	Deinit();
}

void SerialUART::UARTController::Init(UARTHAL::Peripheral * p_config, char * tx_buf, uint32_t tx_size, char * rx_buf, uint32_t rx_size)
{
	if(!status.uart_on)
	{
		UARTHAL::InitSercom(p_config);
		rx_buffer.SetSercomID(p_config->sercom_id);
		ResetTXBuffer(tx_buf, tx_size);
		ResetRXBuffer(rx_buf, rx_size);
		//configure optional interrupts
		EnableErrorIRQ(error_on);
		status = (Status){TXIRQState::None, RXIRQState::None, UARTError::ENone, true};
	}
}

void SerialUART::UARTController::Deinit(void)
{
	if(status.uart_on)
	{
		ClearBuffers(true, true);
		UARTHAL::DeinitSercom(rx_buffer.GetSercomID());
		UARTHAL::EnableTxEmpty(rx_buffer.GetSercomID(), false);
		UARTHAL::EnableRxFull(rx_buffer.GetSercomID(), false);
		status.uart_on = false;
	}
}

void SerialUART::UARTController::ResetTXBuffer(char * tx_buf, uint32_t tx_size)
{
	tx_buffer.ResetBuffer(tx_buf, tx_size);
	UARTHAL::EnableTxEmpty(rx_buffer.GetSercomID(), false);
	status.tx_interrupt = TXIRQState::None;
}

void SerialUART::UARTController::ResetRXBuffer(char * rx_buf, uint32_t rx_size)
{
	rx_buffer.Reset(rx_buf, rx_size);
	UARTHAL::EnableRxFull(rx_buffer.GetSercomID(), true);
	status.rx_interrupt = RXIRQState::None;
	status.error_state = UARTError::ENone;
}

void SerialUART::UARTController::ClearBuffers(bool clear_tx, bool clear_rx)
{
	if(clear_tx) 
	{
		tx_buffer.Clear();
		UARTHAL::EnableTxEmpty(rx_buffer.GetSercomID(), false);
		status.tx_interrupt = TXIRQState::None;
	}
	if(clear_rx)
	{ 
		rx_buffer.Clear();
		UARTHAL::EnableRxFull(rx_buffer.GetSercomID(), true);
		status.rx_interrupt = RXIRQState::None;
		status.error_state = UARTError::ENone;
	}
}

void SerialUART::UARTController::ISR(void)
{
	//transmit isr
	if(UARTHAL::ReadyToTransmit(rx_buffer.GetSercomID()))
	{
		if(tx_buffer.GetBufferState() == GenericBuffer::BufferState::Empty)
		{
			status.tx_interrupt = TXIRQState::TxComplete;
			UARTHAL::EnableTxEmpty(rx_buffer.GetSercomID(), false);
		} else {
			UARTHAL::SetSercomTX((uint8_t)(GetTXBuffer()),rx_buffer.GetSercomID());
			status.tx_interrupt = TXIRQState::TxSuccess;
		}
	}
	//receive and error isr
	if(error_on && UARTHAL::SercomHasErrors(rx_buffer.GetSercomID())) 	
	{
		status.rx_interrupt = RXIRQState::RxError;
		HandleErrors();
	}
	else if(UARTHAL::ReadyToReceive(rx_buffer.GetSercomID()))
	{
		if(rx_buffer.GetBufferState() == GenericBuffer::BufferState::Full)
		{
			UARTHAL::EnableRxFull(rx_buffer.GetSercomID(), false);
			status.rx_interrupt = RXIRQState::RxComplete;	
		} else {
			PutRXBuffer((char)(UARTHAL::GetSercomRX(rx_buffer.GetSercomID())));
			status.rx_interrupt = RXIRQState::RxSuccess;
		}
	}
}

bool SerialUART::UARTController::Transmit(char input)
{
	bool success = false;
	if(status.uart_on)
	{
		UARTHAL::EnableTxEmpty(rx_buffer.GetSercomID(), false);
		success = tx_buffer.Put(input);
		UARTHAL::EnableTxEmpty(rx_buffer.GetSercomID(), true);
	}
	return success;
}

bool SerialUART::UARTController::Receive(char * output)
{
	return rx_buffer.Get(&UARTHAL::EnableRxFull, output);
}

bool SerialUART::UARTController::TransmitPacket(const char * input, uint32_t num_bytes)
{
	bool success = false;
	if(status.uart_on)
	{
		for(uint32_t i = 0; i < num_bytes; i++)
		{
			while(!Transmit(input[i]));
		}
	}
	return success;
}

uint32_t SerialUART::UARTController::TransmitString(const char *input)
{
	uint32_t count = 0;
	if(status.uart_on)
	{
		while(input[count] != '\0')
		{
			while(!Transmit(input[count]));	//wait to transmit successfully (transmit will fail if buffer is full)
			count++;
		}
	}
	return count;
}

bool SerialUART::UARTController::EchoRx(void)
{
	bool success = false;
	//prevent transmit if rx is empty or tx is full to avoid getting stuck if called from ISR handler
	if (status.uart_on && tx_buffer.GetBufferState() != GenericBuffer::BufferState::Full && rx_buffer.GetBufferState() != GenericBuffer::BufferState::Empty)
	{
		char output;
		Receive(&output);
		Transmit(output);
		success = true;
	}
	return success;
}

bool SerialUART::UARTController::ReceiveString(const char *input, uint32_t shift, bool move_pointer)
{
	return rx_buffer.GetString(input, shift, move_pointer, &UARTHAL::EnableRxFull);
}

bool SerialUART::UARTController::TransmitInt(uint32_t input)
{
	char packet[10];
	uint32_t numel = Serial::Int2ASCII(input, &packet);
	return TransmitPacket(packet, numel);
}

bool SerialUART::UARTController::ReceiveInt(uint32_t * output)
{
	return rx_buffer.GetASCIIAsInt(output, &UARTHAL::EnableRxFull);
}

bool SerialUART::UARTController::ReceiveParam(uint32_t * output, const char *input, char delimiter, uint8_t max_digits)
{
	return rx_buffer.GetIntParam(output, input, delimiter, max_digits, &UARTHAL::EnableRxFull);;
}

//private helper function
void SerialUART::UARTController::PutRXBuffer(char input)
{
	rx_buffer.Put(input, &Serial::NoIntEnable);
}

//private helper function
char SerialUART::UARTController::GetTXBuffer(void)
{
	char output = 0;
	tx_buffer.Get(&output);
	return output;
}

//private helper function
void SerialUART::UARTController::HandleErrors(void)
{
	if(UARTHAL::CheckSyncError(rx_buffer.GetSercomID())) status.error_state = UARTError::ESync;
	else if(UARTHAL::CheckOverflowError(rx_buffer.GetSercomID())) status.error_state = UARTError::EOverflow;
	else if(UARTHAL::CheckFrameError(rx_buffer.GetSercomID())) status.error_state = UARTError::EFrame;
	else if(UARTHAL::CheckParityError(rx_buffer.GetSercomID())) status.error_state = UARTError::EParity;
}

//getters
uint32_t SerialUART::UARTController::GetTXEmpty(void) const
{
	return tx_buffer.GetSize() - tx_buffer.GetBufferAvailable();
}
 
uint32_t SerialUART::UARTController::GetRXAvailable(void) const
{
	return rx_buffer.GetBufferAvailable();
}

SerialUART::BufferStates SerialUART::UARTController::GetBufferStates(void) const
{
	return (BufferStates){
		.tx_buffer_state = tx_buffer.GetBufferState(), 
		.rx_buffer_state = rx_buffer.GetBufferState()
		};
}

SerialUART::Status SerialUART::UARTController::GetStatus(void) const
{
	return status;
}

void SerialUART::UARTController::ClearTXInterrupt(void)
{
	status.tx_interrupt = TXIRQState::None;
}

void SerialUART::UARTController::ClearRXInterrupt(void)
{
	status.rx_interrupt = RXIRQState::None;
}

void SerialUART::UARTController::ClearErrors(void)
{
	status.error_state = UARTError::ENone;
}

//enablers
void SerialUART::UARTController::EnableErrorIRQ(bool enable)
{
	error_on = enable;
	UARTHAL::EnableSercomErrors(rx_buffer.GetSercomID(), enable);
}

