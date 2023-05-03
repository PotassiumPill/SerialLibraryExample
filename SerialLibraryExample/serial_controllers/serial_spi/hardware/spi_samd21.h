/* 
 * Name			:	spi_samd21.h
 * Created		:	10/13/2022 5:39:06 PM
 * Author		:	Aaron Reilman
 * Description	:	A SPI serial communication low level driver for samd21.
 */


#ifndef __SPI_SAMD21_H__
#define __SPI_SAMD21_H__

#include "sam.h"

/*!
 * \brief %SPI SAMD21 low level driver global namespace
 *
 * This namespace contains all constants and helper functions for implementing the low level drivers and peripherals on the SAMD21 chip. 
 */
namespace SPISAMD21
{
	/*!
	 * \brief An enum type for for I/O Pad Configuration of MISO, MOSI, and SCK pins
	 */
	enum PadConfig : uint8_t {
		DO0_DI2_SCK1_CSS2,			//!< Data Out pin on pad 0, Data In pin on pad 2, SCK pin on pad 1, Client SS on pad 2 (if in client mode)
		DO0_DI3_SCK1_CSS2,			//!< Data Out pin on pad 0, Data In pin on pad 3, SCK pin on pad 1, Client SS on pad 2 (if in client mode)
		DO0_DI1_SCK3_CSS1,			//!< Data Out pin on pad 0, Data In pin on pad 1, SCK pin on pad 3, Client SS on pad 1 (if in client mode)
		DO0_DI2_SCK3_CSS1,			//!< Data Out pin on pad 0, Data In pin on pad 2, SCK pin on pad 3, Client SS on pad 1 (if in client mode)
		DO2_DI0_SCK3_CSS1,			//!< Data Out pin on pad 2, Data In pin on pad 0, SCK pin on pad 3, Client SS on pad 1 (if in client mode)
		DO2_DI1_SCK3_CSS1,			//!< Data Out pin on pad 2, Data In pin on pad 1, SCK pin on pad 3, Client SS on pad 1 (if in client mode)
		DO3_DI0_SCK1_CSS2,			//!< Data Out pin on pad 3, Data In pin on pad 0, SCK pin on pad 1, Client SS on pad 2 (if in client mode)
		DO3_DI2_SCK1_CSS2			//!< Data Out pin on pad 3, Data In pin on pad 2, SCK pin on pad 1, Client SS on pad 2 (if in client mode)
	};
	/*!
	 * \brief Populates a peripheral struct with default values
	 *
	 * Populates peripheral struct with default values at specific SERCOM# on SAMD21.
	 *
	 * \param peripheral pointer to peripheral struct to populate with default values
	 * \param sercom_id SERCOM# to be configured as %SPI (default = Sercom4)
	 */
	void GetPeripheralDefaults(SPIHAL::Peripheral * peripheral, SERCOMHAL::SercomID sercom_id = SERCOMSAMD21::SercomID::Sercom4);
	/*!
	 * \brief Sets values to configure the generic clock generator before enabling %SPI
	 *
	 * Sets values to configure the generic clock generator used to feed into peripheral.
	 *
	 * \param gen_clk_id generic clock ID (default = GEN_CLK_3)
	 * \param clock_divisor division factor to decrease frequency of clock (default = 0)
	 * \param run_standby run clock while in standby mode (default = false)
	 */
	void SetGenClk(SERCOMSAMD21::GenericClock gen_clk_id = SERCOMSAMD21::GenericClock::GEN_CLK_2, uint16_t clock_divisor = 0, bool run_standby = false);
}

#endif //__SPI_SAMD21_H__
