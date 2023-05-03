import csv
import os
from datetime import datetime

class State:
    def __init__(self, state_name = ""):
        self.state = state_name
        self.default = self
        self.super = None
        self.isSuper = False
        self.events = {}
    
    def set_default(self, default_state):
        self.default = default_state
    
    def set_super(self, self_state):
        self.super = self_state
    
    def set_as_super(self):
        self.isSuper = True
    
    def remove_as_super(self):
        self.isSuper = False

    def add_event(self, event_name, event_result):
            self.events[event_name] = event_result
    
    def __str__(self):
        out = ""
        if self.isSuper:
            out += "Super "
        out += f"State: {self.state}, Default: {self.default.state}, Substate Of: "
        if(self.super is None):
            out += f"None, Events: "
        else:
            out += f"{self.super.state}, Events: "
        state_dict = {}
        for event, state in self.events.items():
            state_dict[event] = state.state
        out += str(state_dict)
        return out

def get_csv_files_in_dir():
    csvFiles = []
    fileNames = [f for f in os.listdir() if os.path.isfile(f)]
    for file in fileNames:
        if os.path.splitext(file)[1] == '.csv':
            csvFiles.append(file)
    return csvFiles

def read_csv(filename):
    with open(filename, "r") as fileIn:
        csv_reader = csv.reader(fileIn, delimiter=',')
        output = []
        for row in csv_reader:
            trimmed_row = [x for x in row if x != '']
            output.append(trimmed_row)
    return output

def initialize_state_machine(input, state_machine):
    for row in input:
        if row[0] not in state_machine:
            state_machine[row[0]] = State(row[0])
        else:
            print("Warning, duplicate state in csv file!")
            return False
    return True

def populate_state_machine(input, state_machine):
    for row in input:
        getDefault = False
        getSuper = False
        getEvent = False
        eventName = ""
        isFirst = True
        for entry in row:
            if not isFirst:
                if entry == "\s":
                    if getDefault or getSuper or getEvent:
                        print(f"Warning, invalid \s parameter in row {row[0]}")
                        return False
                    state_machine[row[0]].set_as_super()    
                elif entry == "\DEFAULT":
                    if getDefault or getSuper or getEvent:
                        print(f"Warning, invalid \DEFAULT parameter in row {row[0]}")
                        return False
                    getDefault = True   
                elif entry == "\SUPER":
                    if getDefault or getSuper or getEvent:
                        print(f"Warning, invalid \SUPER parameter in row {row[0]}")
                        return False
                    getSuper = True
                else:
                    if getDefault:
                        getDefault = False
                        if entry not in state_machine:
                            print(f"Warning, invalid default parameter {entry} in row {row[0]}")
                            return False
                        state_machine[row[0]].set_default(state_machine[entry])
                    elif getSuper:
                        getSuper = False
                        if entry not in state_machine:
                            print(f"Warning, invalid super state parameter {entry} in row {row[0]}")
                            return False    
                        state_machine[row[0]].set_super(state_machine[entry])      
                    elif getEvent:
                        getEvent = False
                        if entry not in state_machine:
                            print(f"Warning, invalid event parameter {entry} in row {row[0]}")
                            return False
                        state_machine[row[0]].add_event(eventName, state_machine[entry])   
                    else:
                        if entry in state_machine:
                            print(f"Warning, state name used as event in row {row[0]}")
                            return False
                        getEvent = True
                        eventName = entry
            isFirst = False
    return True

def write_header_on_file(file, filename, description):
        file.write("/*\n")
        file.write(f" * Name\t\t\t:\t{filename}\n")
        print_date = datetime.now().strftime("%m/%d/%Y %H:%M:%S")
        file.write(f" * Created\t\t:\t{print_date}\n")
        file.write(f" * Author\t\t:\t{os.getlogin()}\n")
        file.write(f" * Description\t:\t{description}\n")
        file.write(" */\n\n\n")

