/* 
 * Name				:	serial_spi.h
 * Created			:	10/13/2022 4:59:36 PM
 * Author			:	Aaron Reilman
 * Description		:	A SPI serial communication driver.
 */


#ifndef __SERIAL_SPI_H__
#define __SERIAL_SPI_H__

#include "serial_spi/spi_hal.h"
#include "serial_buffer/serial_buffer.h"

/*!
 * \brief %SPI controller global namespace.
 * 
 * This namespace contains all global classes, functions and definitions needed to implement controller for %SPI serial communication.
 */
namespace SerialSPI
{
	/*!
	 * \brief An enum class for controller MOSI/client MISO Interrupt Request State
	 */
	enum class TXIRQState
	{
		None,							//!< No transmit interrupts detected
		TXComplete,						//!< A data register empty interrupt was detected and no data wrote to SERCOM data buffer (transmission ended)
		TXSuccess						//!< A data register empty interrupt was detected and next data successfully wrote to SERCOM data buffer (transmission in progress)
	};
	/*!
	 * \brief An enum class for controller MISO/client MOSI Interrupt Request State
	 */
	enum class RXIRQState
	{
		None,							//!< No receive interrupts detected
		RXError,						//!< An error occurred in reception
		RXComplete,						//!< A receive complete interrupt was detected and data was not read from SERCOM data buffer (MISO buffer is full)
		RXSuccess						//!< A receive complete interrupt was detected and the data successfully read from SERCOM data buffer
	};
	/*!
	 * \brief An enum class for %SPI Errors
	 */
	enum class SPIError
	{
		ENone,							//!< No errors detected, data successfully transmitted/received
		EOverflow						//!< Overflow/overrun error in reception detected 
	};
	/*!
	 * \brief A status struct containing interrupt and error statuses, and %SPI power state
	 */
	struct Status {
		TXIRQState tx_interrupt;		//!< Last transmit interrupt detected
		RXIRQState rx_interrupt;		//!< Last receive interrupt detected
		SPIError error_state;			//!< Last error detected
		bool spi_on;					//!< Power state
	};
	/*!
	 * \brief A struct containing the buffer states of the MOSI and MISO buffers
	 */
	struct BufferStates {
		GenericBuffer::BufferState mosi_buffer_state;		//!< MOSI buffer state
		GenericBuffer::BufferState miso_buffer_state;		//!< MISO buffer state
	};
	/*!
	 * \brief %SPI serial communication controller object
	 *
	 * This is a %SPI serial communication controller which manages a single peripheral. It creates two FIFO queues for transmission and reception of data over %SPI and some simple parsing
	 * capabilities. This controller is interrupt driven, so ISR() must be implemented in the %SPI interrupt handler (pay attention to which SERCOM# you are using).
	 * The ISR provides interrupt and error flags and can be read using GetStatus() to implement your own error and interrupt handling outside of basic data transmission/reception.
	 */
	class SPIController
	{
		//functions
		public:
		/*!
		 * \brief Constructor
		 *
		 * Instantiates controller object for %SPI serial communication.\n 
		 * This will not initialize the hardware, Init() must be called before transmission and reception will do anything.
		 *
		 * \sa ~SPIController(), Init(), Deinit()
		 */
		SPIController(void);
		/*!
		 * \brief Destructor
		 *
		 * Called when SPIController object is destroyed. Immediately halts data transmission and resets peripheral hardware.
		 *
		 * \sa SPIController(), Init(), Deinit()
		 */
		~SPIController(void);
		/*!
		 * \brief Initializes %SPI serial communication.
		 *
		 * Uses five parameters for peripheral config and buffer sizes to initialize peripheral hardware, buffers, and statuses if %SPI hasn't been initialized.
		 * This function will not do anything if %SPI is currently enabled.
		 *
		 * \param p_config pointer to peripheral config information
		 * \param mosi_buf initialized character array used for receive/MOSI buffer
		 * \param mosi_size MOSI buffer size
		 * \param miso_buf initialized character array used for transmit/MISO buffer
		 * \param miso_size MISO buffer size
		 * \sa Deinit(), ResetMOSIBuffer(), ResetMISOBuffer(), EnableErrorIRQ()
		 */
		void Init(SPIHAL::Peripheral * p_config, char * mosi_buf, uint32_t mosi_size, char * miso_buf, uint32_t miso_size);
		/*!
		 * \brief Disables and resets %SPI serial communication.
		 *
		 * Disables %SPI serial controller if active and resets peripheral hardware. %SPI controller can be initialized again after calling this function without destroying entire object.
		 *
		 * \sa Init(), ClearBuffers(), ~SPIController()
		 */
		void Deinit(void);
		/*!
		 * \brief Resets MOSI/transmit buffer.
		 *
		 * Resets %SPI controller MOSI buffer to use a new input array and buffer size. Resets TXIRQ status.
		 *
		 * \param mosi_buf initialized character array used for transmit buffer
		 * \param mosi_size transmit buffer size
		 * \sa ResetMISOBuffer(), Init()
		 */
		void ResetMOSIBuffer(char * mosi_buf, uint32_t mosi_size);
		/*!
		 * \brief Resets MISO/receive buffer.
		 *
		 * Resets %SPI controller MISO buffer to use a new input array and buffer size. Resets RXIRQ status and error status.
		 *
		 * \param miso_buf initialized character array used for receive buffer
		 * \param miso_size receive buffer size
		 * \sa ResetMOSIBuffer(), Init()
		 */
		void ResetMISOBuffer(char * miso_buf, uint32_t miso_size);
		/*!
		 * \brief Clears MOSI and/or MISO buffers.
		 *
		 * Clears out the MOSI and/or MISO buffers and resets status data and indices.
		 * Useful for quick handling of full buffers and errors.
		 *
		 * \param clear_mosi clear MOSI/transmit buffer (default = true)
		 * \param clear_miso clear MISO/receive buffer (default = true)
		 * \sa Deinit()
		 */
		void ClearBuffers(bool clear_mosi = true, bool clear_miso = true);
		/*!
		 * \brief Interrupt Service Routine.
		 *
		 * Function to be called in corresponding SERCOM# interrupt handler. Sets IRQ and error statuses, populates MISO buffer, and transmits data from the MOSI buffer.\n 
		 * Interrupts can be read with GetStatus() to handle synchronization of events, check and handle errors, and generally assist with debugging.
		 *
		 * \sa GetStatus(), ClearMISOInterrupt(), ClearMOSIInterrupt(), ClearErrors()
		 */
		void ISR(void);
		/*!
		 * \brief Simple %SPI Procedure which performs a transaction between host and client.
		 *
		 * Performs a %SPI Transaction compatible with nearly any %SPI client. This transaction will pull down the SS line, send bytes, send \0 characters to read data from client, and pull up SS line.\n 
		 * This will return a char coinciding with a status byte sent by most %SPI clients.\n 
		 * Use Receive() to read data from the client.\n 
		 * Call ChangeSSL() before this function if using multiple clients.
		 *
		 * \param num_bytes number of bytes to send to client
		 * \param bytes array of bytes to send
		 * \param num_read number of bytes to read from client (default = 0)
		 * \return 0 or custom status byte defined by client
		 * \sa Transmit(), SSLow(), SSHigh(), Receive(), ChangeSSL()
		 */
		char SPIHostProcedure(uint32_t num_bytes, const char * bytes, uint32_t num_read = 0);
		/*!
		 * \brief Transmit character into logic layer transmit buffer to be sent to %SPI data buffer.
		 *
		 * Transmits character into logic layer MOSI buffer, updates MOSI writer pointer, status, number of available MOSI buffer slots.\n 
		 * Returns a true/false based on success of transmission. Transmission will fail if logic layer MOSI buffer is full.
		 *
		 * \param input character to be transmitted
		 * \return success of transmission
		 * \sa Receive(), SPIHostProcedure()
		 */
		bool Transmit(char input);
		/*!
		 * \brief Receive character from logic layer MISO buffer and free buffer space.
		 *
		 * Receives character from logic layer MISO buffer, updates MISO reader pointer, status, number of available MISO buffer slots.
		 *
		 * \param output pointer to received char (default = nullptr)  
		 * \return success of reception
		 * \sa Transmit(), SPIHostProcedure()
		 */
		bool Receive(char * output = nullptr);

