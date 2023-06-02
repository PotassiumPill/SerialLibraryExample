/*
 * Name				:	common_hal.h
 * Created			:	1/18/2023 4:24:03 PM
 * Author			:	Aaron Reilman
 * Description		:	Common generic hardware functionality for implementing serial controllers.
 */ 


#ifndef __COMMON_HAL_H__
#define __COMMON_HAL_H__

#include <stdint.h>

/*!
 * \brief %SERCOM HAL global namespace.
 * 
 * This namespace contains all global functions and definitions needed to implement low level drivers for common serial communication functions.
 */
namespace SERCOMHAL
{
	/*! 
	 * \brief An enum type for type of pull resistance on pin
	 */
	enum class PullResistor {
		NoPull,					//!< No pull resistor (output/regular pin type)
		PinPullUp,				//!< Internal pull-up
		PinPullDown				//!< Internal pull-down
	};
	/*!
	 * \brief Type definition for SercomID, used for picking the ID of the SERCOM# to setup for serial communication. 
	 */
	typedef uint8_t SercomID;
	/*!
	 * \brief Type definition for Peripheral Function, used as an ID for a multiplexed pin's hardware function.
	 */
	typedef uint8_t PeripheralFunction;
	/*!
	 * \brief Type definition for PinPort, used as an ID for a pin's port.
	 */
	typedef uint8_t PinPort;
	/*!
	 * \brief A pinout struct to configure I/O pins with or without multiplexing.
	 *
	 * This struct contains all of the pinout information needed to configure I/O pins for serial communication or other uses.
	 */
	struct Pinout {
		PeripheralFunction peripheral_function;		//!< ID of the specific peripheral function (if feature exists) to use.
		PinPort port;									//!< ID of the specific port to use.
		uint8_t pin;								//!< Specific pin number to use.
	};
	/*!
	 * \brief Configures a pin based on location, input/output, and multiplexing
	 *
	 * Configures a pin including its input/output and multiplexing.
	 *
	 * \param pin pinout
	 * \param output true if output, false if input
	 * \param multiplexed configures as multiplexed I/O pin if set true
	 * \param pull enable pull resistor type (default = NoPull)
	 */
	void ConfigPin(Pinout pin, bool output, bool multiplexed, PullResistor pull = PullResistor::NoPull);
	/*!
	 * \brief Gets pin state
	 *
	 * Gets state of an input pin (high/low).
	 *
	 * \param pin pinout of configured input pin
	 * \return true if high, false if low
	 */
	bool GetPinState(Pinout pin);
	/*!
	 * \brief Sets output pin to high
	 *
	 * \param output_pin pinout of configured output pin
	 *
	 * \note Must configure pin as an output to use
	 */
	void OutputHigh(Pinout output_pin);
	/*!
	 * \brief Sets output pin to low
	 *
	 * \param output_pin pinout of configured output pin
	 *
	 * \note Must configure pin as an output to use
	 */
	void OutputLow(Pinout output_pin);
}

#endif //__COMMON_HAL_H__
