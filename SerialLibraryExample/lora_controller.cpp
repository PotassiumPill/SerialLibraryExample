/* 
 * Name			:	lora_controller.cpp
 * Created		:	10/18/2022 4:10:48 PM
 * Author		:	Aaron Reilman
 * Description	:	A Lora Communication driver.
 */


#include "lora_controller.h"

#if SPI_MCU_OPT == OPT_SERCOM_SAMD21
SERCOMHAL::Pinout LoRa::DEFAULT_LORA_BUSY_PIN = {0, 0, 19};
SERCOMHAL::Pinout LoRa::DEFAULT_LORA_IRQ_PIN = {0, 0, 18};
SERCOMHAL::Pinout LoRa::DEFAULT_LORA_RESET_PIN = {0, 0, 17};
#else
SERCOMHAL::Pinout LoRa::DEFAULT_LORA_BUSY_PIN = {0, 0, 0};
SERCOMHAL::Pinout LoRa::DEFAULT_LORA_IRQ_PIN = {0, 0, 0};
SERCOMHAL::Pinout LoRa::DEFAULT_LORA_RESET_PIN = {0, 0, 0};
#endif

void LoRa::GetConfigDefaults(LoRa::Config * config_peripheral)
{
	SPIHAL::GetPeripheralDefaults(&(config_peripheral->spi_sercom));
	config_peripheral->busy_pin = DEFAULT_LORA_BUSY_PIN;
	config_peripheral->irq_pin = DEFAULT_LORA_IRQ_PIN;
	config_peripheral->reset_pin = DEFAULT_LORA_BUSY_PIN;
	config_peripheral->frequency = 915000000;
	config_peripheral->output_power = OutputPower::dBm22;
	config_peripheral->ramp_time = TxRampTime::SET_RAMP_20U;
	config_peripheral->spread_factor = SpreadFactor::SF7;
	config_peripheral->bandwidth = Bandwidth::LORA_BW_125;
	config_peripheral->coding_rate = CodingRate::LORA_CR_4_5;
	config_peripheral->low_data_rate_opt = false;
	config_peripheral->preamble_symbols = 8;
	config_peripheral->crc_on = true;
	config_peripheral->public_network = false;
}

LoRa::LoRaController::LoRaController(void)
{
	//do nothing
}

LoRa::LoRaController::~LoRaController(void)
{
	Teardown(); 
}