def generate_string(raw_string, style="CamelCase"):
    parts = raw_string.split("_")
    parts = [s.capitalize() for s in parts]
    if style=="ALL_CAPS":
        parts = [s.upper() + "_" for s in parts]
    elif style=="Name":
        parts = [s + " " for s in parts]
    out = ""
    for part in parts:
        out += part
    return out

def create_code_from_csv(file_name):
    MY_STATE_MACHINE = "my_state_machine"
    input = read_csv(file_name)
    print(f"Reading from {file_name}...\n")
    file_name = os.path.splitext(file_name)[0]
    state_machine = {}
    if not(initialize_state_machine(input, state_machine) and populate_state_machine(input, state_machine)):
        return False
    for state in state_machine.values():
        print(state)
    print("\n")
    if file_name == "state_machine":
        file_name = MY_STATE_MACHINE
    if not os.path.isdir(file_name+"_cpp"):
        os.mkdir(file_name+"_cpp")
    with open(os.path.join(file_name+"_cpp","state_machine.h"),"w") as hOut:
        write_header_on_file(hOut, "state_machine.h", "Simple process manager for creating streamlined code with multiple states, events, and actions.")
        hOut.write("#ifndef __STATE_MACHINE_H__\n")
        hOut.write("#define __STATE_MACHINE_H__\n\n")
        hOut.write("#include <stdint.h>\n\n")
        hOut.write("//edit consts to your need\n")
        hOut.write(f"#define\tNUM_STT_STATES\t{len([state for state in state_machine.values() if not state.isSuper])}\n\n\n")
        hOut.write("/*!\n")
        hOut.write(" * \\brief Namespace containing all typedefs, structures, and primary functions to implement and run your state machine.\n")
        hOut.write(" *\n")
        hOut.write(" * Basic API for implementing a Finite State Machine in your code.\\n \n")
        hOut.write(" * 1) Edit NUM_STT_STATES macro to match the number of states in your code.\\n \n") 
        hOut.write(" * 2) Define and declare an enum of state types in a separate file if desired to improve readability of code.\\n \n") 
        hOut.write(" * 3) Define and declare your state machine and all state action functions. Each state action should return StateMachine::STT_STATE and have 0 parameters.\\n \n")
        hOut.write(" * 4) Add super state functionality to your state action functions by calling StateMachine::ProcessSuperState() at the end of each sub-state. You should define and declare your super state action functions.\\n \n") 
        hOut.write(" * 5) In your superloop, call StateMachine::ExecuteAction() and pass the address of your state machine (i.e. StateMachine::ExecuteAction(&my_state_machine);).\n")
        hOut.write(" */\n")
        hOut.write("namespace StateMachine\n")
        hOut.write("{\n")
        hOut.write("\t/*!\n")
        hOut.write("\t * \\brief Type definition for possible states\n")
        hOut.write("\t *\n")
        hOut.write("\t * Defines the possible states for your code.\n")
        hOut.write("\t *\n")
        hOut.write("\t * \\note Create an enum of uint8_t to organize your state types.\n")
        hOut.write("\t */\n")
        hOut.write("\ttypedef uint8_t STT_STATE;\n\n") 
        hOut.write("\t/*!\n")
        hOut.write("\t * \\brief Type definition for state action function pointer\n")
        hOut.write("\t *\n")
        hOut.write("\t * The function pointer which performs all actions during a specific state and returns the state to transition to.\n")
        hOut.write("\t */\n")
        hOut.write("\ttypedef STT_STATE (*STT_ACTION)(void);\n")
        hOut.write("\t/*!\n")
        hOut.write("\t * \\brief State machine struct containing the base state and all rows in the program.\n")
        hOut.write("\t *\n")
        hOut.write("\t * Contains the current state of the state machine and array of state action functions.\n")
        hOut.write("\t*/\n")
        hOut.write("\tstruct STT_MACHINE {\n")
        hOut.write("\t\tSTT_STATE current_state;\t\t\t\t\t\t//!< current state of machine\n")
        hOut.write("\t\tSTT_ACTION state_actions[NUM_STT_STATES];\t\t//!< array of state action function pointers\n")
        hOut.write("\t};\n")
        hOut.write("\t/*!\n")
        hOut.write("\t * \\brief Iterates through a state machine table\n")
        hOut.write("\t *\n")
        hOut.write("\t * Manages and executes a state machine table\n")
        hOut.write("\t *\n")
        hOut.write("\t * \param state_table pointer to state table struct\n")
        hOut.write("\t */\n")
        hOut.write("\tvoid ExecuteAction(STT_MACHINE * state_table);\n")
        hOut.write("\t/*!\n")
        hOut.write("\t * \\brief Super state processing function to be used in implementing your state machine.\n")
        hOut.write("\t *\n") 
        hOut.write("\t * Processes a super state when called in implementation of sub state action functions.\\n \n") 
        hOut.write("\t * User must implement super state action function in same format as all other state action functions.\\n \n")
        hOut.write("\t * Function is called immediately before return statement in sub-states to ensure that the proper state transition is performed.\n")
        hOut.write("\t *\n")
        hOut.write("\t * \param current_state pointer to current transitory state, used to preserve current sub-state if super state function doesn't transition\n")
        hOut.write("\t * \param super_state value of super state which sub state belongs to\n")
        hOut.write("\t * \param super_function address of super state action function\n")
        hOut.write("\t */\n")
        hOut.write("\tvoid ProcessSuperState(STT_STATE * current_state, STT_STATE super_state, STT_ACTION super_function);\n")
        hOut.write("}\n\n")
        hOut.write("#endif //__STATE_MACHINE_H__\n")
    print("state_machine.h successfully generated!")
    with open(os.path.join(file_name+"_cpp","state_machine.cpp"),"w") as cppOut:
        write_header_on_file(cppOut, "state_machine.cpp", "Simple process manager for creating streamlined code with multiple states, events, and actions.")
        cppOut.write("#include \"state_machine.h\"\n\n")
        cppOut.write("void StateMachine::ExecuteAction(StateMachine::STT_MACHINE * state_table)\n")
        cppOut.write("{\n")
        cppOut.write("\tstate_table->current_state = state_table->state_actions[state_table->current_state]();\n")
        cppOut.write("}\n\n")
        cppOut.write("void StateMachine::ProcessSuperState(StateMachine::STT_STATE * current_state, StateMachine::STT_STATE super_state, StateMachine::STT_ACTION super_function)\n")
        cppOut.write("{\n")
        cppOut.write("\tSTT_STATE temp_state = super_function();\n")
        cppOut.write("\tif(temp_state != super_state)\n")
        cppOut.write("\t\t*current_state = temp_state;\n")
        cppOut.write("}\n")
    print("state_machine.cpp successfully generated!")
    all_events = []
    with open(os.path.join(file_name+"_cpp",f"{file_name}.h"),"w") as hOut:
        write_header_on_file(hOut, f"{file_name}.h", "State, event, action, and function definitions for your state machine.")
        fNameAllCaps = generate_string(file_name, "ALL_CAPS")
        hOut.write(f"#ifndef __{fNameAllCaps}H__\n")
        hOut.write(f"#define __{fNameAllCaps}H__\n\n")
        hOut.write("#include \"state_machine.h\"\n")
        hOut.write("//Add your includes---------------------\n\n\n")
        hOut.write("//Add your macros-----------------------\n\n\n")
        hOut.write("/*!\n")
        hOut.write(" * \\brief Namespace containing all function and structure definitions to implement your state machine.\n")
        hOut.write(" *\n")
        hOut.write(" * Contains defined STT_STATEs, state functions, and all other executive functions to be performed in the state machine.\n")
        hOut.write(" */\n")
        namespace = generate_string(file_name)
        hOut.write(f"namespace {namespace}\n")
        hOut.write("{\n")
        hOut.write("\t/*!\n")
        hOut.write("\t * \\brief Defined enum of STT_STATEs\n")
        hOut.write("\t *\n")
        hOut.write("\t * Enum of uint8_t for all implemented states.\n")
        hOut.write("\t */\n")
        hOut.write("\tenum STT_STATE : uint8_t {\n")
        last_i = int(0)
        for i, state in enumerate([s for s in state_machine.values() if not s.isSuper]):
            str = generate_string(state.state, "ALL_CAPS")[:-1]
            hOut.write(f"\t\t{str}")
            if i < len(state_machine) - 1:
                hOut.write(",")
            hOut.write("\n")
            last_i = i
        last_i += 1
        for i, state in enumerate([s for s in state_machine.values() if s.isSuper]):
            str = generate_string(state.state, "ALL_CAPS")[:-1]
            hOut.write(f"\t\t{str}")
            if i + last_i < len(state_machine) - 1:
                hOut.write(",")
            hOut.write("\n")
        hOut.write("\t};\n")
        hOut.write("\t/*!\n")
        hOut.write("\t * \\brief Populates state machine struct with values\n")
        hOut.write("\t *\n")
        hOut.write("\t * Initializes state machine by populating an initialized struct with states and functions.\n")
        hOut.write("\t *\n")
        hOut.write("\t * \param state_machine pointer to state machine which will be populated with values\n")
        hOut.write("\t */\n")
        fileNameAsFunc = generate_string(file_name)
        hOut.write(f"\tvoid Get{fileNameAsFunc}(StateMachine::STT_MACHINE * state_machine);\n")
        for state in state_machine.values():
            hOut.write("\t/*!\n")
            stateName = generate_string(state.state,"Name")[:-1]
            state_cap = generate_string(state.state, "ALL_CAPS")[:-1]
            hOut.write(f"\t * \\brief {stateName} State action function ({state_cap})\n")
            hOut.write("\t *\n")
            hOut.write(f"\t * Your description for {stateName} State action function\n")
            hOut.write("\t *\n")
            hOut.write("\t * \\return state to transition to (")
            state_name = generate_string(state.default.state, "ALL_CAPS")[:-1]
            hOut.write(f"{state_name}")
            events = list(state.events.values())
            all_events.extend(list(state.events.keys()))
            if state.default in events:
                events.remove(state.default)
            for i, event in enumerate(events):
                hOut.write(", ") 
                if i > len(events) - 2:
                    hOut.write("or ")
                state_name = generate_string(event.state, "ALL_CAPS")[:-1]
                hOut.write(state_name)
            if state.super:
                hOut.write(", unless super state transitions out of sub-state")
            hOut.write(")\n\t */\n")
            state_function = generate_string(state.state)
            hOut.write(f"\tStateMachine::STT_STATE {state_function}StateAction(void);\n")
        hOut.write("\n\t//common action functions and events----------------\n\n")
        hOut.write("\t/* #######################################################################################################################\n")
        hOut.write("\t * TODO: add function prototypes here for common actions and modify/add events below to implement your state machine here.\n")
        hOut.write("\t * #######################################################################################################################\n")
        hOut.write("\t */\n\n")
        all_events = [*set(all_events)]
        for event in all_events:
            event_name = generate_string(event,"Name")[:-1]
            event_func = generate_string(event)
            hOut.write("\t/*!\n")
            hOut.write(f"\t * \\brief Your {event_name} event description\n")
            hOut.write("\t *\n")
            hOut.write(f"\t * Your extended {event_name} event description\n")
            hOut.write("\t *\n")
            hOut.write("\t * \\return true if [event true description], false otherwise\n")
            hOut.write("\t */\n")
            hOut.write(f"\tbool {event_func}(void);\n")
        hOut.write("}\n\n")
        hOut.write(f"#endif //__{fNameAllCaps}H__\n")
    print(f"{file_name}.h successfully generated!")
    with open(os.path.join(file_name+"_cpp",f"{file_name}.cpp"),"w") as cppOut:
        write_header_on_file(cppOut, f"{file_name}.cpp", "State, event, action, and function definitions for your state machine.")
        cppOut.write(f"#include \"{file_name}.h\"\n\n")
        cppOut.write("//Add your public vars----------------\n\n\n")
        cppOut.write("//state machine struct getter\n")
        namespace = generate_string(file_name)
        cppOut.write(f"void {namespace}::Get{namespace}(StateMachine::STT_MACHINE * state_machine)\n")
        cppOut.write("{\n")
        init_state = generate_string(list(state_machine.keys())[0], "ALL_CAPS")[:-1]
        cppOut.write(f"\tstate_machine->current_state = STT_STATE::{init_state};\n")
        for s in [state for state in state_machine.values() if not state.isSuper]:
            state_enum = generate_string(s.state, "ALL_CAPS")[:-1]
            state_func = generate_string(s.state)
            cppOut.write(f"\tstate_machine->state_actions[STT_STATE::{state_enum}] = &{state_func}StateAction;\n")
        cppOut.write("}\n\n")
        cppOut.write("//state action functions\n")
        for state in state_machine.values():
            state_enum = generate_string(state.state, "ALL_CAPS")[:-1]
            state_func = generate_string(state.state)
            state_name = generate_string(state.state, "Name")[:-1]
            state_default = generate_string(state.default.state, "ALL_CAPS")[:-1]
            cppOut.write(f"StateMachine::STT_STATE {namespace}::{state_func}StateAction(void)\n")
            cppOut.write("{\n")
            state_trans_str = "return STT_STATE::"
            if state.super:
                cppOut.write(f"\tStateMachine::STT_STATE current_state = STT_STATE::{state_default};\n")
                state_trans_str = "current_state = STT_STATE::"
            cppOut.write("\t/*\n")
            cppOut.write(f"\t * TODO: implement actions for {state_name} State\n")
            cppOut.write("\t */\n")
            else_suffix = ""
            for event, s in state.events.items():
                next_state = generate_string(s.state, "ALL_CAPS")[:-1]
                event_func = generate_string(event)
                event_name = generate_string(event, "Name")[:-1]
                cppOut.write(f"\t{else_suffix}if({event_func}())\n")
                cppOut.write("\t{\n")
                cppOut.write("\t\t/*\n")
                cppOut.write(f"\t\t * TODO: implement actions for {event_name} Event\n")
                cppOut.write("\t\t */\n")
                cppOut.write(f"\t\t{state_trans_str}{next_state};\n")
                cppOut.write("\t}\n")
                else_suffix = "else "
            if state.super:
                super_state = generate_string(state.super.state, "ALL_CAPS")[:-1]
                super_state_func = generate_string(state.super.state)
                cppOut.write(f"\tStateMachine::ProcessSuperState(&current_state, STT_STATE::{super_state}, &{super_state_func}StateAction);\n")
                cppOut.write("\treturn current_state;\n")
            else:
                cppOut.write(f"\treturn STT_STATE::{state_default};\n")
            cppOut.write("}\n\n")
        cppOut.write("//common event functions\n\n")
        cppOut.write("/* ############################################################\n")
        cppOut.write(" * TODO: add/remove/implement event function definitions below.\n")
        cppOut.write(" * ############################################################\n")
        cppOut.write(" */\n\n")
        for event in all_events:
            event_name = generate_string(event,"Name")[:-1]
            event_func = generate_string(event)
            cppOut.write(f"bool {namespace}::{event_func}(void)\n")
            cppOut.write("{\n")
            cppOut.write("\t/*\n")
            cppOut.write(f"\t * TODO: implement {event_name} Event\n")
            cppOut.write("\t */\n")
            cppOut.write("\treturn true;\n")
            cppOut.write("}\n\n")
        cppOut.write("//common action functions\n\n")
        cppOut.write("/* #############################################\n")
        cppOut.write(" * TODO: implement common action functions here.\n")
        cppOut.write(" * #############################################\n")
        cppOut.write(" */\n\n")
    print(f"{file_name}.cpp successfully generated!")
    return True


def main():
    files = get_csv_files_in_dir()
    if not files:
        print("Could not find csv file in directory!")
        return
    for file in files:
        create_code_from_csv(file)
    


if __name__ == "__main__":
    main()