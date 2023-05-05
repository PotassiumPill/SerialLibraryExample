/*
 * Name			:	common_hal.h
 * Created		:	1/18/2023 4:24:03 PM
 * Author		:	Aaron Reilman
 * Description	:	Common generic hardware functionality for implementing serial controllers.
 */ 


#ifndef __COMMON_HAL_H__
#define __COMMON_HAL_H__

#include "serial_comm_config.h"
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
	 * \brief A pinout struct to configure I/O pins with or without multiplexing.
	 *
	 * This struct contains all of the pinout information needed to configure I/O pins for serial communication or other uses.
	 */
	struct Pinout {
		uint8_t peripheral_function;	//!< Numeric ID of the specific peripheral function to use.
		uint8_t port;					//!< Numeric ID of the specific port (usually A/B) to use.
		uint8_t pin;					//!< Numeric ID of the specific pin number to use.
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

#if (SERCOM_MCU_OPT == OPT_SERCOM_SAMD21)
#include "serial_common/hardware/common_samd21.h"
#endif

#endif //__COMMON_HAL_H__
