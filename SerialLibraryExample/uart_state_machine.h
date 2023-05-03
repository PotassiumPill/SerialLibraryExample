/*
 * Name			:	uart_state_machine.h
 * Created		:	05/03/2023 12:38:10
 * Author		:	Aaron Reilman
 * Description	:	State, event, action, and function definitions for your state machine.
 */


#ifndef __UART_STATE_MACHINE_H__
#define __UART_STATE_MACHINE_H__

#include "state_machine.h"
//Add your includes---------------------
#include "serial_communication.h"
#include "lora_controller.h"
#include "util.h"

//Add your macros-----------------------
#define USING_LORA


#define UART_RX_SIZE		512
#define UART_TX_SIZE		512
#define STT_SPI_GENCLK		3
#define TIMEOUT				2000

/*!
 * \brief Namespace containing all function and structure definitions to implement your state machine.
 *
 * Contains defined STT_STATEs, state functions, and all other executive functions to be performed in the state machine.
 */
namespace UartStateMachine
{
	/*!
	 * \brief Defined enum of STT_STATEs
	 *
	 * Enum of uint8_t for all implemented states.
	 */
	enum STT_STATE : uint8_t {
		DISABLED,
		INITIALIZING,
		OFF_STATE,
		PROMPT_USER,
		WAIT_FOR_RESPONSE,
		LORA_HW,
		LORA_INT,
		SUPER
	};
	/*!
	 * \brief Populates state machine struct with values
	 *
	 * Initializes state machine by populating an initialized struct with states and functions.
	 *
	 * \param state_machine pointer to state machine which will be populated with values
	 */
	void GetUartStateMachine(StateMachine::STT_MACHINE * state_machine);
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
	 * \return state to transition to (OFF_STATE)
	 */
	StateMachine::STT_STATE InitializingStateAction(void);
	/*!
	 * \brief Off State State action function (OFF_STATE)
	 *
	 * Your description for Off State State action function
	 *
	 * \return state to transition to (OFF_STATE, or PROMPT_USER, unless super state transitions out of sub-state)
	 */
	StateMachine::STT_STATE OffStateStateAction(void);
	/*!
	 * \brief Prompt User State action function (PROMPT_USER)
	 *
	 * Your description for Prompt User State action function
	 *
	 * \return state to transition to (WAIT_FOR_RESPONSE)
	 */
	StateMachine::STT_STATE PromptUserStateAction(void);
	/*!
	 * \brief Wait For Response State action function (WAIT_FOR_RESPONSE)
	 *
	 * Your description for Wait For Response State action function
	 *
	 * \return state to transition to (WAIT_FOR_RESPONSE, LORA_HW, or LORA_INT, unless super state transitions out of sub-state)
	 */
	StateMachine::STT_STATE WaitForResponseStateAction(void);
	/*!
	 * \brief Lora Hw State action function (LORA_HW)
	 *
	 * Your description for Lora Hw State action function
	 *
	 * \return state to transition to (WAIT_FOR_RESPONSE)
	 */
	StateMachine::STT_STATE LoraHwStateAction(void);
	/*!
	 * \brief Lora Int State action function (LORA_INT)
	 *
	 * Your description for Lora Int State action function
	 *
	 * \return state to transition to (WAIT_FOR_RESPONSE)
	 */
	StateMachine::STT_STATE LoraIntStateAction(void);
	/*!
	 * \brief Super State action function (SUPER)
	 *
	 * Your description for Super State action function
	 *
	 * \return state to transition to (SUPER, or OFF_STATE)
	 */
	StateMachine::STT_STATE SuperStateAction(void);

	//common action functions and events----------------

	/*!
	 * \brief Your Tx Lora Hw event description
	 *
	 * Your extended Tx Lora Hw event description
	 *
	 * \return true if [event true description], false otherwise
	 */
	bool TxLoraHw(void);
	/*!
	 * \brief Your Tx Lora Int event description
	 *
	 * Your extended Tx Lora Int event description
	 *
	 * \return true if [event true description], false otherwise
	 */
	bool TxLoraInt(void);
	/*!
	 * \brief Your Turn Off event description
	 *
	 * Your extended Turn Off event description
	 *
	 * \return true if [event true description], false otherwise
	 */
	bool TurnOff(void);
	/*!
	 * \brief Your Turn On event description
	 *
	 * Your extended Turn On event description
	 *
	 * \return true if [event true description], false otherwise
	 */
	bool TurnOn(void);
}

#endif //__UART_STATE_MACHINE_H__
