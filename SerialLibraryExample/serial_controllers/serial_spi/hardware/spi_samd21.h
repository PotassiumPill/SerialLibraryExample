/* 
 * Name				:	spi_samd21.h
 * Created			:	10/13/2022 5:39:06 PM
 * Author			:	Aaron Reilman
 * Description		:	A SPI serial communication low level driver for samd21.
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
	enum ExtraParams : uint8_t {
		GEN_CLK,							//!< ID of generic clock used for %SPI
		GEN_CLK_DIVISOR_BITS_9_16,			//!< Clock speed divisor (bit 9:16)
		GEN_CLK_DIVISOR_BITS_1_8,			//!< Clock speed divisor (bit 1:8)
		GEN_CLK_RUN_STANDY,					//!< Clock on/off while in standby
		PAD_CONFIG							//!< Pad configuration for pins
	};
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
	
	void SetGenClkParam(SPIHAL::Peripheral * peripheral, SERCOMSAMD21::GenericClock generic_clock = SERCOMSAMD21::GenericClock::GEN_CLK_2);		//!< Setter for generic clock peripheral parameter
	void SetGenClkDivisorParam(SPIHAL::Peripheral * peripheral, uint16_t divisor = 0);															//!< Setter for generic clock divisor peripheral parameter
	void SetGenClkRunStandbyParam(SPIHAL::Peripheral * peripheral, bool run_standy = false);													//!< Setter for generic clock run in standby peripheral parameter
	void SetPadConfigParam(SPIHAL::Peripheral * peripheral, PadConfig pad_config = PadConfig::DO2_DI0_SCK3_CSS1);								//!< Setter for pad config peripheral parameter
}

#endif //__SPI_SAMD21_H__
