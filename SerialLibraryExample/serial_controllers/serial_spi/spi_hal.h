/* 
 * Name				:	spi_hal.h
 * Created			:	10/13/2022 4:59:17 PM
 * Author			:	Aaron Reilman
 * Description		:	A SPI Serial Communication Hardware Abstraction Layer.
 */


#ifndef __SPI_HAL_H__
#define __SPI_HAL_H__

#include "serial_common/common_hal.h"

#ifndef NUM_EXTRA_SPI_PARAMS
#define NUM_EXTRA_SPI_PARAMS 1
#endif

/*!
 * \brief %SPI HAL global namespace.
 * 
 * This namespace contains all global functions and definitions needed to implement low level drivers for %SPI serial communication.
 */
namespace SPIHAL
{
	/*!
	 * \brief An enum type for the %SPI transfer mode
	 */
	enum class ClockMode {
		Mode0,					//!< Data is sampled on rising leading edge, data is changed on falling trailing edge
		Mode1,					//!< Data is changed on rising leading edge, data is sampled on falling trailing edge
		Mode2,					//!< Data is sampled on falling leading edge, data is changed on rising trailing edge
		Mode3					//!< Data is changed on falling leading edge, data is sampled on rising trailing edge
	};
	/*!
	 * \brief Type definition for PadConfig, used for configuring the pads of SERCOM I/O pins.
	 */
	typedef uint8_t PadConfig;
	/*!
	 * \brief An enum type for how data is shifted out of data buffer
	 */
	enum class Endian {
		MSB,					//!< Most significant bit is shifted out first
		LSB						//!< Least significant bit is shifted out first
	};
	/*!
	 * \brief A peripheral struct containing peripheral setup information.
	 *
	 * This struct contains all basic information needed to configure hardware for %SPI serial communication.
	 */
	struct Peripheral {
		SERCOMHAL::SercomID sercom_id;							//!< SERCOM# used in communication
		SERCOMHAL::Pinout mosi_pin;								//!< MOSI (Master Out Slave In) I/O Pinout
		SERCOMHAL::Pinout miso_pin;								//!< MISO (Master In Slave Out) I/O Pinout
		SERCOMHAL::Pinout sck_pin;								//!< SCK (Serial Clock) I/O Pinout
		SERCOMHAL::Pinout ssl_pin;								//!< SSL (Slave Select Line) I/O Pinout
		uint32_t baud_value;									//!< Baud rate of %SPI transmission
		ClockMode clock_mode;									//!< Clock mode of %SPI transmission
		Endian endianess;										//!< Order of data bit shifting
		uint8_t extra_spi_params[NUM_EXTRA_SPI_PARAMS];			//!< Extra parameters for hardware specific configurations (such as oversampling, clock parameters, etc.)
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
	 * \brief Initializing function for %SPI serial communication low level drivers.
	 *
	 * Initializes the hardware to use %SPI, setting the I/O multiplexing, baud rate, feeding clocks, enabling interrupts, and enabling SERCOM MISO and MOSI data.
	 * This %SPI HAL sets the hardware into master/host mode.
	 *
	 * \param peripheral pointer to peripheral information struct
	 * \param is_client configures %SPI peripheral as client if set true (default = false)
	 */
	void InitSercom(Peripheral * peripheral, bool is_client = false);
	/*!
	 * \brief Tear down function for %SPI serial communication low level drivers.
	 *
	 * Turns off %SPI interrupts and disables SERCOM %SPI.
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
	 * \brief Enables or disables SERCOM transmit complete interrupt.
	 *
	 * \param sercom_id peripheral SERCOM# on hardware
	 * \param enable enable/disable interrupt (default = true)
	 */
	void EnableTxComplete(SERCOMHAL::SercomID sercom_id, bool enable = true);
	/*!
	 * \brief Checks if %SPI select low occurred (used in slave mode)
	 *
	 * \param sercom_id peripheral SERCOM# on hardware
	 * \return if %SPI select low occurred (used in slave mode).
	 */
	bool SPISelectLow(SERCOMHAL::SercomID sercom_id);
	/*!
	 * \brief Enables or disables SERCOM %SPI select low interrupt.
	 *
	 * \param sercom_id peripheral SERCOM# on hardware
	 * \param enable enable/disable interrupt (default = true)
	 */
	void EnableSPISelectLow(SERCOMHAL::SercomID sercom_id, bool enable = true);
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
	 * \return if error occurred
	 */
	bool SercomHasErrors(SERCOMHAL::SercomID sercom_id);	
	/*!
	 * \brief Checks if frame error occurred.
	 *
	 * \param sercom_id peripheral SERCOM# on hardware
	 * \return if frame error occurred.
	 */
	bool CheckOverflowError(SERCOMHAL::SercomID sercom_id);
}

#endif //__SPI_HAL_H__
