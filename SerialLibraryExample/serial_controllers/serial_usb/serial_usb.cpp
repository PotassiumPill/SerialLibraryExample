/* 
 * Name				:	serial_usb.cpp
 * Created			:	8/12/2022 10:53:43 AM
 * Author			:	Aaron Reilman
 * Description		:	A USB serial communication driver.
 */


#include "serial_usb/serial_usb.h"

//Definition of Controller Class
SerialUSB::USBController::USBController(void)
{
	usb_on = false;
	detached = true;
	//ResetUSB();
}

SerialUSB::USBController::~USBController(void)
{
	Deinit();
}

void SerialUSB::USBController::Init(char * buf, uint32_t buf_size)
{
	if(!usb_on)
	{
		ResetBuffer(buf, buf_size);
		USBFeedIOClocks();
		usb_on = true;
		if(tud_inited())
			Reattach();
		else
			tusb_init();	//tiny usb init (tusb_config.h must be configured to device mode, 1 CDC port and using the correct hardware environment)
	}
}

void SerialUSB::USBController::Deinit(void)
{
	if(usb_on)
	{
		//no teardown available for tiny usb stack
		ClearBuffers();
		Detach();
		usb_on = false;
	}
}

void SerialUSB::USBController::ResetBuffer(char * buf, uint32_t buf_size)
{
	usb_buffer.Reset(buf, buf_size);
}

void SerialUSB::USBController::ClearBuffers(bool clear_tx, bool clear_rx)
{
	if(clear_tx) tud_cdc_write_clear();
	if(clear_rx) usb_buffer.Clear();
}

void SerialUSB::USBController::ISR(void)
{
	#if (CFG_TUSB_MCU != OPT_MCU_NONE)
	tud_int_handler(0);
	#endif
}

void SerialUSB::USBController::Task(bool echo)
{
	if(usb_on)
	{
		if(tud_cdc_available() && usb_buffer.GetBufferState() != GenericBuffer::BufferState::Full)
		{
			char packet[usb_buffer.GetBufferEmpty()];
			uint32_t count = tud_cdc_read(packet, usb_buffer.GetBufferEmpty());
			if(echo)
			{
				tud_cdc_write(packet, count);
				tud_cdc_write_flush();
			} else {
				for(uint32_t i = 0; i < count; i++) PutBuffer(packet[i]);
			}
		}
		tud_task();
	}
}

bool SerialUSB::USBController::Transmit(char input)
{
	bool success = false;
	if(usb_on) success = tud_cdc_write_char(input);
	return success;
}

bool SerialUSB::USBController::Receive(char * output)
{
	return usb_buffer.Get(&(Serial::NoIntEnable), output);
}

bool SerialUSB::USBController::TransmitPacket(const char * input, uint32_t num_bytes)
{
	bool success = false;
	if(usb_on)
	{
		uint32_t count = tud_cdc_write(input, num_bytes);
		tud_cdc_write_flush();
		success = count == num_bytes;
	}
	return success;
}

uint32_t SerialUSB::USBController::TransmitString(const char *input)
{
	uint32_t count = 0;
	if(usb_on)
	{
		count = tud_cdc_write_str(input);
		tud_cdc_write_flush();
	}
	return count;
}

bool SerialUSB::USBController::ReceiveString(const char *input, uint32_t shift, bool move_pointer)
{
	return usb_buffer.GetString(input, shift, move_pointer, &(Serial::NoIntEnable));
}

bool SerialUSB::USBController::TransmitInt(uint32_t input)
{
	char packet[10];
	uint32_t numel = Serial::Int2ASCII(input, &(packet));
	return TransmitPacket(packet, numel);
}

bool SerialUSB::USBController::ReceiveInt(uint32_t * output)
{
	return usb_buffer.GetASCIIAsInt(output, &(Serial::NoIntEnable));
}

bool SerialUSB::USBController::ReceiveParam(uint32_t * output, const char *input, char delimiter, uint8_t max_digits)
{
	return usb_buffer.GetIntParam(output, input, delimiter, max_digits, &(Serial::NoIntEnable));
}

//private helper function
void SerialUSB::USBController::PutBuffer(char input)
{
	usb_buffer.Put(input, &(Serial::NoIntEnable));
}

//getters
uint32_t SerialUSB::USBController::GetBufferAvailable(void) const
{
	return usb_buffer.GetBufferAvailable();
}

bool SerialUSB::USBController::IsConnected(void) const
{
	return usb_on && tud_mounted();
}

void SerialUSB::USBController::Detach(void)
{
	if(usb_on && !detached)
	{
		tud_disconnect();
		detached = true;
	}
}

void SerialUSB::USBController::Reattach(void)
{
	if(usb_on && detached)
	{
		tud_connect();
		detached = false;
	}
}
