# SerialLibraryExample
Example project to showcase custom C++ serial communication library, configured for an example on the SAMD21 using Microchip Studio. 

##Features
1) [Serial UART](/SerialLibraryExample/serial_uart) library
2) [Serial SPI](/SerialLibraryExample/serial_spi) library.
3) [Serial USB](/SerialLibraryExample/serial_usb) library using [tinyUSB](https://github.com/hathach/tinyusb/tree/master/src) stack. 
4) Python scripts for generating finite state machine code, which is very useful for streamlining code and maximizing functionality. Refer to [README](/state_machine_generator/README.txt) for more information on how to use.

## Running the Example Project
1) Refer to the necessary datasheets to set up the SAMD21 and your debugger of choice. 
2) The regular example uses a serial CDC USB connection.
3) Open a serial terminal with 115200 baud rate and ensure it is connected to the right port.
4) If properly connected you should see text indicating the USB is plugged in.
5) You can now type the following commands:
   - `hello world`  (response `World: hello!`)
   - `integer_#`    (replace # with ASCII digits)
   - `square_#!`    (replace # with ASCII digits)
   - `off`          (moves into an "off" state)
   - `echo`         (echos all data sent to chip on terminal but ceases regular function in example)
6) In the off state, command `on` can be sent to go back into "on" state.
