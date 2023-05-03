/* 
 * Name			:	serial_usb.h
 * Created		:	8/12/2022 10:53:43 AM
 * Author		:	Aaron Reilman
 * Description	:	A USB serial communication driver.
 */


#ifndef __SERIAL_USB_H__
#define __SERIAL_USB_H__

#include "tusb.h"
#include "serial_buffer/serial_buffer.h"

#define DEFAULT_USB_FIFO_SIZE	512

/*!
 * \brief %USB Controller global namespace.
 *
 * This namespace contains all global classes, functions and definitions needed to implement controller for USB serial communication.
 */
 namespace SerialUSB
 {
	/*!
	 * \brief A helper function for initializing clocks and I/O pins on specific chip.
	 *
	 * Sets up clocks and I/O pins before initializing tinyUSB stack. Specific hardware implementation is determined by your specific tusb_config.h file.
	 *
	 * \sa USBController.Init() 
	 */
	void USBFeedIOClocks(void);
	/*!
	 * \brief Triggers USB reset operation.
	 */
	void ResetUSB(void);
	/*!
	 * \brief %USB serial communication controller object
	 *
	 * This is a %USB serial communication controller which manages a single peripheral. It creates a %SerialBuffer for reception of data over USB and some simple parsing capabilities. 
	 * This controller is interrupt and task driven, so Task() must be implemented before or after all application code in the main while loop, and ISR() must be implemented in the 
	 * USB interrupt Handler. 
	 */
	class USBController
	{
		//functions
		public:
		/*!
		 * \brief Constructor
		 *
		 * Instantiates controller object for USB serial communication. This will use the TinyUSB stack to perform the necessary tasks. The object instance must be initialized using Init() to begin
		 * using it.
		 *
		 * \sa ~USBController(), Init(), Deinit()
		 */
		USBController(void);
		/*!
		 * \brief Destructor
		 *
		 * Called when UARTController object is destroyed. Immediately halts data transmission.
		 *
		 * \note The hardware teardown for TinyUSB is currently unimplemented. It is not recommended to call the destructor manually.
		 * \sa USBController(), Init(), Deinit()
		 */
		~USBController(void);
		/*!
		 * \brief Initializes USB serial communication.
		 *
		 * Initializes peripheral hardware and uses optional parameter for FIFO buffer size if USB hasn't been initialized.\n 
		 * This function will not do anything if USB is currently enabled.
		 *
		 * \param buf initialized character array used for FIFO receive buffer
		 * \param buf_size buffer size
		 * \sa Deinit(), ResetBuffer()
		 */
		void Init(char * buf, uint32_t buf_size);
		/*!
		 * \brief Disables and resets USB serial communication.
		 *
		 * Disables USB serial controller if active.
		 *
		 * \note The hardware teardown for TinyUSB is currently unimplemented. It is not recommended to call this.
		 * \sa Init(), ClearBuffers(), ~UARTController()
		 */
		void Deinit(void);
		/*!
		 * \brief Resets FIFO receive buffer.
		 *
		 * Resets USB controller buffer using parameters for initialized arrays.
		 *
		 * \param buf initialized character array used for FIFO receive buffer
		 * \param buf_size buffer size
		 * \sa Init()
		 */
		void ResetBuffer(char * buf, uint32_t buf_size);
		/*!
		 * \brief Clears Rx and/or Tx buffers.
		 *
		 * Clears out the receive and/or transmit buffers and resets status data and indices.\n 
		 * Useful for quick handling of full buffers and errors.
		 *
		 * \param clear_tx clear transmit buffer (default = true)
		 * \param clear_rx clear receive buffer (default = true)
		 * \sa Deinit()
		 */
		void ClearBuffers(bool clear_tx = true, bool clear_rx = true);
		/*!
		 * \brief USB Interrupt Service Routine.
		 *
		 * Function to be called in USB interrupt handler. This calls the tusb ISR function and lets the USB stack handle interrupts.
		 *
		 * \sa Task()
		 */
		void ISR(void);
		/*!
		 * \brief USB Task Routine. 
		 *
		 * Function to be called in main while loop before or after application code. This will read all incoming data into the FIFO receive buffer and echo read data if desired using optional
		 * parameters. This also performs the main tusb task. There is also an option to disable the FIFO receive buffer which will not allow any read data to fill the FIFO receive buffer.
		 *
		 * \param echo sends all received data back to the host device allowing it to be read on a terminal (default = false)
		 * \sa ISR()
		 */
		void Task(bool echo = false);
		/*!
		 * \brief Transmit character across USB.
		 *
		 * Uses tusb FIFO queue to transmit a character.
		 *
		 * \param input char to be transmitted
		 * \return success of transmission
		 * \sa TransmitPacket(), TransmitString(), TransmitInt(), Task()
		 */
		bool Transmit(char input);
		/*!
		 * \brief Receive character across USB from host.
		 *
		 * Uses controller FIFO receive buffer to get the next unread character from the received data. 
		 *
		 * \param output pointer to received char (default = nullptr)
		 * \return success of reception
		 * \sa ReceiveString(), ReceiveInt(), ReceiveParam(), Task()
		 */
		bool Receive(char * output = nullptr);
		/*!
		 * \brief Transmits a packet of bytes, which do not need to be null-terminated.
		 *
		 * Transmits an array of characters given the number of characters to transmit. Good for sending raw bytes rather than ASCII characters.
		 * 
		 * \param input char array to be sent to host
		 * \param num_bytes number of bytes to be sent to host
		 * \return success of transmission
		 * \sa Transmit(), TransmitString(), TransmitInt(), Task() 
		 */
		bool TransmitPacket(const char * input, uint32_t num_bytes);
		/*!
		 * \brief Transmit string across USB.
		 *
		 * Uses tusb FIFO queue to transmit an entire array of characters.
		 * 
		 * \param input char array to be sent to host
		 * \return number of bytes sent in transmission
		 * \note Input array must be null-terminated.
		 * \sa Transmit(), TransmitPacket(), TransmitInt(), Task()
		 */
		uint32_t TransmitString(const char *input);
		/*!
		 * \brief Checks if input string has been received over USB from host.
		 *
		 * Checks input string with front of FIFO receive buffer, returning true if the input string has been detected.\n 
		 * Can be used multiple times for different strings.\n 
		 * Iterates read index.\n 
		 * If a string is detected, all calls will be disabled until a new character is received.\n  
		 * Shift parameter detects substring offset by shift places. Useful for detecting a command with parameter (i.e. "c_25").\n 
		 * Moves the receive buffer pointer back to the end of the substring if move_pointer is set to true.
		 *
		 * \param input char array to be detected
		 * \param shift offset of string from end of receive buffer (default = 0)
		 * \param move_pointer moves rd_index to end of detected string if shift is on (default = true) 
		 * \return if string has been received
		 * \note Input array must be null-character terminated.
		 * \sa Receive(), ReceiveParam(), ReceiveInt(), Task()
		 */
		bool ReceiveString(const char *input, uint32_t shift = 0u, bool move_pointer = true);
		/*!
		 * \brief Transmits unsigned int as ASCII string over USB.
		 *
		 * Transmits each digit as an ASCII string using tusb FIFO queue.
		 * 
		 * \param input raw value of integer to be transmitted as ASCII characters
		 * \return success of transmission
		 * \sa Transmit(), TransmitPacket(), TransmitString(), Task()
		 */
		bool TransmitInt(uint32_t input);
		/*!
		 * \brief Receives ASCII number across USB from host and returns unsigned int.
		 *
		 * Reads FIFO receive buffer until empty or non-numeric ASCII character received, and converts to unsigned int.
		 *
		 * \param output pointer to received int value
		 * \return success of reception (returns false if no integer was detected) 
		 * \sa Receive(), ReceiveParam(), ReceiveString(), Task()
		 */
		bool ReceiveInt(uint32_t * output);
		/*!
		 * \brief Checks if input string followed by an ASCII parameter has been received across USB from host and returns the ASCII parameter as unsigned int.
		 *
		 * Checks input string with front of receive buffer with offset [1, max_digits], and then returns the following ASCII number characters as unsigned int.\n 
		 * Call HasReceivedParam() immediately after call to ensure string has been received.\n 
		 * Example: ReceiveParam("c_") will output unsigned int 12u if "c_12" is sent via USB.\n 
		 * Can also be used multiple times for different strings with parameters and in conjunction with ReceiveString() calls.\n 
		 * Iterates rx read index.\n
		 * If a string is detected, all calls will be disabled until a new character is received.\n 
		 * delimiter parameter will detect an end-character if set to nonzero to allow for more secure message reading.\n 
		 * max_digits parameter will set a bound for the max amount of digits in the parameter. If max_digits > 9, the return value may not be what is expected.
		 *
		 * \param output pointer to received int value
		 * \param input input character array to be detected
		 * \param delimiter optional delimiting character after int parameter (default = '\0')
		 * \param max_digits maximum number of digits in parameter, must not exceed 9 (default = 8)
		 * \return success of reception (will return false if string value not detected, no integer detected, or no delimiter detected if set)
		 * \note Input array must be null-character terminated.
		 * \sa Receive(), ReceiveString(), ReceiveInt(), Task()
		 */
		bool ReceiveParam(uint32_t * output, const char *input, char delimiter = '\0', uint8_t max_digits = 8u);
		
		uint32_t GetBufferAvailable(void) const;	//!< Getter for number of unread characters available in FIFO receive buffer
		
		bool IsConnected(void) const;	//!< Retrieves state of connection (true = connected)	
		
		void Detach(void);				//!< Suspends USB device to go into low power mode	
		void Reattach(void);			//!< Reattaches suspended USB device
		
		
		private:
		//private helper functions
		void PutBuffer(char input);
		
		//private data members
		Serial::SerialBuffer usb_buffer;
		bool usb_on;
		bool detached;
	}; //USBController
}


#endif //__SERIAL_USB_H__
