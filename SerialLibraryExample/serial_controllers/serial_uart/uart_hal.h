/* 
 * Name				:	uart_hal.h
 * Created			:	6/23/2022 11:08:30 AM
 * Author			:	Aaron Reilman
 * Description		:	A UART serial communication Hardware Abstraction Layer.
 */


#ifndef __UART_HAL_H__
#define __UART_HAL_H__

#include "serial_common/common_hal.h"

#ifndef NUM_EXTRA_UART_PARAMS
#define NUM_EXTRA_UART_PARAMS 1
#endif

/*!
 * \brief %UART HAL global namespace.
 * 
 * This namespace contains all global functions and definitions needed to implement low level drivers for UART serial communication.
 */
namespace UARTHAL
{
	/*!
	 * \brief An enum type for how data is shifted out of data buffer
	 */
	enum class Endian {
		MSB,							//!< Most significant bit is shifted out first
		LSB								//!< Least significant bit is shifted out first
	};
	/*!
	 * \brief An enum type for the number of stop bits in data transmission
	 */
	enum class StopBits {
		OneStopBit,						//!< One stop bit in data transmission
		TwoStopBits						//!< Two stop bits in data transmission
	};
	/*!
	 * \brief An enum class for data parity type.
	 */	
	enum class Parity {
		PNone,							//!< No parity
		PEven,							//!< Even parity
		POdd							//!< Odd parity
	};
	/*!
	 * \brief A peripheral struct containing peripheral setup information.
	 *
	 * This struct contains all basic information needed to configure hardware for UART serial communication.
	 */
	struct Peripheral {	
		SERCOMHAL::SercomID sercom_id;							//!< SERCOM# used in communication
		SERCOMHAL::Pinout tx_pin;								//!< TX (Transmit Data) I/O Pinout
		SERCOMHAL::Pinout rx_pin;								//!< RX (Receive Data) I/O Pinout
		uint32_t baud_value;									//!< Baud rate of UART transmission
		Parity parity;											//!< Parity type in UART transmission
		Endian endianness;										//!< Order of data bit shifting
		StopBits num_stop_bits;									//!< Number of stop bits in data transmission
		uint8_t extra_uart_params[NUM_EXTRA_UART_PARAMS];		//!< Extra parameters for hardware specific configurations (such as oversampling, clock parameters, etc.)
	};
	/*!
	 * \brief Populates a peripheral struct with default values
	 *
	 * Populates peripheral struct with default values as determined by specific MCU.
	 *
	 * \param peripheral pointer to peripheral struct to populate with default values
	 */
	void GetPeripheralDefaults(Peripheral * peripheral);
	/*!
	 * \brief Initializing function for UART serial communication low level drivers.
	 *
	 * Initializes the hardware to use USART, setting the I/O multiplexing, baud rate, feeding clocks, enabling interrupts, and enabling SERCOM tx and rx data.
	 *
	 * \param peripheral pointer to peripheral information struct
	 */
	void InitSercom(Peripheral * peripheral);
	/*!
	 * \brief Tear down function for UART serial communication low level drivers.
	 *
	 * Turns off USART interrupts and disables SERCOM USART.
	 *
	 * \param sercom_id peripheral SERCOM# on hardware
	 */
	void DeinitSercom(SERCOMHAL::SercomID sercom_id);
	/*!
	 * \brief Checks if SERCOM data register is empty and therefore ready for transmission.
	 *
	 * \param sercom_id peripheral SERCOM# on hardware
	 * \return if ready to transmit
	 */
	bool ReadyToTransmit(SERCOMHAL::SercomID sercom_id);
	/*!
	 * \brief Checks if receive has completed and therefore SERCOM data register is ready to be read.
	 *
	 * \param sercom_id peripheral SERCOM# on hardware
	 * \return if ready to receive
	 */
	bool ReadyToReceive(SERCOMHAL::SercomID sercom_id);
	/*!
	 * \brief Reads SERCOM data register
	 *
	 * \param sercom_id peripheral SERCOM# on hardware
	 * \return value of SERCOM data register
	 */
	uint8_t GetSercomRX(SERCOMHAL::SercomID sercom_id);
	/*!
	 * \brief Writes to SERCOM data register
	 *
	 * \param input value to be written
	 * \param sercom_id peripheral SERCOM# on hardware
	 */
	void SetSercomTX(uint8_t input, SERCOMHAL::SercomID sercom_id);
	/*!
	 * \brief Enables or disables SERCOM data register empty interrupt.
	 *
	 * \param sercom_id peripheral SERCOM# on hardware
	 * \param enable enable/disable interrupt (default = true)
	 */
	void EnableTxEmpty(SERCOMHAL::SercomID sercom_id, bool enable = true);
	/*!
	 * \brief Enables or disables SERCOM receive complete interrupt.
	 *
	 * \param sercom_id peripheral SERCOM# on hardware
	 * \param enable enable/disable interrupt (default = true)
	 */
	void EnableRxFull(SERCOMHAL::SercomID sercom_id, bool enable = true);
	/*!
	 * \brief Checks if transmission has completed.
	 *
	 * \param sercom_id peripheral SERCOM# on hardware
	 * \return if transmission is complete
	 */
	bool TransmitComplete(SERCOMHAL::SercomID sercom_id);
	/*!
	 * \brief Checks if data receive has started.
	 *
	 * \param sercom_id peripheral SERCOM# on hardware
	 * \return if receive has started
	 */
	bool ReceiveStart(SERCOMHAL::SercomID sercom_id);
	/*!
	 * \brief Checks if clear to send input change occurred.
	 *
	 * \param sercom_id peripheral SERCOM# on hardware
	 * \return if clear to send occurred.
	 */
	bool ClearToSend(SERCOMHAL::SercomID sercom_id);
	/*!
	 * \brief Enables or disables SERCOM transmit complete interrupt.
	 *
	 * \param sercom_id peripheral SERCOM# on hardware
	 * \param enable enable/disable interrupt (default = true)
	 */
	void EnableTxComplete(SERCOMHAL::SercomID sercom_id, bool enable = true);
	/*!
	 * \brief Enables or disables SERCOM receive start interrupt.
	 *
	 * \param sercom_id peripheral SERCOM# on hardware
	 * \param enable enable/disable interrupt (default = true)
	 */
	void EnableRxStart(SERCOMHAL::SercomID sercom_id, bool enable = true);
	/*!
	 * \brief Enables or disables SERCOM clear to send interrupt.
	 *
	 * \param sercom_id peripheral SERCOM# on hardware
	 * \param enable enable/disable interrupt (default = true)
	 */
	void EnableClearToSend(SERCOMHAL::SercomID sercom_id, bool enable = true);
	/*!
	 * \brief Enables or disables SERCOM error interrupt.
	 *
	 * \param sercom_id peripheral SERCOM# on hardware
	 * \param enable enable/disable interrupt (default = true)
	 */
	void EnableSercomErrors(SERCOMHAL::SercomID sercom_id, bool enable = true);
	/*!
	 * \brief Checks if error occurred.
	 *
	 * \param sercom_id peripheral SERCOM# on hardware
	 * \return if error occurred.
	 */
	bool SercomHasErrors(SERCOMHAL::SercomID sercom_id);
	/*!
	 * \brief Checks if collision error occurred. Also handles hardware reconfiguration in case of collision.
	 *
	 * \param sercom_id peripheral SERCOM# on hardware
	 * \return if collision occurred.
	 */
	bool CheckCollision(SERCOMHAL::SercomID sercom_id);
	/*!
	 * \brief Checks if sync error occurred.
	 *
	 * \param sercom_id peripheral SERCOM# on hardware
	 * \return if sync error occurred.
	 */
	bool CheckSyncError(SERCOMHAL::SercomID sercom_id);
	/*!
	 * \brief Checks if overflow/overrun occurred.
	 *
	 * \param sercom_id peripheral SERCOM# on hardware
	 * \return if overflow/overrun error occurred.
	 */
	bool CheckOverflowError(SERCOMHAL::SercomID sercom_id);	
	/*!
	 * \brief Checks if frame error occurred.
	 *
	 * \param sercom_id peripheral SERCOM# on hardware
	 * \return if frame error occurred.
	 */
	bool CheckFrameError(SERCOMHAL::SercomID sercom_id);
	/*!
	 * \brief Checks if parity error occurred.
	 *
	 * \param sercom_id peripheral SERCOM# on hardware
	 * \return if parity error occurred.
	 */
	bool CheckParityError(SERCOMHAL::SercomID sercom_id);
}

#endif //__UART_HAL_H__
