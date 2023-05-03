/* 
 * Name			:	lora_controller.cpp
 * Created		:	10/18/2022 4:10:48 PM
 * Author		:	Aaron Reilman
 * Description	:	A Lora Communication driver.
 */


#ifndef __LORA_CONTROLLER_H__
#define __LORA_CONTROLLER_H__

#include "serial_communication.h"

#define LORA_DEFAULT_RX_BUFFER_SIZE		256

/*!
 * \brief LoRa global namespace.
 * 
 * This namespace contains all global classes, functions and definitions needed to implement controller for Semtech LLCC-68 LoRa Radio communication.
 */
namespace LoRa
{
	/*!
	 * \brief An enum class for maximum LoRa output power.
	 */
	enum class OutputPower {
		dBm22,					//!< 22 dBm (decibel-milliwatts)
		dBm20,					//!< 20 dBm (decibel-milliwatts)
		dBm17,					//!< 17 dBm (decibel-milliwatts)
		dBm14					//!< 14 dBm (decibel-milliwatts)
	};
	/*!
	 * \brief An enum class for transmit power ramping time.
	 */
	enum class TxRampTime {
		SET_RAMP_10U,			//!< 10 microseconds
		SET_RAMP_20U,			//!< 20 microseconds
		SET_RAMP_40U,			//!< 40 microseconds
		SET_RAMP_80U,			//!< 80 microseconds
		SET_RAMP_200U,			//!< 200 microseconds
		SET_RAMP_800U,			//!< 800 microseconds
		SET_RAMP_1700U,			//!< 1700 microseconds
		SET_RAMP_3400U			//!< 3400 microseconds
	};
	/*!
	 * \brief An enum class for output spreading factor in LoRa modulation.  Higher spreading factor results in longer range.
	 */
	enum class SpreadFactor {
		SF5,					//!< 5 symbols per bit
		SF6,					//!< 6 symbols per bit
		SF7,					//!< 7 symbols per bit
		SF8,					//!< 8 symbols per bit
		SF9,					//!< 9 symbols per bit
		SF10,					//!< 10 symbols per bit
		SF11					//!< 11 symbols per bit
	};
	/*!
	 * \brief An enum class for the bandwidth over which information is spread in LoRa modulation. Lower bandwidth results in longer range but lower throughput.
	 */
	enum class Bandwidth {
		LORA_BW_125,			//!< 125 kHz
		LORA_BW_250,			//!< 250 kHz
		LORA_BW_500				//!< 500 kHz
	};
	/*!
	 * \brief An enum class for the FEC coding rate which tunes the robustness of the forward error correction.
	 */
	enum class CodingRate {
		LORA_CR_4_5,			//!< 4 raw bits : 5 total bits
		LORA_CR_4_6,			//!< 4 raw bits : 6 total bits
		LORA_CR_4_7,			//!< 4 raw bits : 7 total bits
		LORA_CR_4_8				//!< 4 raw bits : 8 total bits
	};
	/*!
	 * \brief Config struct which contains all necessary peripherals and configurables to pass to the LoRa controller.
	 */
	struct Config {
		SPIHAL::Peripheral spi_sercom;			//!< %SPI communication peripheral information (refer to SerialSPI)
		SERCOMHAL::Pinout busy_pin;				//!< I/O pin which is connected to LoRa busy pin
		SERCOMHAL::Pinout irq_pin;				//!< I/O pin which is connected to the LoRa IRQ pin
		SERCOMHAL::Pinout reset_pin;			//!< I/O pin which is connected to the LoRa reset pin
		uint32_t frequency;						//!< Output frequency of the LoRa signal in Hz
		OutputPower output_power;				//!< Maximum LoRa output power
		TxRampTime ramp_time;					//!< Transmit power ramping time
		SpreadFactor spread_factor;				//!< Output spreading factor in LoRa modulation (higher = longer range)
		Bandwidth bandwidth;					//!< Bandwidth over which information is spread in LoRa modulation (lower = longer range)
		CodingRate coding_rate;					//!< FEC coding rate
		bool low_data_rate_opt;					//!< Toggle low data rate optimization (recommended when symbol time is equal or above 16.38 ms)
		uint16_t preamble_symbols;				//!< Number of preamble symbols to be sent in data packet (0x55)
		bool crc_on;							//!< Toggles CRC header to be sent in data packet
		bool public_network;					//!< Toggles public network sync word
	};
	
