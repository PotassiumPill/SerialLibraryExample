/* 
 * Name			:	serial_usb.cpp
 * Created		:	8/12/2022 10:53:43 AM
 * Author		:	Aaron Reilman
 * Description	:	A USB serial communication driver.
 */


#include "serial_usb/serial_usb.h"

#if CFG_TUSB_MCU == OPT_MCU_SAMD21
#include "sam.h"

void SerialUSB::USBFeedIOClocks(void)
{
	//preset: 8MHz osc set and enabled, fed into clock generator 0
	NVMCTRL->CTRLB.bit.RWS = 1;
	
	while(!SYSCTRL->PCLKSR.bit.DFLLRDY);
	SYSCTRL->DFLLCTRL.reg = SYSCTRL_DFLLCTRL_ENABLE;
	while(!SYSCTRL->PCLKSR.bit.DFLLRDY);
	
	uint32_t coarse =
		((*(uint32_t*)FUSES_DFLL48M_COARSE_CAL_ADDR) & FUSES_DFLL48M_COARSE_CAL_Msk) >> FUSES_DFLL48M_COARSE_CAL_Pos;
	uint32_t fine =
		((*(uint32_t*)FUSES_DFLL48M_FINE_CAL_ADDR) & FUSES_DFLL48M_FINE_CAL_Msk) >> FUSES_DFLL48M_FINE_CAL_Pos;
	SYSCTRL->DFLLVAL.reg = SYSCTRL_DFLLVAL_COARSE(coarse) | SYSCTRL_DFLLVAL_FINE(fine);
	while (!SYSCTRL->PCLKSR.bit.DFLLRDY);
	
	SYSCTRL->DFLLCTRL.reg |= SYSCTRL_DFLLCTRL_USBCRM | SYSCTRL_DFLLCTRL_CCDIS;
	SYSCTRL->DFLLMUL.reg =  SYSCTRL_DFLLMUL_MUL(48000) | SYSCTRL_DFLLMUL_FSTEP(1) | SYSCTRL_DFLLMUL_CSTEP(1);
	
	/* Closed loop mode */
	SYSCTRL->DFLLCTRL.bit.MODE = 1;
	/* Enable the DFLL */
	SYSCTRL->DFLLCTRL.bit.ENABLE = 1;
	while (!SYSCTRL->PCLKSR.bit.DFLLRDY);
	
	GCLK->GENCTRL.reg = GCLK_GENCTRL_ID(0) | GCLK_GENCTRL_SRC(GCLK_SOURCE_DFLL48M) | GCLK_GENCTRL_IDC | GCLK_GENCTRL_GENEN;
	while(GCLK->STATUS.bit.SYNCBUSY);

	PM->APBBMASK.bit.USB_ = 0x1u;
	PM->AHBMASK.bit.USB_ = 0x1u;
	GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK0 | GCLK_CLKCTRL_ID_USB;
	while (GCLK->STATUS.bit.SYNCBUSY);
	PORT->Group[0].DIRSET.reg = 0x1u << 23u;
	PORT->Group[0].DIRSET.reg = 0x1u << 24u;
	PORT->Group[0].DIRSET.reg = 0x1u << 25u;
	PORT->Group[0].PMUX[11].reg = PORT_PMUX_PMUXO(6u);
	PORT->Group[0].PINCFG[23].reg = PORT_PINCFG_PMUXEN;
	PORT->Group[0].PMUX[12].reg = PORT_PMUX_PMUXO(6u) | PORT_PMUX_PMUXE(6u);
	PORT->Group[0].PINCFG[24].reg = PORT_PINCFG_PMUXEN;
	PORT->Group[0].PINCFG[25].reg = PORT_PINCFG_PMUXEN;
}

void SerialUSB::ResetUSB(void)
{
	USB->DEVICE.CTRLA.bit.SWRST = 0x1;
	while(!USB->DEVICE.SYNCBUSY.bit.SWRST);
	while(USB->DEVICE.SYNCBUSY.bit.SWRST);
}
#else
void SerialUSB::USBFeedIOClocks(void) {}
void SerialUSB::ResetUSB(void) {}
#endif

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
	bool received = false;
	if(usb_on) received = usb_buffer.Get(&(Serial::NoIntEnable), output);
	return received;
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
