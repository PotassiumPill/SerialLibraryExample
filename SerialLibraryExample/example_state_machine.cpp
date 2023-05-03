/*
 * Name			:	example_state_machine.cpp
 * Created		:	05/02/2023 14:55:51
 * Author		:	aaron
 * Description	:	State, event, action, and function definitions for your state machine.
 */


#include "example_state_machine.h"

//Add your public vars----------------
char RX_BUFFER[RX_BUFFER_SIZE];
SerialUSB::USBController usb_controller;

uint32_t param;
bool connected = false;

char lora_rx[1];
LoRa::LoRaController lora_controller;


//state machine struct getter
void ExampleStateMachine::GetExampleStateMachine(StateMachine::STT_MACHINE * state_machine)
{
	state_machine->current_state = STT_STATE::DISABLED;
	state_machine->state_actions[STT_STATE::DISABLED] = &DisabledStateAction;
	state_machine->state_actions[STT_STATE::INITIALIZING] = &InitializingStateAction;
	state_machine->state_actions[STT_STATE::DISCONNECTED] = &DisconnectedStateAction;
	state_machine->state_actions[STT_STATE::PROMPT_USER] = &PromptUserStateAction;
	state_machine->state_actions[STT_STATE::WAIT_FOR_RESPONSE] = &WaitForResponseStateAction;
	state_machine->state_actions[STT_STATE::LORA_HW] = &LoraHwStateAction;
	state_machine->state_actions[STT_STATE::LORA_INT] = &LoraIntStateAction;
}

//state action functions
StateMachine::STT_STATE ExampleStateMachine::DisabledStateAction(void)
{
	//do nothing
	return STT_STATE::INITIALIZING;
}

StateMachine::STT_STATE ExampleStateMachine::InitializingStateAction(void)
{
	//TODO: CHANGE CLOCK INITIALIZATION TO FIT HARDWARE
	// Switch CPU clock source to 32 kHz ultra low power oscillator
	Util::enterCriticalSection();
	GCLK->GENCTRL.reg = GCLK_GENCTRL_SRC(GCLK_SOURCE_OSCULP32K) | GCLK_GENCTRL_GENEN;  // Change generator 0 source
	while(GCLK->STATUS.bit.SYNCBUSY);                                  // Wait for write to complete

	// 8 MHz internal oscillator setup
	SYSCTRL->OSC8M.bit.ENABLE = 0x00u;                                 // Disable oscillator
	while(SYSCTRL->OSC8M.bit.ENABLE);                                  // Wait for oscillator to stop
	SYSCTRL->OSC8M.bit.PRESC = 0x00u;                                  // Set options
	SYSCTRL->OSC8M.bit.ENABLE = 0x01u;                                 // Enable oscillator
	while(!(SYSCTRL->OSC8M.bit.ENABLE));                               // Wait for oscillator to become ready

	// Set Generic Clock Generator 0 to use the 8 MHz oscillator
	GCLK->GENCTRL.reg = GCLK_GENCTRL_ID(0) | GCLK_GENCTRL_SRC(GCLK_SOURCE_OSC8M) | GCLK_GENCTRL_GENEN;  // Change generator 0 source
	while(GCLK->STATUS.bit.SYNCBUSY);                                  // Wait for write to complete
	Util::exitCriticalSection();
	
	//init
	#ifdef USING_LORA
	SPISAMD21::SetGenClk((SERCOMSAMD21::GenericClock)STT_SPI_GENCLK);
	LoRa::Config com3_lora_config;
	LoRa::GetConfigDefaults(&com3_lora_config);
	SPISAMD21::GetPeripheralDefaults(&(com3_lora_config.spi_sercom), SERCOMSAMD21::SercomID::Sercom3);
	com3_lora_config.spi_sercom.ssl_pin = (SERCOMHAL::Pinout){0,0,7};
	com3_lora_config.busy_pin = (SERCOMHAL::Pinout){0, 0, 17};
	com3_lora_config.irq_pin = (SERCOMHAL::Pinout){0, 0, 4};
	com3_lora_config.reset_pin = (SERCOMHAL::Pinout){0, 1, 2};
	lora_controller.Init(&com3_lora_config, lora_rx, sizeof(lora_rx));
	#endif
	
	usb_controller.Init(RX_BUFFER, sizeof(RX_BUFFER));
	usb_controller.Task();
	
	return STT_STATE::DISCONNECTED;
}

