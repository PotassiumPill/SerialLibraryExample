/*
 * Name			:	example_state_machine.h
 * Created		:	05/03/2023 15:47:53
 * Author		:	Aaron Reilman
 * Description	:	State, event, action, and function definitions for your state machine.
 */


#ifndef __EXAMPLE_STATE_MACHINE_H__
#define __EXAMPLE_STATE_MACHINE_H__

#include "state_machine.h"
//Add your includes---------------------
#include "serial_communication.h"
#include "util.h"

//Add your macros-----------------------
#define RX_BUFFER_SIZE	512
#define TX_BUFFER_SIZE	512
#define BAUD_RATE		115200

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
		DISABLED,				//!< Entry point state
		INITIALIZING,			//!< Initializes clocks and serial controllers
		OFF,					//!< Command don't work, just waits to turn on
		PROMPT_USER,			//!< Prints out a prompt when turned on
		ON,						//!< Primary command processing
		SUPER					//!< Checks for off command and USB connect/disconnect
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
	 * Entry point for state machine, does nothing
	 *
	 * \return state to transition to (INITIALIZING)
	 */
	StateMachine::STT_STATE DisabledStateAction(void);
	/*!
	 * \brief Initializing State action function (INITIALIZING)
	 *
	 * Initializes clocks and serial controllers
	 *
	 * \return state to transition to (OFF)
	 */
	StateMachine::STT_STATE InitializingStateAction(void);
	/*!
	 * \brief Off State action function (OFF)
	 *
	 * Sits in an idle off state, only checking for the on ("on") command
	 *
	 * \return state to transition to (OFF, or PROMPT_USER, unless super state transitions out of sub-state)
	 */
	StateMachine::STT_STATE OffStateAction(void);
	/*!
	 * \brief Prompt User State action function (PROMPT_USER)
	 *
	 * Sends a message through serial port when on moving to on state
	 *
	 * \return state to transition to (ON)
	 */
	StateMachine::STT_STATE PromptUserStateAction(void);
	/*!
	 * \brief On State action function (ON)
	 *
	 * Checks for various commands sent through serial port:\n 
	 * "hello world" 
	 * "integer_#" (replace # with a desired ASCII integer)
	 * "square_#!" (replace # with a desired ASCII integer)
	 * "echo" this will echo the received data onto the terminal and regular function will cease
	 * "off" turns off machine
	 *
	 * \return state to transition to (ON, or OFF, unless super state transitions out of sub-state)
	 */
	StateMachine::STT_STATE OnStateAction(void);
	/*!
	 * \brief Super State action function (SUPER)
	 *
	 * Super state which checks for USB connection and disconnection
	 *
	 * \return state to transition to (SUPER, OFF, or PROMPT_USER)
	 */
	StateMachine::STT_STATE SuperStateAction(void);

	//common action functions and events----------------

	/*!
	 * \brief Checks if USB has been disconnected
	 *
	 * Checks if an established USB connection has disconnected
	 *
	 * \return true if USB has been unplugged, false otherwise
	 */
	bool Unplugged(void);
	/*!
	 * \brief Checks if on command has been received
	 *
	 * Checks if "on" has been received in serial port
	 *
	 * \return true if on command received, false otherwise
	 */
	bool TurnOn(void);
	/*!
	 * \brief Checks if USB has been connected
	 *
	 * Checks if a USB has just been connected to device
	 *
	 * \return true if USB has been plugged in, false otherwise
	 */
	bool PluggedIn(void);
	/*!
	 * \brief Checks if off command has been received
	 *
	 * Checks if "off" has been received in serial port
	 *
	 * \return true if off command received, false otherwise
	 */
	bool TurnOff(void);
}

#endif //__EXAMPLE_STATE_MACHINE_H__
