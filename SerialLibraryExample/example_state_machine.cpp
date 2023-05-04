/*
 * Name			:	example_state_machine.cpp
 * Created		:	05/03/2023 15:47:53
 * Author		:	Aaron Reilman
 * Description	:	State, event, action, and function definitions for your state machine.
 */


#include "example_state_machine.h"

//#define USING_UART

//Add your public vars----------------


char RX_BUFFER[RX_BUFFER_SIZE];
#ifdef USING_UART
char TX_BUFFER[TX_BUFFER_SIZE];
SerialUART::UARTController uart_controller;
#else
SerialUSB::USBController usb_controller;
#endif

uint32_t param;
bool connected = false;
bool echo = false;


//state machine struct getter
void ExampleStateMachine::GetExampleStateMachine(StateMachine::STT_MACHINE * state_machine)
{
	state_machine->current_state = STT_STATE::DISABLED;
	state_machine->state_actions[STT_STATE::DISABLED] = &DisabledStateAction;
	state_machine->state_actions[STT_STATE::INITIALIZING] = &InitializingStateAction;
	state_machine->state_actions[STT_STATE::OFF] = &OffStateAction;
	state_machine->state_actions[STT_STATE::PROMPT_USER] = &PromptUserStateAction;
	state_machine->state_actions[STT_STATE::ON] = &OnStateAction;
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
	
	#ifdef USING_UART
	UARTHAL::Peripheral uart_peripheral;
	UARTHAL::GetPeripheralDefaults(&uart_peripheral);
	uart_peripheral.baud_value = (uint32_t)BAUD_RATE;
	uart_controller.Init(&uart_peripheral, TX_BUFFER, sizeof(TX_BUFFER), RX_BUFFER, sizeof(RX_BUFFER));
	#else
	usb_controller.Init(RX_BUFFER, sizeof(RX_BUFFER));
	usb_controller.Task(echo);
	#endif
	
	return STT_STATE::OFF;
}

StateMachine::STT_STATE ExampleStateMachine::OffStateAction(void)
{
	StateMachine::STT_STATE current_state = STT_STATE::OFF;
	#ifndef USING_UART
	usb_controller.Task(echo);
	#endif
	echo = false;
	if(TurnOn())
	{
		#ifdef USING_UART
		uart_controller.TransmitString("On command received! Turning on...\n");
		#else
		usb_controller.TransmitString("On command received! Turning on...\n");
		#endif
		current_state = STT_STATE::PROMPT_USER;
	}
	StateMachine::ProcessSuperState(&current_state, STT_STATE::SUPER, &SuperStateAction);
	return current_state;
}

StateMachine::STT_STATE ExampleStateMachine::PromptUserStateAction(void)
{
	#ifdef USING_UART
	uart_controller.TransmitString("Send strings through terminal to see responses!\n");
	#else
	usb_controller.Task(echo);
	usb_controller.TransmitString("Send strings through terminal to see responses!\n");
	#endif
	return STT_STATE::ON;
}

StateMachine::STT_STATE ExampleStateMachine::OnStateAction(void)
{
	StateMachine::STT_STATE current_state = STT_STATE::ON;
	#ifdef USING_UART
	if(uart_controller.ReceiveString("hello world"))
	{
		uart_controller.TransmitString("World: hello!\n");
	}
	else if(uart_controller.ReceiveString("echo"))
	{
		uart_controller.TransmitString("Received data will now be echoed!\n");
		echo = true;
		uart_controller.TransmitString("(Regular function will cease until unplugged)\n");
	}
	else if(uart_controller.ReceiveParam(&param, "integer_"))
	{
		uart_controller.TransmitString("Integer: ");
		uart_controller.TransmitInt(param);
		uart_controller.Transmit('\n');
	}
	else if(uart_controller.ReceiveParam(&param, "square_",'!'))
	{
		uart_controller.TransmitString("Square value: ");
		uart_controller.TransmitInt(param * param);
		uart_controller.Transmit('\n');
	}
	#else
	usb_controller.Task(echo);
	if(usb_controller.ReceiveString("hello world"))
	{
		usb_controller.TransmitString("World: hello!\n");
	}
	else if(usb_controller.ReceiveString("echo"))
	{
		usb_controller.TransmitString("Received data will now be echoed!\n");
		echo = true;
		usb_controller.Task(echo);
		usb_controller.TransmitString("(Regular function will cease until unplugged)\n");
	}
	else if(usb_controller.ReceiveParam(&param, "integer_"))
	{
		usb_controller.TransmitString("Integer: ");
		usb_controller.TransmitInt(param);
		usb_controller.Transmit('\n');
	}
	else if(usb_controller.ReceiveParam(&param, "square_",'!'))
	{
		usb_controller.TransmitString("Square value: ");
		usb_controller.TransmitInt(param * param);
		usb_controller.Transmit('\n');
	}
	#endif
	if(TurnOff())
	{
		#ifdef USING_UART
		uart_controller.TransmitString("Off command received! Turning off...\n");
		#else
		usb_controller.TransmitString("Off command received! Turning off...\n");
		#endif
		current_state = STT_STATE::OFF;
	}
	StateMachine::ProcessSuperState(&current_state, STT_STATE::SUPER, &SuperStateAction);
	return current_state;
}

StateMachine::STT_STATE ExampleStateMachine::SuperStateAction(void)
{
	#ifndef USING_UART
	usb_controller.Task(echo);
	#endif
	if(Unplugged())
	{
		return STT_STATE::OFF;
	}
	else if(PluggedIn())
	{
		#ifndef USING_UART
		usb_controller.TransmitString("USB plugged in!\n");
		#endif
		return STT_STATE::PROMPT_USER;
	}
	return STT_STATE::SUPER;
}

//common event functions

bool ExampleStateMachine::Unplugged(void)
{
	#ifdef USING_UART
	return false;
	#else
	bool unplugged =  connected && !usb_controller.IsConnected();
	if(unplugged)
		connected = false;
	return unplugged;
	#endif
}

bool ExampleStateMachine::TurnOn(void)
{
	#ifdef USING_UART
	return uart_controller.ReceiveString("on");
	#else
	return usb_controller.ReceiveString("on");
	#endif
}

bool ExampleStateMachine::PluggedIn(void)
{
	#ifdef USING_UART
	return false;
	#else
	bool plugged_in = !connected && usb_controller.IsConnected();
	if(plugged_in)
		connected = true;
	return plugged_in;
	#endif
}

bool ExampleStateMachine::TurnOff(void)
{
	#ifdef USING_UART
	return uart_controller.ReceiveString("off");
	#else
	return usb_controller.ReceiveString("off");
	#endif
}

//common action functions

void USB_Handler(void)
{
	#ifndef USING_UART
	usb_controller.ISR();
	#endif
}

void SERCOM0_Handler(void)
{
	#ifdef USING_UART
	uart_controller.ISR();
	if(echo) uart_controller.EchoRx();
	#endif
}