void LoRa::LoRaController::Init(LoRa::Config * config_peripheral, char * rx_buf, uint32_t rx_buf_size)
{
	ResetRxBuffer(rx_buf, rx_buf_size);
	reset_pin = config_peripheral->reset_pin;
	SERCOMHAL::ConfigPin(reset_pin, true, false);
	SERCOMHAL::OutputHigh(reset_pin);
	ResetLoRa();
	//configure busy, tx irq, rx irq
	busy_pin = config_peripheral->busy_pin;
	irq_pin = config_peripheral->irq_pin;
	SERCOMHAL::ConfigPin(busy_pin, false, false);
	SERCOMHAL::ConfigPin(irq_pin, false, false);
	//configure spi controller
	spi_control.Init(&(config_peripheral->spi_sercom), mosi_buffer, sizeof(mosi_buffer), miso_buffer, sizeof(miso_buffer));
	//standby mode
	uint8_t set_stndby_params[1] = {0x00};
	SendOpCode(0, 0x80, 1, set_stndby_params);
	//set to LoRa packet
	uint8_t set_packet_type_params[1] = {0x01};
	SendOpCode(0, 0x8A, 1, set_packet_type_params);
	//frequency * 2^25 / clock_frequency (8000000)
	uint64_t freq = (uint64_t)(config_peripheral->frequency) * (1 << 25) / 32000000;
	uint8_t set_frequency_params[4] = {(uint8_t)((freq >> 24) & 0xFF), (uint8_t)((freq >> 16) & 0xFF), (uint8_t)((freq >> 8) & 0xFF), (uint8_t)(freq & 0xFF)};
	SendOpCode(0, 0x86, 4, set_frequency_params);
	//power amp configure
	uint8_t set_pa_config_params[4];
	switch(config_peripheral->output_power)
	{
		case OutputPower::dBm14:
			set_pa_config_params[0] = 0x02;
			set_pa_config_params[1] = 0x02;
			break;
		case OutputPower::dBm17:
			set_pa_config_params[0] = 0x02;
			set_pa_config_params[1] = 0x03;
			break;
		case OutputPower::dBm20:
			set_pa_config_params[0] = 0x03;
			set_pa_config_params[1] = 0x05;
			break;
		case OutputPower::dBm22:
			set_pa_config_params[0] = 0x04;
			set_pa_config_params[1] = 0x07;
			break;
	}
	set_pa_config_params[2] = 0x00;
	set_pa_config_params[3] = 0x01;
	SendOpCode(0, 0x95, 4, set_pa_config_params);
	//tx params config (power and ramp time)
	uint8_t set_tx_params[2];
	set_tx_params[0] = 0x16;
	switch(config_peripheral->ramp_time)
	{
		case TxRampTime::SET_RAMP_10U:
			set_tx_params[1] = 0x00;
			break;
		case TxRampTime::SET_RAMP_20U:
			set_tx_params[1] = 0x01;
			break;
		case TxRampTime::SET_RAMP_40U:
			set_tx_params[1] = 0x02;
			break;
		case TxRampTime::SET_RAMP_80U:
			set_tx_params[1] = 0x03;
			break;
		case TxRampTime::SET_RAMP_200U:
			set_tx_params[1] = 0x04;
			break;
		case TxRampTime::SET_RAMP_800U:
			set_tx_params[1] = 0x05;
			break;
		case TxRampTime::SET_RAMP_1700U:
			set_tx_params[1] = 0x06;
			break;
		case TxRampTime::SET_RAMP_3400U:
			set_tx_params[1] = 0x07;
			break;
	}
	SendOpCode(0, 0x8E, 2, set_tx_params);
	//limitation in tx clamping
	volatile uint8_t value = ReadRegister(0x08D8);
	value |= 0x1E;
	WriteRegister(0x8D8, value);
	//set buffer address (tx, rx at beginning)
	uint8_t set_buf_adr_params[2] = {0x00, 0x00};
	SendOpCode(0, 0x8F, 2, set_buf_adr_params);
	//set modulation parameters
	uint8_t set_mod_params[4];
	switch(config_peripheral->spread_factor)
	{
		case SpreadFactor::SF5:
			set_mod_params[0] = 0x05;
			break;
		case SpreadFactor::SF6:
			set_mod_params[0] = 0x06;
			break;
		case SpreadFactor::SF7:
			set_mod_params[0] = 0x07;
			break;
		case SpreadFactor::SF8:
			set_mod_params[0] = 0x08;
			break;
		case SpreadFactor::SF9:
			set_mod_params[0] = 0x09;
			break;
		case SpreadFactor::SF10:
			set_mod_params[0] = 0x0A;
			break;
		case SpreadFactor::SF11:
			set_mod_params[0] = 0x0B;
			break;
	}
	switch(config_peripheral->bandwidth)
	{
		case Bandwidth::LORA_BW_125:
			set_mod_params[1] = 0x04;
			break;
		case Bandwidth::LORA_BW_250:
			set_mod_params[1] = 0x05;
			break;
		case Bandwidth::LORA_BW_500:
			set_mod_params[1] = 0x06;
			break;
	}
	switch(config_peripheral->coding_rate)
	{
		case CodingRate::LORA_CR_4_5:
			set_mod_params[2] = 0x01;
			break;
		case CodingRate::LORA_CR_4_6:
			set_mod_params[2] = 0x02;
			break;
		case CodingRate::LORA_CR_4_7:
			set_mod_params[2] = 0x03;
			break;
		case CodingRate::LORA_CR_4_8:
			set_mod_params[2] = 0x04;
			break;
	}
	set_mod_params[3] = config_peripheral->low_data_rate_opt;
	SendOpCode(0, 0x8B, 4, set_mod_params);
	//modulation quality limitation fix
	volatile uint8_t value2 = ReadRegister(0x0889);
	if(config_peripheral->bandwidth == Bandwidth::LORA_BW_500)
		value2 &= 0xFB;
	else
		value2 |= 0x04;
	WriteRegister(0x0889, value2);
	preamble_symbols = config_peripheral->preamble_symbols;
	crc_on = config_peripheral->crc_on;
	//packet params (sets payload size to 256)
	uint8_t set_packet_params[6] = {(uint8_t)((preamble_symbols >> 8) & 0xFF), (uint8_t)(preamble_symbols & 0xFF), 0x01, 0xFF, crc_on, 0x00};
	SendOpCode(0, 0x8C, 6, set_packet_params); 
	//setup irq  (irq_mask: timeout, CRC_err, rx_done, tx_done; DIO1: tx_done, CRC_err, timeout, rx_done)
	uint8_t set_irq_params[8] = {0b00000010, 0b01000011, 0b00000010, 0b01000011, 0x00, 0x00, 0x00, 0x00};
	SendOpCode(0, 0x08, 8, set_irq_params);
	//lora sync word write
	uint8_t sync_word_params[4];
	sync_word_params[0] = 0x07;
	sync_word_params[1] = 0x40;
	if(config_peripheral->public_network)
	{
		sync_word_params[2] = 0x34;
		sync_word_params[3] = 0x44; 
	} else {
		sync_word_params[2] = 0x14;
		sync_word_params[3] = 0x24;
	}
	SendOpCode(0, 0x0D, 4, sync_word_params);	
}

