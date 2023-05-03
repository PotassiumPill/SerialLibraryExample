/* 
 * Name			:	serial_buffer.cpp
 * Created		:	8/18/2022 12:49:40 PM
 * Author		:	Aaron Reilman
 * Description	:	A generic FIFO-like buffer for high level serial communication.
 */


#include "serial_buffer/serial_buffer.h"

//Definition of blank interrupt enable
void Serial::NoIntEnable(SERCOMHAL::SercomID peripheral_id, bool enable)
{
	(void) peripheral_id;
	(void) enable;
}
//Definition of Integer to ASCII converter
uint32_t Serial::Int2ASCII(uint32_t num, char (*result)[10])
{
	volatile uint32_t temp = num;
	uint32_t multiplier = 1u;
	uint8_t numel = 1u;
	//find number of digits
	while(temp / multiplier > 9u)
	{
		multiplier *= 10u;
		numel++;
	}
	//convert each digit to ASCII and add to buffer
	uint8_t i = 0u;
	while(i < numel)
	{
		(*result)[i] = temp / multiplier + 48u;
		temp %= multiplier;
		multiplier /= 10u;
		i++;
	}
	return numel;
}

//Definition of Buffer Class
Serial::SerialBuffer::SerialBuffer(SERCOMHAL::SercomID peripheral_id, char * buf, uint32_t buf_size)
{
	sercom_id = peripheral_id;
	Reset(buf, buf_size);
}

Serial::SerialBuffer::~SerialBuffer(void)
{
	Clear();
}

void Serial::SerialBuffer::Reset(char * buf, uint32_t buf_size)
{
	buffer.ResetBuffer(buf, buf_size);
	index_shift = 1u;
}

void Serial::SerialBuffer::Clear(void)
{
	buffer.Clear();
}

bool Serial::SerialBuffer::Put(char input, void (* int_func)(uint8_t, bool))
{
	int_func(sercom_id, false);
	bool success = buffer.Put(input);
	int_func(sercom_id, true);
	return success;
}

bool Serial::SerialBuffer::Get(void (* int_func)(uint8_t, bool), char * output)
{
	int_func(sercom_id, false);
	bool result = buffer.Get(output);
	if(result && index_shift) index_shift--;
	int_func(sercom_id, true);
	return result;
}

bool Serial::SerialBuffer::GetString(const char *input, uint32_t shift, bool move_pointer, void (* int_func)(uint8_t, bool))
{
	bool has_string = false;
	//prevents checking after initialization or a previous string has been found until new character is received
	while(Get(int_func));
	if(!index_shift)
	{
		has_string = true;
		uint32_t numel = 0;
		//finds size of input string by detecting null character
		while(input[numel] != '\0') numel++;
		uint32_t i = 0;
		uint32_t rd_index;
		uint32_t bsize = buffer.GetSize();
		char * arr_ref = buffer.GetRawElements(&rd_index);
		uint32_t reader = rd_index + ((shift > rd_index) * bsize) - shift;
		while(has_string && i < numel)
		{
			//iterates backwards in buffer and input string and checks if each element is equal to each other
			if(reader == 0u) reader = bsize - 1;
			else reader--;
			i++;
			has_string = (input[numel - i] == arr_ref[reader]);
		}
		if(has_string)
		{
			index_shift = 1u;
			if(move_pointer && shift > 0u)
			{
				//moves buffer back by the shift amount if string has been found
				int_func(sercom_id, false);
				buffer.ShiftReadPointer(shift, false);
				index_shift += shift;
				int_func(sercom_id, true);
			}
		}
	}
	return has_string;
}

bool Serial::SerialBuffer::GetASCIIAsInt(uint32_t * output, void (* int_func)(uint8_t, bool))
{
	bool result = false;
	*output = 0u;
	unsigned char received = buffer.Peek();
	while(received > 47u && received < 58u)
	{
		result = true;
		Get(int_func);
		*output *= 10u;
		*output += received - 48u;
		received = buffer.Peek();
	}
	return result;
}

bool Serial::SerialBuffer::GetIntParam(uint32_t * output, const char *input, char delimiter, uint8_t max_digits, void (* int_func)(uint8_t, bool))
{
	bool result = false;
	uint8_t i = 1u;
	while(!GetString(input, i, true, int_func) && i < max_digits+1u) i++;
	if(i<max_digits+1u)
	{
		volatile uint32_t temp_index;
		uint32_t rd_index;
		uint32_t bsize = buffer.GetSize();
		char * arr_ref = buffer.GetRawElements(&rd_index);
		if(rd_index == 0u) temp_index = bsize - 1;
		else temp_index = rd_index - 1u;
		if(GetASCIIAsInt(output, int_func))
		{
			char read_delimiter;
			if(!delimiter || (delimiter && Get(int_func, &read_delimiter) && read_delimiter == delimiter))
			{
				result = true;
				arr_ref[temp_index] = '\0';
			} else {
				*output = '\0';
			}
		}
	}
	return result;
}

void Serial::SerialBuffer::SetSercomID(SERCOMHAL::SercomID peripheral_id)
{
	sercom_id = peripheral_id;
}

SERCOMHAL::SercomID Serial::SerialBuffer::GetSercomID(void) const
{
	return sercom_id;
}

uint32_t Serial::SerialBuffer::GetBufferAvailable(void) const
{
	return buffer.GetBufferAvailable();
}

uint32_t Serial::SerialBuffer::GetBufferEmpty(void) const
{
	return buffer.GetSize() - buffer.GetBufferAvailable();
}

GenericBuffer::BufferState Serial::SerialBuffer::GetBufferState(void) const
{
	return buffer.GetBufferState();
}


