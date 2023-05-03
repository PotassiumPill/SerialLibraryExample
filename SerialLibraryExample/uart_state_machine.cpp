/*
 * Name			:	uart_state_machine.cpp
 * Created		:	05/03/2023 12:38:10
 * Author		:	Aaron Reilman
 * Description	:	State, event, action, and function definitions for your state machine.
 */


#include "uart_state_machine.h"

//Add your public vars----------------
char RX_BUFFER[UART_RX_SIZE];
char TX_BUFFER[UART_TX_SIZE];
SerialUART::UARTController uart_controller;

uint32_t param;
bool connected = false;

char lora_rx[1];
LoRa::LoRaController lora_controller;

//state machine struct getter
void UartStateMachine::GetUartStateMachine(StateMachine::STT_MACHINE * state_machine)
{
	state_machine->current_state = STT_STATE::DISABLED;
	state_machine->state_actions[STT_STATE::DISABLED] = &DisabledStateAction;
	state_machine->state_actions[STT_STATE::INITIALIZING] = &InitializingStateAction;
	state_machine->state_actions[STT_STATE::OFF_STATE] = &OffStateStateAction;
	state_machine->state_actions[STT_STATE::PROMPT_USER] = &PromptUserStateAction;
	state_machine->state_actions[STT_STATE::WAIT_FOR_RESPONSE] = &WaitForResponseStateAction;
	state_machine->state_actions[STT_STATE::LORA_HW] = &LoraHwStateAction;
	state_machine->state_actions[STT_STATE::LORA_INT] = &LoraIntStateAction;
}

//state action functions
StateMachine::STT_STATE UartStateMachine::DisabledStateAction(void)
{
	//do nothing
	return STT_STATE::INITIALIZING;
}

StateMachine::STT_STATE UartStateMachine::InitializingStateAction(void)
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
	
	UARTHAL::Peripheral uart_peripheral;
	UARTHAL::GetPeripheralDefaults(&uart_peripheral);
	uart_controller.Init(&uart_peripheral, TX_BUFFER, sizeof(TX_BUFFER), RX_BUFFER, sizeof(RX_BUFFER));
	
	return STT_STATE::OFF_STATE;
}

StateMachine::STT_STATE UartStateMachine::OffStateStateAction(void)
{
	StateMachine::STT_STATE current_state = STT_STATE::OFF_STATE;
	/*
	 * TODO: implement actions for Off State State
	 */
	if(TurnOn())
	{
		uart_controller.TransmitString("Received on command! Turning on...\n\n");
		current_state = STT_STATE::PROMPT_USER;
	}
	StateMachine::ProcessSuperState(&current_state, STT_STATE::SUPER, &SuperStateAction);
	return current_state;
}

StateMachine::STT_STATE UartStateMachine::PromptUserStateAction(void)
{
	uart_controller.TransmitString("Send a command to test it out!\n\n");\
	uart_controller.TransmitString("Command list:\n");
	uart_controller.TransmitString("\"lora_hello_world\" -> sends hello world through LoRa radio.\n");
	uart_controller.TransmitString("\"lora_integer_#\" -> sends an ASCII integer through LoRa radio (replace # with a valid integer.\n");
	uart_controller.TransmitString("\"off\" -> turn off.\n");
	return STT_STATE::WAIT_FOR_RESPONSE;
}

StateMachine::STT_STATE UartStateMachine::WaitForResponseStateAction(void)
{
	StateMachine::STT_STATE current_state = STT_STATE::WAIT_FOR_RESPONSE;
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

StateMachine::STT_STATE UartStateMachine::LoraHwStateAction(void)
{
	#ifdef USING_LORA
	if(lora_controller.TransmitString("hello world",TIMEOUT))
	{
		uart_controller.TransmitString("Transmitted hello world through LoRa!\n");
	}
	else
	{
		uart_controller.TransmitString("LoRa timed out!\n");
	}
	#else
	uart_controller.TransmitString("Not using LoRa!\n");
	#endif
	return STT_STATE::WAIT_FOR_RESPONSE;
}

StateMachine::STT_STATE UartStateMachine::LoraIntStateAction(void)
{
	#ifdef USING_LORA
	if(lora_controller.TransmitInt(param,TIMEOUT))
	{
		uart_controller.TransmitString("Transmitted ");
		uart_controller.TransmitInt(param);
		uart_controller.TransmitString("through LoRa!\n");
	}
	else
	{
		uart_controller.TransmitString("LoRa timed out!\n");
	}
	#else
	uart_controller.TransmitString("Not using LoRa! Integer param: ");
	uart_controller.TransmitInt(param);
	uart_controller.Transmit('\n');
	#endif
	return STT_STATE::WAIT_FOR_RESPONSE;
}

StateMachine::STT_STATE UartStateMachine::SuperStateAction(void)
{
	if(TurnOff())
	{
		uart_controller.TransmitString("Received off command! Turning off...\n");
		return STT_STATE::OFF_STATE;
	}
	return STT_STATE::SUPER;
}

//common event functions

bool UartStateMachine::TxLoraHw(void)
{
	return uart_controller.ReceiveString("lora_hello_world");
}

bool UartStateMachine::TxLoraInt(void)
{
	return uart_controller.ReceiveParam(&param, "lora_integer_");
}

bool UartStateMachine::TurnOff(void)
{
	/*
	 * TODO: implement Turn Off Event
	 */
	return true;
}

bool UartStateMachine::TurnOn(void)
{
	return uart_controller.ReceiveString("on");
}

//common action functions

void SERCOM0_Handler(void)
{
	uart_controller.ISR();
}

