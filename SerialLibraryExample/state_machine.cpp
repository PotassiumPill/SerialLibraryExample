/*
 * Name				:	state_machine.cpp
 * Created			:	05/03/2023 15:41:45
 * Author			:	Aaron Reilman
 * Description		:	Simple process manager for creating streamlined code with multiple states, events, and actions.
 */


#include "state_machine.h"

void StateMachine::ExecuteAction(StateMachine::STT_MACHINE * state_table)
{
	state_table->current_state = state_table->state_actions[state_table->current_state]();
}

void StateMachine::ProcessSuperState(StateMachine::STT_STATE * current_state, StateMachine::STT_STATE super_state, StateMachine::STT_ACTION super_function)
{
	STT_STATE temp_state = super_function();
	if(temp_state != super_state)
		*current_state = temp_state;
}
