How to Use:

1) Create comma-delimited .csv file with name of your state machine (this will be used as your namespace).
2) Each row in the csv file represents a state.
3) The first row will be generated as the initial state on program startup.
4) The first column will always be the state name.
5) Use underscores instead of spaces when naming all states and events. For clarity it is best to use all caps when naming your states.
6) To indicate that a state is exclusively a super state, write \s in the next cell after the state name.
7) Each row's default transition state (the next state to transition to if no events occur) will be set to itself.
8) To change a row's default transition state, enter \DEFAULT in a cell and the name of the state to set it to in the next cell.
9) To add events to a row, enter the name of the event in a cell and the state it will transition to in the next cell.
10) To bind a state to a super state, add \SUPER to a cell and the name of the super state in the next cell. Super states can also be bound to a bigger super state.
11) To generate files, open this folder in terminal and enter: "py -3 cpp_generator.py" OR "py -3 c_generator.py" (without quotation marks).
12) You will find generated files in a new folder in this directory.