	//default consts based on hardware implementation
	extern SERCOMHAL::Pinout DEFAULT_LORA_BUSY_PIN;
	extern SERCOMHAL::Pinout DEFAULT_LORA_IRQ_PIN;
	extern SERCOMHAL::Pinout DEFAULT_LORA_RESET_PIN;
	
	/*!
	 * \brief Populates a config struct with default values
	 *
	 * Populates config struct with default values for LoRa communication.
	 *
	 * \param config_peripheral pointer to config struct to populate with default values
	 */
	void GetConfigDefaults(Config * config_peripheral);
	
	/*!
	 * \brief LoRa radio controller object
	 *
	 * This is a LoRa radio controller which offers protocol for initializing a LoRa radio, sending opcodes to LoRa, and creating simple transmit and receive procedures.
	 * This controller also includes a receive buffer with some high level parsing and checking functions to easily implement custom communication protocols.
	 */
	class LoRaController
	{
		//functions
		public:
		/*!
		 * \brief Constructor
		 *
		 * Instantiates controller object for LoRa communication.
		 * This will not initialize the hardware, Init() must be called before communication can work.
		 *
		 * \sa ~LoRaController(), Init(), Teardown()
		 */
		LoRaController(void);
		/*!
		 * \brief Destructor
		 *
		 * Called when LoRaController object is destroyed. Immediately halts data transmission and resets peripheral hardware.
		 *
		 * \sa LoRaController(), Init(), Teardown()
		 */
		~LoRaController(void);
		/*!
		 * \brief Initializes LoRa communication.
		 *
		 * Uses three parameters for hardware config and buffer size to initialize peripheral hardware, SPI module, and data buffer.
		 * 
		 * \param config_peripheral pointer to config struct containing all hardware configurations for %SPI module and LoRa hardware
		 * \param rx_buf initialized character array used for receive buffer
		 * \param rx_buf_size rx buffer size
		 * \sa Teardown(), ResetRXBuffer()
		 */
		void Init(Config * config_peripheral, char * rx_buf, uint32_t rx_buf_size);
		/*! 
		 * \brief Disables and resets %SPI module and LoRa hardware.
		 *
		 * Hard resets LoRa hardware, puts it into standby mode, and disables %SPI.
		 * \sa Init(), ~LoRaController(), ClearRXBuffer()
		 */
		void Teardown(void);
		/*!
		 * \brief Interrupt Service Routine for %SPI module.
		 *
		 * Function to be called in corresponding SERCOM# interrupt handler. Handles %SPI module interrupts.
		 */ 
		void ISR(void);
		/*!
		 * \brief Function for sending opcodes to LoRa.
		 *
		 * Sends an opcode to LoRa through %SPI, offering ability to send read commands with compatability with ReadSPI(), and send parameters.
		 *
		 * \param num_read number of characters to read
		 * \param command opcode command name
		 * \params num_params number of parameters in command (default = 0)
		 * \params params character array containing parameter values (default = nullptr)
		 * \return status byte received from LoRa
		 * \sa ReadSPI()
		 */
		uint8_t SendOpCode(uint32_t num_read, uint8_t command, uint32_t num_params = 0, uint8_t * params = nullptr);
		/*!
		 * \brief Reads from %SPI MISO.
		 *
		 * Use with SendOpCode() read commands to read %SPI MISO buffer. SendOpCode() handles RFU but most other read commands will send a status byte after the first parameter or NOP.
		 *
		 * \param output pointer to received char (default = nullptr)
		 * \return success of reception
		 * \sa SendOpCode()
		 */
		bool ReadSPI(char * output = nullptr);
		/*!
		 * \brief Writes value to given address in LoRa hardware.
		 *
		 * Sends opcode with necessary parameters to write to a LoRa register.
		 * \param address register address
		 * \param value value to write to address
		 * \sa ReadRegister()
		 */
		void WriteRegister(uint16_t address, uint8_t value);
		/*!
		 * \brief Reads value at specific register in LoRa hardware.
		 *
		 * Sends opcode with necessary parameters to read value from a LoRa register and outputs value.
		 *
		 * \param address register address
		 * \return value at address
		 * \sa WriteRegister()
		 */
		uint8_t ReadRegister(uint16_t address);
		/*!
		 * \brief Returns the IRQ status bytes of LoRa radio as unsigned short.
		 *
		 * Reads the IRQ status of the LoRa radio and clears all interrupts. Refer to datasheet for the bit specifications of IRQ status bytes.
		 *
		 * \return status bytes of IRQ status as unsigned short (9:0)
		 */
		uint16_t GetIRQStatus(void);
		/*!
		 * \brief Set LoRa into transmit mode using given payload.
		 *
		 * This function allows user to implement custom transmission after calling WriteBuffer() or directly sending %WriteBuffer opcode.
		 *
		 * \param payload number of bytes to send
		 * \param timeout length of timeout (refer to datasheet for time calculations), set to 0 for no timeout (default = 0)
		 * \return success of transmission (false return means timeout occurred, check your config settings to make sure they are valid)
		 * \sa WriteBuffer(), Transmit(), TransmitString(), TransmitInt(), GetIRQStatus()
		 */
		bool SetTxMode(uint8_t payload, uint32_t timeout = 0);
		/*!
		 * \brief Writes bytes to LoRa data buffer for transmission.
		 *
		 * This function allows user to implement custom transmission before calling SetTxMode() or configuring it manually using %SetTx opcode.
		 *
		 * \param payload number of bytes to be sent
		 * \param payload_bytes character array of bytes to be sent
		 * \sa SetTxMode()
		 */
		void WriteBuffer(uint8_t payload, uint8_t * payload_bytes);
		/*!
		 * \brief Reads received bytes from LoRa data buffer.
		 *
		 * This function reads the bytes received by the LoRa radio, used in custom implementation of reception by manually calling %SetRx opcode and reading payload information.
		 *
		 * \param payload number of bytes to read/were received
		 */
		void ReadBuffer(uint8_t payload);
		/*!
		 * \brief Reset procedure
		 *
		 * Briefly sets reset pin to high, then low in order to reset LoRa.
		 */
		void ResetLoRa(void);
		/*!
		 * \brief Transmits a single byte through LoRa radio
		 *
		 * Handles all steps for transmitting a single byte through LoRa, including setting payload size in the packet parameters, writing to data buffer, and setting LoRa into transmit mode.
		 * Also returns success of transmission by reading IRQ status, if timeout was enabled and set.
		 * 
		 * \param input byte to be sent via LoRa
		 * \param timeout timeout value, refer to datasheet for specifications (default = 0/no timeout)
		 * \return success of transmission (false = transmission timed out, refer to datasheet to verify valid settings)
		 * \sa TransmitString(), TransmitInt()
		 */
		bool Transmit(char input, uint32_t timeout = 0);
		/*!
		 * \brief Transmits a single byte through LoRa radio
		 *
		 * Handles all steps for transmitting a single byte through LoRa, including setting payload size in the packet parameters, writing to data buffer, and setting LoRa into transmit mode.
		 * Also returns success of transmission by reading IRQ status, if timeout was enabled and set.
		 * 
		 * \param input byte to be sent via LoRa
		 * \param timeout timeout value, refer to datasheet for specifications (default = 0/no timeout)
		 * \return success of transmission (false = transmission timed out, refer to datasheet to verify valid settings)
		 * \sa TransmitString(), TransmitInt()
		 */
		bool TransmitPacket(const char * input, uint8_t num_bytes, uint32_t timeout = 0);
		/*!
		 * \brief Transmits a string through LoRa radio
		 *
		 * Handles all steps for transmitting multiple bytes through LoRa, including setting payload size in the packet parameters, writing to data buffer, and setting LoRa into transmit mode.
		 * Array must be null-character terminated.
		 *
		 * \param input character array to be sent through LoRa (must be null-character terminated)
		 * \param timeout timeout value, refer to datasheet for specifications (default 0/no timeout)
		 * \return success of transmission (false = transmission timed out, refer to datasheet to verify valid settings)
		 * \sa Transmit(), TransmitInt()
		 */
		bool TransmitString(const char *input, uint32_t timeout = 0);
		/*!
		 * \brief Transmits an int value as a string of ASCII character through LoRa radio
		 * Handles all steps for transmitting multiple numeric ASCII bytes through LoRa, including converting int to ASCII array, setting payload size in the packet parameters, writing to data buffer, and setting LoRa into transmit mode.
		 * 
		 * \param input unsigned int value to be sent as ASCII bytes through LoRa
		 * \param timeout timeout value, refer to datasheet for specifications (default 0/no timeout)
		 * \return success of transmission (false = transmission timed out, refer to datasheet to verify valid settings)
		 * \sa Transmit(), TransmitString()
		 */
		bool TransmitInt(uint32_t input, uint32_t timeout = 0);
		/*!
		 * \brief Sets LoRa radio into receive mode to receive packet.
		 *
		 * Handles all steps for receiving a single packet through LoRa radio, including setting radio into receive mode, reading data buffer into external buffer, and outputting any errors and timeouts.
		 *
		 * \param timeout timeout value, refer to datasheet for specifications (default 0/no timeout)
		 * \param timeout_status pointer to timeout status boolean, which is set to true if receive failed due to timeout (default = nullptr)
		 * \return success of reception (false = reception timed out or CRC error checking failed)
		 * \sa ReadRxChar(), ReadRxString(), ReadRxASCIIInt(), ReadRxParam()
		 */
		bool ReceiveSingle(uint32_t timeout = 0, bool * timeout_status = nullptr);
		/*!
		 * \brief Resets receive buffer which holds all data received from LoRa radio.
		 *
		 * Resets received data buffer to use a new input array and buffer size.
		 * 
		 * \param rx_buf initialized character array used for received data buffer
		 * \param rx_size received data buffer size
		 * \sa ClearRxBuffer()
		 */ 
		void ResetRxBuffer(char * rx_buf, uint32_t rx_buf_size);
		/*!
		 * \brief Clears received data buffer
		 *
		 * Clears out received data buffer and resets all indices and indicators.
		 *
		 * \sa Teardown()
		 */
		void ClearRxBuffer(void);
		/*!
		 * \brief Reads single byte from received data buffer
		 *
		 * Reads a single byte from received data buffer and iterates pointer.
		 *
		 * \param output pointer to received char (default = nullptr)
		 * \return success of read
		 * \sa ReceiveSingle(), ReadRxString(), ReadRxASCIIInt(), ReadRxParam()
		 */
		bool ReadRxChar(char * output = nullptr);
		/*!
		 * \brief Checks if input string has been read from received data buffer
		 *
		 * Checks input string with front of received data buffer, returning true if the input string has been detected.\n
		 * Can be used multiple times for different strings.\n
		 * Iterates rx read index.\n
		 * If a string is detected, all calls will be disabled until a new character is received.\n
		 * Shift parameter detects substring offset by shift places. Useful for detecting a command with parameter (i.e. "c_25").\n
		 * Moves the receive buffer pointer back to the end of the substring if move_pointer is set to true.
		 *
		 * \param input char array to be detected
		 * \param shift offset of string from end of receive buffer (default = 0)
		 * \param move_pointer moves rx_rd_index to end of detected string if shift is on (default = true)
		 * \return if string has been read
		 * \note Input array must be null-character terminated.
		 * \sa ReceiveSingle(), ReadRxChar(), ReadRxASCIIInt(), ReadRxParam()
		 */
		bool ReadRxString(const char * input, uint32_t shift = 0u, bool move_pointer = true);
		/*!
		 * \brief Reads SCII number and outputs unsigned int.
		 *
		 * Reads received data buffer until empty or non-numeric ASCII character received, and converts to unsigned int.
		 *
		 * \param output pointer to received int value
		 * \return success of read (returns false if no integer was detected)
		 * \sa ReceiveSingle(), ReadRxChar(), ReadRxString(), ReadRxParam()
		 */
		bool ReadRxASCIIInt(uint32_t * output);
		/*!
		 * \brief Checks if input string followed by an ASCII parameter has been read from received data buffer and returns the ASCII parameter as unsigned int.
		 *
		 * Checks input string with front of received data buffer with offset [1, max_digits], and then returns the following ASCII number characters as unsigned int.\n
		 * Call HasReceivedParam() immediately after call to ensure string has been received.\n
		 * Example: ReceiveParam("c_") will output unsigned int 12u if "c_12" is sent via %SPI.\n
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
		 * \return success of read (will return false if string value not detected, no integer detected, or no delimiter detected if set)
		 * \note Input array must be null-character terminated.
		 * \sa ReceiveSingle(), ReadRxChar(), ReadRxString(), ReadRxASCIIInt()
		 */
		bool ReadRxParam(uint32_t * output, const char *input, char delimiter = '\0', uint8_t max_digits = 8u);
		
		uint32_t GetRxAvailable(void) const;			//!< Getter for number of unread characters available in received data buffer
		
		private:
		//private data members
		char miso_buffer[DEFAULT_MISO_BUFFER_SIZE];
		char mosi_buffer[DEFAULT_MOSI_BUFFER_SIZE];
		SERCOMHAL::Pinout reset_pin;
		SERCOMHAL::Pinout busy_pin;
		SERCOMHAL::Pinout irq_pin;
		uint16_t preamble_symbols;
		bool crc_on;
		SerialSPI::SPIController spi_control;
		Serial::SerialBuffer received_data;
	}; //LoRaController
}

#endif //__LORA_CONTROLLER_H__
