/* 
 * Name				:	serial_uart.h
 * Created			:	6/8/2022 11:12:06 AM
 * Author			:	Aaron Reilman
 * Description		:	A UART serial communication driver.
 */


#ifndef __SERIAL_UART_H__
#define __SERIAL_UART_H__

#include "serial_uart/uart_config.h"

#include "serial_uart/uart_hal.h"
#include "serial_buffer/serial_buffer.h"

/*!
 * \brief %UART controller global namespace.
 * 
 * This namespace contains all global classes, functions and definitions needed to implement controller for UART serial communication.
 */
namespace SerialUART
{
	/*!
	 * \brief An enum class for Transmit Interrupt Request State
	 */
	enum class TXIRQState
	{
		None,					//!< No transmit interrupts detected
		TxComplete,				//!< A data register empty interrupt was detected and no data wrote to SERCOM data buffer (transmission ended)
		TxSuccess				//!< A data register empty interrupt was detected and next data successfully wrote to SERCOM data buffer (transmission in progress)
	};
	/*!
	 * \brief An enum class for Receive Interrupt Request State
	 */
	enum class RXIRQState
	{
		None,					//!< No receive interrupts detected
		RxError,				//!< An error occurred in reception
		RxComplete,				//!< A receive complete interrupt was detected and data was not read from SERCOM data buffer (receive buffer is full)
		RxSuccess				//!< A receive complete interrupt was detected and the data successfully read from SERCOM data buffer
	};
	/*!
	 * \brief An enum class for UART Errors
	 */
	enum class UARTError
	{
		ENone,					//!< No errors detected, data successfully transmitted/received
		ESync,					//!< Sync error in reception detected
		EOverflow,				//!< Overflow/overrun error in reception detected 
		EFrame,					//!< Frame error in reception detected
		EParity					//!< Parity error in reception detected
	};
	/*!
	 * \brief A status struct containing interrupt and error statuses, and UART power state
	 */
	struct Status {
		TXIRQState tx_interrupt;		//!< Last transmit interrupt detected
		RXIRQState rx_interrupt;		//!< Last receive interrupt detected
		UARTError error_state;			//!< Last error detected
		bool uart_on;					//!< Power state
	};
	/*!
	 * \brief A struct containing the buffer states of the transmit and receive buffers
	 */
	struct BufferStates {
		GenericBuffer::BufferState tx_buffer_state;		//!< Transmit buffer state
		GenericBuffer::BufferState rx_buffer_state;		//!< Receive buffer state
	};
	/*!
	 * \brief %UART serial communication controller object
	 *
	 * This is a %UART serial communication controller which manages a single peripheral. It creates two FIFO queues for transmission and reception of data over UART and some simple parsing
	 * capabilities. This controller is interrupt driven, so ISR() must be implemented in the UART interrupt handler (pay attention to which SERCOM# you are using).
	 * The ISR provides interrupt and error flags and can be read using GetStatus() to implement your own error and interrupt handling outside of basic data transmission/reception.
	 */
	class UARTController
	{
		//functions
		public:
		/*!
		 * \brief Constructor
		 *
		 * Instantiates controller object for UART serial communication.\n 
		 * This will not initialize the hardware, Init() must be called before transmission and reception will do anything.
		 *
		 * \sa ~UARTController(), Init(), Deinit()
		 */
		UARTController(void);
		/*!
		 * \brief Destructor
		 *
		 * Called when UARTController object is destroyed. Immediately halts data transmission and resets peripheral hardware.
		 *
		 * \sa UARTController(), Init(), Deinit()
		 */
		~UARTController(void);
		/*!
		 * \brief Initializes UART serial communication.
		 *
		 * Uses five parameters for peripheral config and buffer sizes to initialize peripheral hardware, buffers, and statuses if UART hasn't been initialized.\n 
		 * This function will not do anything if UART is currently enabled.
		 *
		 * \param p_config pointer to peripheral config information
		 * \param rx_buf initialized character array used for receive buffer
		 * \param rx_size receive buffer size
		 * \param rx_buf initialized character array used for transmit buffer
		 * \param tx_size transmit buffer size
		 * \sa Deinit(), ResetTXBuffer(), ResetRXBuffer(), EnableErrorIRQ()
		 */
		void Init(UARTHAL::Peripheral * p_config, char * tx_buf, uint32_t tx_size, char * rx_buf, uint32_t rx_size);
		/*!
		 * \brief Disables and resets UART serial communication.
		 *
		 * Disables UART serial controller if active and resets peripheral hardware. UART controller can be initialized again after calling this function without destroying entire object.
		 *
		 * \sa Init(), ClearBuffers(), ~UARTController()
		 */
		void Deinit(void);
		/*!
		 * \brief Resets transmit buffer.
		 *
		 * Resets UART controller transmit buffer to use a new input array and buffer size. Resets TXIRQ status.
		 *
		 * \param tx_buf initialized character array used for transmit buffer
		 * \param tx_size transmit buffer size
		 * \sa ResetRXBuffer(), Init()
		 */
		void ResetTXBuffer(char * tx_buf, uint32_t tx_size);
		/*!
		 * \brief Resets receive buffer.
		 *
		 * Resets UART controller receive buffer to use a new input array and buffer size. Resets RXIRQ status and error status.
		 *
		 * \param rx_buf initialized character array used for receive buffer
		 * \param rx_size receive buffer size
		 * \sa ResetTXBuffer, Init()
		 */
		void ResetRXBuffer(char * rx_buf, uint32_t rx_size);
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
		 * \brief Interrupt Service Routine.
		 *
		 * Function to be called in corresponding SERCOM# interrupt handler. Sets IRQ and error statuses, populates receive buffer, and transmits data from the transmit buffer.\n 
		 * Interrupts can be read with GetStatus() to handle synchronization of events, check and handle errors, and generally assist with debugging.
		 *
		 * \sa GetStatus(), ClearTXInterrupt(), ClearRXInterrupt(), ClearErrors()
		 */
		void ISR(void);	
		/*!
		 * \brief Transmit character into logic layer transmit buffer to be sent to USART data buffer.
		 *
		 * Transmits character into logic layer transmit buffer, updates tx writer pointer, status, number of available tx buffer slots.\n 
		 * Returns a true/false based on success of transmission. Transmission will fail if logic layer transmit buffer is full.
		 *
		 * \param input character to be transmitted
		 * \return success of transmission
		 * \sa TransmitString(), TransmitInt(), EchoRx()
		 */
		bool Transmit(char input);
		/*!
		 * \brief Receive character from logic layer receive buffer and free buffer space.
		 *
		 * Receives character from logic layer receive buffer, updates rx reader pointer, status, number of available rx buffer slots.
		 *
		 * \param output pointer to received char (default = nullptr)  
		 * \return success of reception
		 * \sa ReceiveString(), ReceiveInt(), ReceiveParam(), EchoRx()
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
		 * \sa Transmit(), TransmitString(), TransmitInt()
		 */
		bool TransmitPacket(const char * input, uint32_t num_bytes);
		/*!
		 * \brief Transmit an entire char array at once.
		 *
		 * Adds an entire char array to the logic layer transmit buffer at once.\n 
		 * Returns true/false based on success of transmission.
		 * 
		 * \param input char array to be transmitted
		 * \return number of bytes sent
		 * \note Input array must be null-character terminated.
		 * \sa Transmit(), TransmitInt()
		 */
		uint32_t TransmitString(const char *input);
		/*!
		 * \brief Transmits char from the receive buffer.
		 *
		 * Reads from logic layer receive buffer and adds read char into logic layer transmit buffer. Ideally used to echo received data onto serial terminal.\n 
		 * Returns true/false based on success of transmission. Transmission will fail if instance is disabled, TX buffer is full, or RX buffer is empty.
		 *
		 * \return success of transmission
		 * \note This function is ISR friendly.
		 * \sa Transmit(), Receive()
		 */
		bool EchoRx(void);
		/*!
		 * \brief Checks if input string has been received from USART.
		 *
		 * Checks input string with front of receive buffer, returning true if the input string has been detected.\n 
		 * Can be used multiple times for different strings.\n 
		 * Iterates rx read index.\n 
		 * If a string is detected, all calls will be disabled until a new character is received.\n 
		 * Shift parameter detects substring offset by shift places. Useful for detecting a command with parameter (i.e. "c_25").\n 
		 * Moves the receive buffer pointer back to the end of the substring if move_pointer is set to true.
		 *
		 * \param input char array to be detected
		 * \param shift offset of string from end of receive buffer (default = 0)
		 * \param move_pointer moves rx_rd_index to end of detected string if shift is on (default = true) 
		 * \return if string has been received
		 * \note Input array must be null-character terminated.
		 * \sa Receive(), ReceiveParam(), ReceiveInt()
		 */
		bool ReceiveString(const char *input, uint32_t shift = 0u, bool move_pointer = true);
		/*!
		 * \brief Transmits unsigned int as ASCII string.
		 *
		 * Transmits each digit as an ASCII string.\n 
		 * Returns false is UART is off.
		 * 
		 * \param input raw value of integer to be transmitted as ASCII characters
		 * \return success of transmission
		 * \sa Transmit(), TransmitString()
		 */
		bool TransmitInt(uint32_t input);
		/*!
		 * \brief Receives ASCII number and outputs unsigned int.
		 *
		 * Reads receive buffer until empty or non-numeric ASCII character received, and converts to unsigned int.
		 *
		 * \param output pointer to received int value
		 * \return success of reception (returns false if no integer was detected) 
		 * \sa Receive(), ReceiveParam(), ReceiveString()
		 */
		bool ReceiveInt(uint32_t * output);
		/*!
		 * \brief Checks if input string followed by an ASCII parameter has been received from USART and returns the ASCII parameter as unsigned int.
		 *
		 * Checks input string with front of receive buffer with offset [1, max_digits], and then returns the following ASCII number characters as unsigned int.\n 
		 * Call HasReceivedParam() immediately after call to ensure string has been received.\n 
		 * Example: ReceiveParam("c_") will output unsigned int 12u if "c_12" is sent via USART.\n 
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
		 * \sa Receive(), ReceiveString(), ReceiveInt()
		 */
		bool ReceiveParam(uint32_t * output, const char *input, char delimiter = '\0', uint8_t max_digits = 8u);