StateMachine::STT_STATE ExampleStateMachine::DisconnectedStateAction(void)
{
	StateMachine::STT_STATE current_state = STT_STATE::DISCONNECTED;
	usb_controller.Task();
	if(TurnOn())
	{
		usb_controller.TransmitString("Received on command! Turning on...\n\n");
		current_state = STT_STATE::PROMPT_USER;
	}
	StateMachine::ProcessSuperState(&current_state, STT_STATE::SUPER, &SuperStateAction);
	return current_state;
}

StateMachine::STT_STATE ExampleStateMachine::PromptUserStateAction(void)
{
	usb_controller.Task();
	usb_controller.TransmitString("Send a command to test it out!\n\n");\
	usb_controller.TransmitString("Command list:\n");
	usb_controller.TransmitString("\"lora_hello_world\" -> sends hello world through LoRa radio.\n");
	usb_controller.TransmitString("\"lora_integer_#\" -> sends an ASCII integer through LoRa radio (replace # with a valid integer.\n");
	usb_controller.TransmitString("\"off\" -> turn off.\n");
	return STT_STATE::WAIT_FOR_RESPONSE;
}

StateMachine::STT_STATE ExampleStateMachine::WaitForResponseStateAction(void)
{
	StateMachine::STT_STATE current_state = STT_STATE::WAIT_FOR_RESPONSE;
	usb_controller.Task();
	if(TxLoraHw())
	{
		current_state = STT_STATE::LORA_HW;
	}
	else if(TxLoraInt())
	{
		current_state = STT_STATE::LORA_INT;
	}
	StateMachine::ProcessSuperState(&current_state, STT_STATE::SUPER, &SuperStateAction);
	return current_state;
}

StateMachine::STT_STATE ExampleStateMachine::LoraHwStateAction(void)
{
	usb_controller.Task();
	#ifdef USING_LORA
	if(lora_controller.TransmitString("hello world",TIMEOUT))
	{
		usb_controller.Task();
		usb_controller.TransmitString("Transmitted hello world through LoRa!\n");
	}
	else
	{
		usb_controller.Task();
		usb_controller.TransmitString("LoRa timed out!\n");
	}
	#else
	usb_controller.TransmitString("Not using LoRa!\n");
	#endif
	return STT_STATE::WAIT_FOR_RESPONSE;
}

StateMachine::STT_STATE ExampleStateMachine::LoraIntStateAction(void)
{
	usb_controller.Task();
	#ifdef USING_LORA
	if(lora_controller.TransmitInt(param,TIMEOUT))
	{
		usb_controller.Task();
		usb_controller.TransmitString("Transmitted ");
		usb_controller.TransmitInt(param);
		usb_controller.TransmitString("through LoRa!\n");
	}
	else
	{
		usb_controller.Task();
		usb_controller.TransmitString("LoRa timed out!\n");
	}
	#else
	usb_controller.TransmitString("Not using LoRa! Integer param: ");
	usb_controller.TransmitInt(param);
	usb_controller.Transmit('\n');
	#endif
	return STT_STATE::WAIT_FOR_RESPONSE;
}

StateMachine::STT_STATE ExampleStateMachine::SuperStateAction(void)
{
	usb_controller.Task();
	if(TurnOff())
	{
		usb_controller.TransmitString("Received off command! Turning off...\n");
		return STT_STATE::DISCONNECTED;
	}
	else if(Unplugged())
	{
		return STT_STATE::DISCONNECTED;
	}
	else if(PluggedIn())
	{
		usb_controller.TransmitString("USB Plugged in!\n");
		return STT_STATE::PROMPT_USER;
	}
	return STT_STATE::SUPER;
}

//common event functions

bool ExampleStateMachine::TurnOn(void)
{
	return usb_controller.ReceiveString("on");
}

bool ExampleStateMachine::Unplugged(void)
{
	bool unplugged =  connected && !usb_controller.IsConnected();
	if(unplugged)
		connected = false;
	return unplugged;
}

bool ExampleStateMachine::TxLoraHw(void)
{
	return usb_controller.ReceiveString("lora_hello_world");
}

bool ExampleStateMachine::TurnOff(void)
{
	return usb_controller.ReceiveString("off");
}

bool ExampleStateMachine::PluggedIn(void)
{
	bool plugged_in = !connected && usb_controller.IsConnected();
	if(plugged_in)
		connected = true;
	return plugged_in;
}

bool ExampleStateMachine::TxLoraInt(void)
{
	return usb_controller.ReceiveParam(&param, "lora_integer_");
}

//common action functions

void SERCOM3_Handler(void)
{
	lora_controller.ISR();
}

void USB_Handler(void)
{
	usb_controller.ISR();
}

