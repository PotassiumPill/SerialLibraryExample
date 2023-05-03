/*
 * Name			:	state_machine.h
 * Created		:	05/02/2023 14:55:51
 * Author		:	aaron
 * Description	:	Simple process manager for creating streamlined code with multiple states, events, and actions.
 */


#ifndef __STATE_MACHINE_H__
#define __STATE_MACHINE_H__

#include <stdint.h>

//edit consts to your need
#define	NUM_STT_STATES	7


/*!
 * \brief Namespace containing all typedefs, structures, and primary functions to implement and run your state machine.
 *
 * Basic API for implementing a Finite State Machine in your code.\n 
 * 1) Edit NUM_STT_STATES macro to match the number of states in your code.\n 
 * 2) Define and declare an enum of state types in a separate file if desired to improve readability of code.\n 
 * 3) Define and declare your state machine and all state action functions. Each state action should return StateMachine::STT_STATE and have 0 parameters.\n 
 * 4) Add super state functionality to your state action functions by calling StateMachine::ProcessSuperState() at the end of each sub-state. You should define and declare your super state action functions.\n 
 * 5) In your superloop, call StateMachine::ExecuteAction() and pass the address of your state machine (i.e. StateMachine::ExecuteAction(&my_state_machine);).
 */
namespace StateMachine
{
	/*!
	 * \brief Type definition for possible states
	 *
	 * Defines the possible states for your code.
	 *
	 * \note Create an enum of uint8_t to organize your state types.
	 */
	typedef uint8_t STT_STATE;

	/*!
	 * \brief Type definition for state action function pointer
	 *
	 * The function pointer which performs all actions during a specific state and returns the state to transition to.
	 */
	typedef STT_STATE (*STT_ACTION)(void);
	/*!
	 * \brief State machine struct containing the base state and all rows in the program.
	 *
	 * Contains the current state of the state machine and array of state action functions.
	*/
	struct STT_MACHINE {
		STT_STATE current_state;						//!< current state of machine
		STT_ACTION state_actions[NUM_STT_STATES];		//!< array of state action function pointers
	};
	/*!
	 * \brief Iterates through a state machine table
	 *
	 * Manages and executes a state machine table
	 *
	 * \param state_table pointer to state table struct
	 */
	void ExecuteAction(STT_MACHINE * state_table);
	/*!
	 * \brief Super state processing function to be used in implementing your state machine.
	 *
	 * Processes a super state when called in implementation of sub state action functions.\n 
	 * User must implement super state action function in same format as all other state action functions.\n 
	 * Function is called immediately before return statement in sub-states to ensure that the proper state transition is performed.
	 *
	 * \param current_state pointer to current transitory state, used to preserve current sub-state if super state function doesn't transition
	 * \param super_state value of super state which sub state belongs to
	 * \param super_function address of super state action function
	 */
	void ProcessSuperState(STT_STATE * current_state, STT_STATE super_state, STT_ACTION super_function);
}

#endif //__STATE_MACHINE_H__
