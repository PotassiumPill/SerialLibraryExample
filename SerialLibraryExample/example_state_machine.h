/*
 * Name			:	example_state_machine.h
 * Created		:	05/03/2023 15:47:53
 * Author		:	aaron
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
		DISABLED,
		INITIALIZING,
		OFF,
		PROMPT_USER,
		ON,
		SUPER
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
	 * Your description for Disabled State action function
	 *
	 * \return state to transition to (INITIALIZING)
	 */
	StateMachine::STT_STATE DisabledStateAction(void);
	/*!
	 * \brief Initializing State action function (INITIALIZING)
	 *
	 * Your description for Initializing State action function
	 *
	 * \return state to transition to (OFF)
	 */
	StateMachine::STT_STATE InitializingStateAction(void);
	/*!
	 * \brief Off State action function (OFF)
	 *
	 * Your description for Off State action function
	 *
	 * \return state to transition to (OFF, or PROMPT_USER, unless super state transitions out of sub-state)
	 */
	StateMachine::STT_STATE OffStateAction(void);
	/*!
	 * \brief Prompt User State action function (PROMPT_USER)
	 *
	 * Your description for Prompt User State action function
	 *
	 * \return state to transition to (ON)
	 */
	StateMachine::STT_STATE PromptUserStateAction(void);
	/*!
	 * \brief On State action function (ON)
	 *
	 * Your description for On State action function
	 *
	 * \return state to transition to (ON, unless super state transitions out of sub-state)
	 */
	StateMachine::STT_STATE OnStateAction(void);
	/*!
	 * \brief Super State action function (SUPER)
	 *
	 * Your description for Super State action function
	 *
	 * \return state to transition to (SUPER, OFF, OFF, or PROMPT_USER)
	 */
	StateMachine::STT_STATE SuperStateAction(void);

	//common action functions and events----------------

	/*!
	 * \brief Your Unplugged event description
	 *
	 * Your extended Unplugged event description
	 *
	 * \return true if [event true description], false otherwise
	 */
	bool Unplugged(void);
	/*!
	 * \brief Your Turn On event description
	 *
	 * Your extended Turn On event description
	 *
	 * \return true if [event true description], false otherwise
	 */
	bool TurnOn(void);
	/*!
	 * \brief Your Plugged In event description
	 *
	 * Your extended Plugged In event description
	 *
	 * \return true if [event true description], false otherwise
	 */
	bool PluggedIn(void);
	/*!
	 * \brief Your Turn Off event description
	 *
	 * Your extended Turn Off event description
	 *
	 * \return true if [event true description], false otherwise
	 */
	bool TurnOff(void);
}

#endif //__EXAMPLE_STATE_MACHINE_H__
