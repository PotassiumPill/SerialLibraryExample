# State Machine Generator

Python scripts for generating finite-state machine template code, based in either C or C++.

## How To Use
1) Create comma-delimited csv file with name of your state machine (this will be used as your namespace).
2) Each row in the csv file represents a state.
3) The first row will be generated as the initial state on program startup.
4) The first column will always be the state name.
5) Use underscores instead of spaces when naming all states and events. For clarity it is best to use all caps when naming your states.
6) To indicate that a state is exclusively a super state, write `\s` in the next cell after the state name.
7) Each row's default transition state (the next state to transition to if no events occur) will be set to itself.
8) To change a row's default transition state, enter `\DEFAULT` in a cell and the name of the state to set it to in the next cell.
9) To add events to a row, enter the name of the event in a cell and the state it will transition to in the next cell.
10) To bind a state to a super state, add `\SUPER` to a cell and the name of the super state in the next cell. Super states can also be bound to a bigger super state.
11) To generate files, open this folder in terminal and enter: `py -3 cpp_generator.py` OR `py -3 c_generator.py`.
12) You will find generated files in a new folder in this directory.

## Implement State Machine Template
1) You will implement the functions in the cpp file that matches the name of your csv file. The getter function (named similarly to `GetYourStateMachineName`) will be implemented already.
2) If desired, you can fill in documentation for your code in the h file that maches the name of your csv file.

## Execute State Machine
1) In your main superloop, you will need to define, initialize, and run your state machine.
2) The example below shows each step of this process in the main loop:
```
#include "my_state_machine.h"

int main(void)
{	
	StateMachine::STT_MACHINE my_state_machine;
	MyStateMachine::GetMyStateMachine(&emy_state_machine);
	while (1) 
	{
		StateMachine::ExecuteAction(&my_state_machine);
	}
}
```

## Manually Add New States
1) Navigate to `state_machine.h`, and find the line which looks similar to `#define	NUM_STT_STATES	5`.
2) Increase the number of states by the amount of non-super states you want to add.
3) Navigate to the h file that maches the name of your csv file.
4) In the STT_STATE enum, add your new states. You must add non-super states before the first super state, and add super states to the end of the enum.
5) Add state functions for your new states. Each state function must return `StateMachine::STT_STATE` (C++) or `STT_STATE` (C) and have a `void` parameter. 
6) Add event functions here, if necessary.
7) Navigate to the cpp file that maches the name of your csv file.
8) In the state machine getter (named similarly to `GetYourStateMachineName`), add lines which map your state enums to their respective state functions. For example, with a state enum `NEW_STATE` and state function `NewStateAction()`, add the line:
```
state_machine->state_actions[STT_STATE::NEW_STATE] = &NewStateAction;
```
in C++, or
```
state_machine->state_actions[NEW_STATE] = &NewStateAction;
```
in C.

9) Implement the functions you defined in your header file earlier. Remember to return the value of the states you want to transition to, and call `StateMachine::ProcessSuperState()`/`ProcessSuperState()` at the end to bind a state to a super state. Refer to the [state machine documentation page](https://potassiumpill.github.io/SerialLibraryExample/namespace_state_machine.html) to see its use, or refer to previously generated code.