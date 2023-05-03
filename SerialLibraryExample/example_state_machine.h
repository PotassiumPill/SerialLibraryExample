/*
 * Name			:	example_state_machine.h
 * Created		:	05/02/2023 14:55:51
 * Author		:	aaron
 * Description	:	State, event, action, and function definitions for your state machine.
 */


#ifndef __EXAMPLE_STATE_MACHINE_H__
#define __EXAMPLE_STATE_MACHINE_H__

#include "state_machine.h"
//Add your includes---------------------
#include "serial_communication.h"
#include "lora_controller.h"
#include "util.h"


//Add your macros-----------------------
#define USING_LORA


#define RX_BUFFER_SIZE		512
#define STT_SPI_GENCLK		3	
#define TIMEOUT				2000


/*!
 * \brief Namespace containing all function and structure definitions to implement your state machine.
 *
 * Contains defined STT_STATEs, state functions, and all other executive functions to be performed in the state machine.
 */
namespace ExampleStateMachine
{
	/*!
	 * \brief Defined enum of STT_STATEs
	 *
	 * Enum of uint8_t for all implemented states.
	 */
	enum STT_STATE : uint8_t {
		DISABLED,				//Entry point for state machine
		INITIALIZING,			//Initializes clocks and controller objects
		DISCONNECTED,			//Off state
		PROMPT_USER,			//Prompts user with commands and intro
		WAIT_FOR_RESPONSE,		//Waits for user response (usb rx)
		LORA_HW,				//Sends hello world via LoRa
		LORA_INT,				//Sends an integer via LoRa
		SUPER					//Super state which checks for off command and connect/disconnect
	};
	/*!
	 * \brief Populates state machine struct with values
	 *
	 * Initializes state machine by populating an initialized struct with states and functions.
	 *
	 * \param state_machine pointer to state machine which will be populated with values
	 */
	void GetExampleStateMachine(StateMachine::STT_MACHINE * state_machine);
	/*!
	 * \brief Disabled State action function (DISABLED)
	 *
	 * Entry point for state machine, which does nothing
	 *
	 * \return state to transition to (INITIALIZING)
	 */
	StateMachine::STT_STATE DisabledStateAction(void);
	/*!
	 * \brief Initializing State action function (INITIALIZING)
	 *
	 * Initializes clocks and controller objects
	 *
	 * \return state to transition to (DISCONNECTED)
	 */
	StateMachine::STT_STATE InitializingStateAction(void);
	/*!
	 * \brief Disconnected State action function (DISCONNECTED)
	 *
	 * Off state, which loops idly except checking for on command
	 *
	 * \return state to transition to (DISCONNECTED, or PROMPT_USER, unless super state transitions out of sub-state)
	 */
	StateMachine::STT_STATE DisconnectedStateAction(void);
	/*!
	 * \brief Prompt User State action function (PROMPT_USER)
	 *
	 * Prompts user with commands and intro
	 *
	 * \return state to transition to (WAIT_FOR_RESPONSE)
	 */
	StateMachine::STT_STATE PromptUserStateAction(void);
	/*!
	 * \brief Wait For Response State action function (WAIT_FOR_RESPONSE)
	 *
	 * Waits for user response through a serial USB connection sent from a terminal
	 *
	 * \return state to transition to (WAIT_FOR_RESPONSE, LORA_HW, or LORA_INT, unless super state transitions out of sub-state)
	 */
	StateMachine::STT_STATE WaitForResponseStateAction(void);
	/*!
	 * \brief Lora Hw State action function (LORA_HW)
	 *
	 * Sends hello world via LoRa
	 *
	 * \return state to transition to (WAIT_FOR_RESPONSE)
	 */
	StateMachine::STT_STATE LoraHwStateAction(void);
	/*!
	 * \brief Lora Int State action function (LORA_INT)
	 *
	 * Sends an ASCII integer given by user via LoRa
	 *
	 * \return state to transition to (WAIT_FOR_RESPONSE)
	 */
	StateMachine::STT_STATE LoraIntStateAction(void);
	/*!
	 * \brief Super State action function (SUPER)
	 *
	 * Super state which encompasses majority of the other sub-states.\n
	 * Checks for off command and connect/disconnect
	 *
	 * \return state to transition to (SUPER, DISCONNECTED, DISCONNECTED, or PROMPT_USER)
	 */
	StateMachine::STT_STATE SuperStateAction(void);

	//common action functions and events----------------
	
	/*!
	 * \brief received "on" via serial USB
	 *
	 * \return true if on command received, false otherwise
	 */
	bool TurnOn(void);
	/*!
	 * \brief USB has transitioned from connected to disconnected
	 *
	 * \return true if USB has just been unplugged, false otherwise
	 */
	bool Unplugged(void);
	/*!
	 * \brief "lora_hello_world" received via serial USB
	 *
	 * \return true if hello world LoRa command received, false otherwise
	 */
	bool TxLoraHw(void);
	/*!
	 * \brief received "off" via serial USB
	 *
	 * \return true if off command received, false otherwise
	 */
	bool TurnOff(void);
	/*!
	 * \brief has transitioned from disconnected to connected
	 *
	 * \return true if USB has just been plugged in, false otherwise
	 */
	bool PluggedIn(void);
	/*!
	 * \brief "lora_integer_#" received via serial USB
	 *
	 * \return true if integer LoRa command received, false otherwise
	 */
	bool TxLoraInt(void);
}

#endif //__EXAMPLE_STATE_MACHINE_H__