		uint32_t GetMISOAvailable(void) const;			//!< Getter for number of unread characters available in MISO/receive buffer
		uint32_t GetMOSIEmpty(void) const;				//!< Getter for number of empty slots available in MOSI/transmit buffer
		BufferStates GetBufferStates(void) const;		//!< Get MISO and MOSI buffer states
		/*!
		 * \brief Getter for errors, interrupts, and power state.
		 *
		 * Useful for adding additional ISR responses in SERCOM handler.
		 *
		 * \sa ISR(), ClearMOSIInterrupt(), ClearMISOInterrupt(), ClearErrors()
		 */
		Status GetStatus(void) const;						
		
		void ClearMOSIInterrupt(void);						//!< Clears MOSI Interrupt Status
		void ClearMISOInterrupt(void);						//!< Clears MISO Interrupt Status
		void ClearErrors(void);								//!< Clears Error Status
			
		void EnableErrorIRQ(bool enable = true);			//!< Enabler for Error interrupt
		
		void SSHigh(void);									//!< Sets Slave Select Line to High (indicates end of transmission to slave device)
		void SSLow(void);									//!< Sets Slave Select Line to Low (indicates start of transmission to slave device)
		void ChangeSSL(SERCOMHAL::Pinout ssl_pinout);		//!< Change Slave Select Line pin (if using multiple clients)
		
		private:
		//private helper functions
		void PutMISOBuffer(char input);
		char GetMOSIBuffer(void);
		void HandleErrors(void);
		
		//private data members
		GenericBuffer::GENERIC_BUFFER<char> mosi_buffer;
		GenericBuffer::GENERIC_BUFFER<char> miso_buffer;
		SERCOMHAL::SercomID sercom_id;
		Status status;
		SERCOMHAL::Pinout ssl_pin;
		bool error_on;
	};	//SPIController
}

#endif //__SERIAL_SPI_H__