void LoRa::LoRaController::Teardown(void)
{
	ResetLoRa();
	//standby mode
	uint8_t set_stndby_params[1] = {0x00};
	SendOpCode(0, 0x80, 1, set_stndby_params);
	spi_control.Deinit();
	ClearRxBuffer();
}

void LoRa::LoRaController::ISR(void)
{
	spi_control.ISR();
}

uint8_t LoRa::LoRaController::SendOpCode(uint32_t num_read, uint8_t command, uint32_t num_params, uint8_t * params)
{
	char spi_bytes[num_params + 1];
	spi_bytes[0] = command;
	for(uint32_t i = 0; i < num_params; i++)
	{
		spi_bytes[i+1] = params[i];
	}
	//wait until busy pin is low
	while(SERCOMHAL::GetPinState(busy_pin));
	return spi_control.SPIHostProcedure(num_params + 1, spi_bytes, num_read);
}

bool LoRa::LoRaController::ReadSPI(char * output)
{
	return spi_control.Receive(output);
}

void LoRa::LoRaController::WriteRegister(uint16_t address, uint8_t value)
{
	uint8_t write_to_reg_params[3] = {(uint8_t)((address >> 8) & 0xFF), (uint8_t)(address & 0xFF), value};
	SendOpCode(0, 0x0D, 3, write_to_reg_params);
}

uint8_t LoRa::LoRaController::ReadRegister(uint16_t address)
{
	uint8_t read_register_params[2] = {(uint8_t)((address >> 8) & 0xFF), (uint8_t)(address & 0xFF)};
	SendOpCode(2, 0x1D, 2, read_register_params);
	//one status bit read before register reading begins
	spi_control.Receive();
	char output;
	spi_control.Receive(&output);
	return output;
}

uint16_t LoRa::LoRaController::GetIRQStatus(void)
{
	//get irqs
	SendOpCode(3, 0x12);
	spi_control.Receive();
	uint16_t output = 0;
	char fragment;
	//convert bytes to unsigned short value 
	for(int i = 0; i < 2; i++)
	{
		spi_control.Receive(&fragment);
		output = (output << 8) | fragment;
	}
	//clear all IRQs
	uint8_t clear_irq_params[2] = {0xFF, 0xFF};
	SendOpCode(0, 0x02, 2, clear_irq_params);
	return output;
}


bool LoRa::LoRaController::SetTxMode(uint8_t payload, uint32_t timeout)
{
	//packet params (sets payload size to size)
	uint8_t set_packet_params[6] = {(uint8_t)((preamble_symbols >> 8) & 0xFF), (uint8_t)(preamble_symbols & 0xFF), 0x00, payload, crc_on, 0x00};
	SendOpCode(0, 0x8C, 6, set_packet_params);
	//begin tx mode
	uint8_t tx_mode_params[3] = {(uint8_t)((timeout >> 16) & 0xFF), (uint8_t)((timeout >> 8) & 0xFF), (uint8_t)(timeout & 0xFF)};
	SendOpCode(0, 0x83, 3, tx_mode_params);
	//wait on irq pin
	while(!SERCOMHAL::GetPinState(irq_pin));
	//get irq bytes and clear irq
	uint16_t irq_status = GetIRQStatus();
	if(irq_status & 0x1)
		return true;
	return false;
}

void LoRa::LoRaController::WriteBuffer(uint8_t payload, uint8_t * payload_bytes)
{
	//first char is 0 to account for buffer offset (0 = write to beginning)
	uint8_t write_buffer_params[256];
	write_buffer_params[0] = 0;
	for(uint32_t i = 0; i < payload; i++)
	{
		write_buffer_params[i+1] = payload_bytes[i];
	}
	SendOpCode(0, 0x0E, payload + 1, write_buffer_params);
}

void LoRa::LoRaController::ReadBuffer(uint8_t payload)
{
	//0 added to params for pointer offset (start at beginning)
	uint8_t params[1] = {0};
	SendOpCode(payload + 1, 0x1E, 1, params);
	spi_control.Receive();
	char read_data;
	//put all received payload into generic receive serial buffer
	for(uint32_t i = 0; i < payload; i++)
	{
		spi_control.Receive(&read_data);
		received_data.Put(read_data, &(Serial::NoIntEnable));
	}
}

void LoRa::LoRaController::ResetLoRa(void)
{
	SERCOMHAL::OutputLow(reset_pin);
	for(uint32_t i = 0; i < 5000; i++);
	SERCOMHAL::OutputHigh(reset_pin);
}