		uint32_t GetRXAvailable(void) const;			//!< Getter for number of unread characters available in receive buffer
		uint32_t GetTXEmpty(void) const;				//!< Getter for number of empty slots available in transmit buffer
		BufferStates GetBufferStates(void) const;		//!< Get receive and transmit buffer states
		/*!
		 * \brief Getter for errors, interrupts, and power state.
		 *
		 * Useful for adding additional ISR responses in SERCOM handler.
		 *
		 * \sa ISR(), ClearTXInterrupt(), ClearRXInterrupt(), ClearErrors()
		 */
		Status GetStatus(void) const;						
		
		void ClearTXInterrupt(void);						//!< Clears TX Interrupt Status
		void ClearRXInterrupt(void);						//!< Clears RX Interrupt Status
		void ClearErrors(void);								//!< Clears Error Status
		
		void EnableErrorIRQ(bool enable = true);			//!< Enabler for Error interrupt
				
		private:
		//private helper functions
		void PutRXBuffer(char input);
		char GetTXBuffer(void);
		void HandleErrors(void);
		
		//private data members
		GenericBuffer::GENERIC_BUFFER<char> tx_buffer;
		Serial::SerialBuffer rx_buffer;
		Status status;
		bool error_on;
	}; //UARTController
}

#endif //__SERIAL_UART_H__
