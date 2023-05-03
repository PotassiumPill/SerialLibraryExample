/* 
 * Name			:	serial_buffer.h
 * Created		:	8/18/2022 12:49:40 PM
 * Author		:	Aaron Reilman
 * Description	:	A generic FIFO-like buffer for high level serial communication.
 */


#ifndef __SERIAL_BUFFER_H__
#define __SERIAL_BUFFER_H__

#include "serial_common/common_hal.h"
#include "serial_buffer/generic_buffer.h"

/*!
 * \brief %Serial Buffer global namespace.
 *
 * This namespace contains all global classes, functions and definitions needed to implement serial buffer.
 */
namespace Serial
{
	/*!
	 * \brief Blank Interrupt Enable function to be used as blank parameter for implementing serial buffer.
	 *
	 * This function emulates an unimplemented/unneeded interrupt enabling function for passing into serial buffer functions when parameters do not need to be enabled or disabled.
	 *
	 * \note This function doesn't do anything but is necessary for passing as a parameter.
	 * \sa SerialBuffer.Put(), SerialBuffer.Get(), SerialBuffer.PutString(), SerialBuffer.GetString(), SerialBuffer.PutIntAsASCII(), SerialBuffer.GetASCIIAsInt(), SerialBuffer.GetIntParam()
	 */
	void NoIntEnable(SERCOMHAL::SercomID peripheral_id = 0u, bool enable = false);
	/*!
	 * \brief Integer to ASCII helper function.
	 *
	 * Function which converts an unsigned integer to an array of ASCII characters.
	 *
	 * \param num positive integer value to convert to ASCII
	 * \param result pointer to output ASCII array (must be 11 long)
	 * \return number of ASCII characters in buffer
	 * \note Use in conjunction with SerialBuffer.TransmitString() to create a simple %TransmitInt() function.
	 * \sa SerialBuffer.TransmitString(), SerialBuffer.ReceiveASCIIAsInt()
	 */
	uint32_t Int2ASCII(uint32_t num, char (*result)[10]);
	/*!
	 * \brief %Serial Buffer object
	 *
	 * This is a %Serial Buffer object which acts as a generic high-level character array with FIFO functionality and basic string reading abilities.
	 */
	class SerialBuffer
	{
		//functions
		public:
		/*!
		 * \brief Constructor
		 *
		 * Instantiates %Serial Buffer object. You must call Reset() before usage if no parameters are passed so character array can be initialized.
		 * 
		 * \param peripheral_id SERCOM peripheral ID used on hardware (default = 0)
		 * \param buf char array to be used for buffer (default = nullptr)
		 * \param buf_size buffer size (default = 0)
		 * \note You must call Reset() before usage if constructed without parameters. You must also called SetSercomID() if you need to set a SERCOM# besides 0.
		 * \sa Reset(), SetSercomID(), ~SerialBuffer()
		 */
		SerialBuffer(SERCOMHAL::SercomID peripheral_id = 0u, char * buf = nullptr, uint32_t buf_size = 0);
		/*!
		 * \brief Destructor
		 *
		 * Called when %Serial Buffer object is destroyed. Immediately clears buffer and halts function.
		 *
		 * \sa SerialBuffer(), Clear()
		 */
		~SerialBuffer(void);
		/*!
		 * \brief Resets SerialBuffer.
		 *
		 * Resets all data members and points to new buffer with new size. Must be called if instantiated without parameters. 
		 *
		 * \param buf char array to be used for buffer
		 * \param buf_size buffer size
		 * \note You must call this before use if constructed without parameters.
		 * \sa SerialBuffer(), Clear(), SetSercomID()
		 */
		void Reset(char * buf, uint32_t buf_size);
		/*!
		 * \brief Clears and resets buffer.
		 *
		 * Clears the char array and resets pointers. This retains the same character array and size.\n 
		 * Useful for quick handling of full buffers and errors in serial communication.
		 *
		 * \sa Reset(), ~SerialBuffer()
		 */ 
		void Clear(void);
		/*!
		 * \brief Puts char into buffer.
		 *
		 * Adds char to end of buffer. Prevents addition to a full buffer.
		 *
		 * \param input char to be added to end of buffer
		 * \param int_func interrupt function pointer to be invoked
		 * \return true unless buffer is full and char was attempted to be added
		 * \sa Get(), PutString(), PutIntAsASCII()
		 */ 
		bool Put(char input, void (* int_func)(uint8_t, bool));
		/*!
		 * \brief Gets char from buffer.
		 *
		 * Removes char from front of buffer and sets output pointer parameter to the received char.
		 *
		 * \param int_func interrupt function pointer to be invoked
		 * \param output pointer to received char (default = nullptr)
		 * \return success of reception
		 * \sa Put(), GetString(), GetASCIIAsInt(), GetIntParam()
		 */
		bool Get(void (* int_func)(uint8_t, bool), char * output = nullptr);
		/*!
		 * \brief Reads entire buffer and checks if it is equal to the input string.
		 *
		 * First removes all entries in buffer.\n 
		 * Checks input string with front of buffer, returning true if the input string has been detected.\n 
		 * Can be used multiple times for different strings.\n 
		 * If a string is detected, all calls will be disabled until a new character is received.\n 
		 * Shift parameter detects substring offset by shift places. Useful for detecting a command with parameter (i.e. "c_25").\n 
		 * Adds chars after found shifted string and moves the receive buffer pointer back to the end of the substring if move_pointer is set to true.\n 
		 *
		 * \param input char array to be detected
		 * \param shift offset of string from front of buffer
		 * \param move_pointer moves rd_index to end of detected string if shift is on
		 * \param int_func interrupt function pointer to be invoked
		 * \return if buffer was equal to string
		 * \note Input array must be null-character terminated.
		 * \sa Get(), GetASCIIAsInt(), GetIntParam()
		 */
		bool GetString(const char *input, uint32_t shift, bool move_pointer, void (* int_func)(uint8_t, bool));
		/*!
		 * \brief Gets ASCII integers from front of buffer. 
		 *
		 * Removes ASCII numeric digits from front of buffer and sets output pointer parameter to unsigned int value of the ASCII digits.
		 *
		 * \param output pointer to received int value
		 * \param int_func interrupt function pointer to be invoked
		 * \return success of reception (returns false if no integer was detected)
		 * \sa Get(), GetIntParam(), GetString()
		 */
		bool GetASCIIAsInt(uint32_t * output, void (* int_func)(uint8_t, bool));
		/*!
		 * \brief Reads entire buffer and checks if it is equal to the input string + numeric parameter.
		 *
		 * Checks input string with front of buffer with offset [1, max_digits], and then returns the ASCII number characters following string as unsigned int.\n 
		 * Example: GetIntParam("c_") will set output pointer value to unsigned int 12u if "c_12" was in the front of the buffer.\n 
		 * Can also be used multiple times for different strings with parameters and in conjunction with GetString() calls.\n 
		 * If a string is detected, all calls will be disabled until a new data is added to buffer.\n 
		 * delimiter parameter will detect an end-character if set to nonzero to allow for more secure message reading.\n 
		 * max_digits parameter will set a bound for the max amount of digits in the parameter. If max_digits > 9, the return value may not be what is expected.
		 * 
		 * \param output pointer to received int value
		 * \param input input string to be detected
		 * \param max_digits maximum number of digits in parameter, must not exceed 9
		 * \param delimiter optional delimiting character after int parameter (default = '\0')
		 * \param int_func interrupt function pointer to be invoked
		 * \return success of reception (will return false if string value not detected, no integer detected, or no delimiter detected if set)
		 * \note Input array must be null-character terminated.
		 * \sa GetString(), GetASCIIAsInt(), Get()
		 */
		bool GetIntParam(uint32_t * output, const char *input, char delimiter, uint8_t max_digits, void (* int_func)(uint8_t, bool));
		
		void SetSercomID(SERCOMHAL::SercomID peripheral_id);	//!< Setter for SERCOM ID used on hardware.
		SERCOMHAL::SercomID GetSercomID(void) const;			//!< Getter for SERCOM ID used on hardware.
		uint32_t GetBufferEmpty(void) const;					//!< Getter for number of empty slots available in buffer
		uint32_t GetBufferAvailable(void) const;				//!< Getter for number of chars available in buffer	
		GenericBuffer::BufferState GetBufferState(void) const;	//!< Getter for buffer state (empty, full, neither)
		
		private:
		//private data members
		SERCOMHAL::SercomID sercom_id;
		GenericBuffer::GENERIC_BUFFER<char> buffer;
		uint32_t index_shift;
		
	}; //SerialBuffer
}

#endif //__SERIAL_BUFFER_H__