bool LoRa::LoRaController::Transmit(char input, uint32_t timeout)
{
	//standby mode
	uint8_t set_stndby_params[1] = {0x00};
	SendOpCode(0, 0x80, 1, set_stndby_params);
	//write to data buffer
	uint8_t write_buffer_params[2] = {0x00, input};
	SendOpCode(0, 0x0E, 2, write_buffer_params);
	//begin tx mode
	return SetTxMode(1, timeout);
}

bool LoRa::LoRaController::TransmitString(const char * input, uint32_t timeout)
{
	//standby mode
	uint8_t set_stndby_params[1] = {0x00};
	SendOpCode(0, 0x80, 1, set_stndby_params);
	//write to data buffer
	uint8_t i = 0;
	uint8_t write_buffer_params[256];
	write_buffer_params[0] = 0x00;
	while(input[i] != '\0' && i < 255)
	{
		write_buffer_params[i+1] = input[i];
		i++;
	}
	SendOpCode(0, 0x0E, i + 1, write_buffer_params);
	return SetTxMode(i, timeout);
	
}

bool LoRa::LoRaController::TransmitPacket(const char * input, uint8_t num_bytes, uint32_t timeout)
{
	//standby mode
	uint8_t set_stndby_params[1] = {0x00};
	SendOpCode(0, 0x80, 1, set_stndby_params);
	WriteBuffer(num_bytes, (uint8_t *)input);
	//write to data buffer
	return SetTxMode(num_bytes, timeout);
}

bool LoRa::LoRaController::TransmitInt(uint32_t input, uint32_t timeout)
{
	char packet[10];
	uint32_t numel = Serial::Int2ASCII(input, &(packet));
	return TransmitPacket(packet, numel, timeout);
}

bool LoRa::LoRaController::ReceiveSingle(uint32_t timeout, bool * timeout_status)
{
	//standby mode
	uint8_t set_stndby_params[1] = {0x00};
	SendOpCode(0, 0x80, 1, set_stndby_params);
	//packet params (sets payload size to max)
	uint8_t set_packet_params[6] = {(uint8_t)((preamble_symbols >> 8) & 0xFF), (uint8_t)(preamble_symbols & 0xFF), 0x00, 0xFF, crc_on, 0x00};
	SendOpCode(0, 0x8C, 6, set_packet_params); 
	//begin rx mode
	uint8_t rx_mode_params[3] = {(uint8_t)((timeout >> 16) & 0xFF), (uint8_t)((timeout >> 8) & 0xFF), (uint8_t)(timeout & 0xFF)};
	//prevents continuous mode
	if(timeout > 0x00FFFFFE) rx_mode_params[2] = 0xFE;
	SendOpCode(0, 0x82, 3, rx_mode_params);
	//wait on irq and clear
	while(!SERCOMHAL::GetPinState(irq_pin));
	uint16_t irq_status = GetIRQStatus();
	bool result = false;
	if((irq_status >> 1) & 0x1)
	{
		//read data buffer and check valid CRC
		if((irq_status >> 6) & 0x1)
		{
			if(timeout_status != nullptr) *timeout_status = false;	
		} else {
			//get payload length and location on valid crc
			SendOpCode(3, 0x13);
			char payload_size;
			spi_control.Receive();
			spi_control.Receive(&payload_size);
			spi_control.Receive();
			ReadBuffer((uint8_t)payload_size);
			result = true;
		}
	} 
	else if((irq_status >> 9) & 0x1)
	{
		if(timeout_status != nullptr) *timeout_status = true;		
	}
	return result;
}

void LoRa::LoRaController::ResetRxBuffer(char * rx_buf, uint32_t rx_buf_size)
{
	received_data.Reset(rx_buf, rx_buf_size);
}

void LoRa::LoRaController::ClearRxBuffer(void)
{
	received_data.Clear();
}

bool LoRa::LoRaController::ReadRxChar(char * output)
{
	return received_data.Get(&(Serial::NoIntEnable), output);
}

bool LoRa::LoRaController::ReadRxString(const char * input, uint32_t shift, bool move_pointer)
{
	return received_data.GetString(input, shift, move_pointer, &(Serial::NoIntEnable));
}

bool LoRa::LoRaController::ReadRxASCIIInt(uint32_t * output)
{
	return received_data.GetASCIIAsInt(output, &(Serial::NoIntEnable));
}

bool LoRa::LoRaController::ReadRxParam(uint32_t * output, const char *input, char delimiter, uint8_t max_digits)
{
	return received_data.GetIntParam(output, input, delimiter, max_digits, &(Serial::NoIntEnable));
}

uint32_t LoRa::LoRaController::GetRxAvailable(void) const
{
	return received_data.GetBufferAvailable();
}