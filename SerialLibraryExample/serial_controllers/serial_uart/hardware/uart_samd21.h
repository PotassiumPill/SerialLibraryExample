/*
 * Name				:	uart_samd21.h
 * Created			:	7/1/2022 10:16:02 AM
 * Author			:	Aaron Reilman
 * Description		:	A UART serial communication low level driver for samd21.
 */ 


#ifndef __UART_SAMD21_H__
#define __UART_SAMD21_H__

#define NUM_EXTRA_UART_PARAMS 7

#include "sam.h"
#include "serial_uart/uart_hal.h"

#include "serial_common/hardware/common_samd21.h"

/*!
 * \brief %UART SAMD21 low level driver global namespace
 *
 * This namespace contains all constants and helper functions for implementing the low level drivers and peripherals on the SAMD21 chip. 
 */
namespace UARTSAMD21
{
	/*!
	 * \brief Extra parameters for hardware specific configuration
	 * 
	 * Extra parameter labels used in extra_uart_params peripheral array for SAMD21-specific configuration 
	 */
	enum ExtraParams : uint8_t {
		CLK_SRC,						//!< ID of clock source used in generic clock
		GEN_CLK,						//!< ID of generic clock used for %UART
		GEN_CLK_DIVISOR_BITS_9_16,		//!< Clock speed divisor (bit 9:16)
		GEN_CLK_DIVISOR_BITS_1_8,		//!< Clock speed divisor (bit 1:8)
		GEN_CLK_RUN_STANDY,				//!< Clock on/off while in standby
		PAD_CONFIG,						//!< Pad configuration for pins
		SAMPLE_ADJUSTMENT				//!< Over-sampling rate and sampling adjustment
	};
	/*!
	 * \brief An enum type for over-sampling rate and sampling adjustment
	 */
	enum SampleAdjustment : uint8_t {
		Over16x_7_8_9,					//!< 16x over-sampling rate with 7-8-9 sample adjustment
		Over16x_9_10_11,				//!< 16x over-sampling rate with 9-10-11 sample adjustment
		Over16x_11_12_13,				//!< 16x over-sampling rate with 11-12-13 sample adjustment
		Over16x_13_14_15,				//!< 16x over-sampling rate with 13-14-15 sample adjustment
		Over8x_3_4_5,					//!< 8x over-sampling rate with 3-4-5 sample adjustment
		Over8x_4_5_6,					//!< 8x over-sampling rate with 4-5-6 sample adjustment
		Over8x_5_6_7,					//!< 8x over-sampling rate with 4-5-6 sample adjustment
		Over8x_6_7_8					//!< 8x over-sampling rate with 6-7-8 sample adjustment
	};
	/*!
	 * \brief An enum type for I/O Pad Configuration of TX and RX pins
	 */
	enum PadConfig : uint8_t {
		Tx0_Rx1,						//!< Transmit pin on pad 0, receive pin on pad 1
		Tx0_Rx2,						//!< Transmit pin on pad 0, receive pin on pad 2
		Tx0_Rx3,						//!< Transmit pin on pad 0, receive pin on pad 3
		Tx2_Rx0,						//!< Transmit pin on pad 2, receive pin on pad 0
		Tx2_Rx1,						//!< Transmit pin on pad 2, receive pin on pad 1
		Tx2_Rx3							//!< Transmit pin on pad 2, receive pin on pad 3
	};
	/*!
	 * \brief Populates a peripheral struct with default values
	 *
	 * Populates peripheral struct with default values at specific SERCOM# on SAMD21.
	 *
	 * \param peripheral pointer to peripheral struct to populate with default values
	 * \param sercom_id SERCOM# to be configured as %UART (default = Sercom0)
	 */
	void GetPeripheralDefaults(UARTHAL::Peripheral * peripheral, SERCOMHAL::SercomID sercom_id = SERCOMSAMD21::SercomID::Sercom0);
	
	void SetClkSrcParam(UARTHAL::Peripheral * peripheral, SERCOMSAMD21::ClockSource clock_source = SERCOMSAMD21::ClockSource::CS_OSC8M);			//!< Setter for clock source peripheral parameter
	void SetGenClkParam(UARTHAL::Peripheral * peripheral, SERCOMSAMD21::GenericClock generic_clock = SERCOMSAMD21::GenericClock::GEN_CLK_1);		//!< Setter for generic clock peripheral parameter
	void SetGenClkDivisorParam(UARTHAL::Peripheral * peripheral, uint16_t divisor = 0);																//!< Setter for generic clock divisor peripheral parameter
	void SetGenClkRunStandbyParam(UARTHAL::Peripheral * peripheral, bool run_standy = false);														//!< Setter for generic clock run in standby peripheral parameter
	void SetPadConfigParam(UARTHAL::Peripheral * peripheral, PadConfig pad_config = PadConfig::Tx2_Rx3);											//!< Setter for pad config peripheral parameter
	void SetSampleAdjustmentParam(UARTHAL::Peripheral * peripheral, SampleAdjustment sample_adjustment = SampleAdjustment::Over16x_7_8_9);			//!< Setter for sample adjustment peripheral parameter
}

#endif //__UART_SAMD21_H__
