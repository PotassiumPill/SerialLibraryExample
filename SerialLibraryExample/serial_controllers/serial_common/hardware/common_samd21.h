/* 
 * Name				:	common_samd21.h
 * Created			:	1/18/2023 4:40:56 PM
 * Author			:	Aaron Reilman
 * Description		:	Common generic hardware functionality for SAMD21.
 */


#ifndef __COMMON_SAMD21_H__
#define __COMMON_SAMD21_H__

#include "serial_common/common_hal.h"

#include "sam.h"

/*!
 * \brief %SERCOM SAMD21 low level driver global namespace
 *
 * This namespace contains all constants and helper functions for implementing the low level drivers and peripherals on the SAMD21 chip. 
 */
namespace SERCOMSAMD21
{
	/*!
	 * \brief An enum type for Generic Clock source on SAMD21.
	 */
	enum ClockSource : uint8_t {
		CS_XOSC, CS_GCKLIN, CS_GCLKGEN1, CS_OSCULP32K, CS_OSC32K, CS_XOSC32K, CS_OSC8M, CS_DFLL48M, CS_FDPLL96M
	};
	/*!
	 * \brief An enum type for Generic Clock Generator on SAMD21.
	 */
	enum GenericClock : uint8_t {
		GEN_CLK_0,
		GEN_CLK_1,
		GEN_CLK_2,
		GEN_CLK_3,
		GEN_CLK_4,
		GEN_CLK_5,
		GEN_CLK_6,
		GEN_CLK_7,
		GEN_CLK_8
	};
	/*!
	 * \brief An enum type for SERCOM# configuration on SAMD21.
	 */
	enum SercomID : uint8_t {
		Sercom0,
		Sercom1,
		Sercom2,
		Sercom3,
		Sercom4,
		Sercom5
	};
	/*!
	 * \brief An enum type for pin peripheral function on multiplexed pins on SAMD21.
	 */
	enum PeripheralFunction : uint8_t {
		PF_NONE = 0, PF_A = 0, PF_B, PF_C, PF_D, PF_E, PF_F, PF_G, PF_H
	};
	/*!
	 * \brief An enum type for a pin's port on SAMD21.
	 */	
	enum Port : uint8_t {
		PORT_A, PORT_B
	};
	/*!
	 * \brief Helper function for getting the Sercom struct address on the SAMD21 from the peripheral SERCOM ID
	 *
	 * Reads the peripheral SERCOM ID and returns the Sercom pointer so SAMD21 low level drivers can be configured.
	 *
	 * \param sercom_id peripheral SERCOM# on hardware to be configured
	 * \return Sercom struct pointer containing the register address of the corresponding SERCOM (0-5)
	 */
	Sercom * GetSercom(SERCOMHAL::SercomID sercom_id);
	/*!
	 * \brief Sercom clock enabler
	 *
	 * Sercom clock generator using 8 MHz oscillator on SAMD21. Includes ability to use clock divisor to decrease frequency.
	 *
	 * \param sercom_id SERCOM# to feed clock to
	 * \param clock_source generic clock source (must be enabled)
	 * \param gen_clk_num generic clock generator to use
	 * \param clock_divisor clock divisor, refer to datasheet for restrictions and valid divisors to use (default = 0)
	 * \param run_standby run clock while in sleep standby mode (default = false)
	 */
	void EnableSercomClock(SERCOMHAL::SercomID sercom_id, ClockSource clock_source, GenericClock gen_clk_num, uint16_t clock_divisor = 0, bool run_standby = false);
}; 

#endif //__COMMON_SAMD21_H__
