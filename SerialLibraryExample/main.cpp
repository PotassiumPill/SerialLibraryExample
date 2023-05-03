/*
 * SerialLibraryExample.cpp
 *
 * Created: 5/2/2023 1:03:46 PM
 * Author : Aaron Reilman
 */ 


#include "sam.h"
#include "example_state_machine.h"


int main(void)
{
    StateMachine::STT_MACHINE example_state_machine;
	ExampleStateMachine::GetExampleStateMachine(&example_state_machine);
    while (1) 
    {
		StateMachine::ExecuteAction(&example_state_machine);
    }
}
