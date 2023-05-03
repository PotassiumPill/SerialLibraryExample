/*
 * Name			:	generic_buffer.h
 * Created		:	3/12/2023 3:35:16 PM
 * Author		:	Aaron Reilman
 * Description	:	A generic ring buffer with basic queue functionality.	
 */ 


#ifndef GENERIC_BUFFER_H_
#define GENERIC_BUFFER_H_

#include <stdint.h>

/*!
 * \brief Generic Buffer global namespace.
 *
 * This namespace contains the BufferState enum class and GENERIC_BUFFER template for use in serial communication protocols and data which requires FIFO functionality.
 */
namespace GenericBuffer {
	
	/*!
	 * \brief An enum class for %Serial Buffer State
	 */
	enum class BufferState
	{
		Empty,			//!< Buffer is empty
		Full,			//!< Buffer is full
		NotEmptyNotFull	//!< Buffer has data but isn't full
	};
	/*!
	 * \brief Generic ring buffer template.
	 *
	 * Template class for a generic FIFO ring buffer. Offers very basic put and get functionality, with the ability to access raw elements to directly access and alter elements.
	 */
	template <typename T> class GENERIC_BUFFER
	{
		//functions
		public:
		/*!
		 * \brief Constructor
		 *
		 * Instantiates Generic Buffer object. You must call ResetBuffer() before usage if no parameters are passed so character array can be initialized.
		 *
		 * \param arr array to use as ring buffer (default = nullptr)
		 * \param arr_size size of the array or number of elements in array to use as ring buffer
		 * \sa ResetBuffer(), ~GENERIC_BUFFER()
		 */
		GENERIC_BUFFER(T* arr = nullptr, uint32_t arr_size = 0)
		{
			ResetBuffer(arr, arr_size);
		}
		/*!
		 * \Destructor
		 *
		 * Destroys Generic Buffer object instance.
		 */
		~GENERIC_BUFFER(void)
		{
			Clear();
		}
		/*!
		 * \brief Resets the buffer with new array and size.
		 *
		 * This function allows the ring buffer to use a new array and/or a new size. It is also called before use when no arguments are passed into constructor.
		 *
		 * \param arr new array to use as ring buffer
		 * \param arr_size size of the array or number of elements in array to use as ring buffer
		 * \note You must call this before use if constructed without parameters.
		 * \sa GENERIC_BUFFER(), Clear()
		 */
		void ResetBuffer(T* arr, uint32_t arr_size)
		{
			fifo_buffer = arr;
			buffer_size = arr_size;
			buffer_avail = 0;
			rd_index = 0;
			wr_index = 0;
		}
		/*!
		 * \brief Clears ring buffer
		 *
		 * Essentially resets the buffer while preserving the current array and size.
		 * \sa ResetBuffer()
		 */
		void Clear(void)
		{
			ResetBuffer(fifo_buffer, buffer_size);
		}
		/*!
		 * \brief Retrieves raw elements of ring buffer for direct manipulation of array elements.
		 *
		 * Retrieves the read index, write index, and returns the array pointer that comprises the ring buffer.\n 
		 * This is useful for direct manipulation of array elements for more complex functionality.
		 * 
		 * \param read_index pointer to current read index (default = nullptr)
		 * \param write_index pointer to current write index (default = nullptr)
		 * \return pointer to the beginning of the array used for the ring buffer
		 * \sa ShiftReadPointer()
		 */
		T* GetRawElements(uint32_t * read_index = nullptr, uint32_t * write_index = nullptr)
		{
			if(read_index != nullptr) *read_index = rd_index;
			if(write_index != nullptr) *write_index = wr_index;
			return &(fifo_buffer[0]);
		}
		/*!
		 * \brief Shifts the read pointer forwards or backwards
		 *
		 * Allows for direct manipulation of where the read pointer is, handling all calculations for capping shift amount and updating the current length of the buffer.
		 *
		 * \param shift_size size of the shift based on number of elements (ie shift_size = 1 shifts the read index 1 element forwards/backwards)
		 * \param increase true = shifts forwards, false = shifts backwards
		 * \note Function will cap shift amount if it increases by more than the current length or decreases by more than the size of the array.
		 * \sa GetRawElements()
		 */
		void ShiftReadPointer(uint32_t shift_size, bool increase)
		{
			volatile uint32_t true_shift = shift_size;
			if(increase && (shift_size > buffer_avail))
				true_shift = buffer_avail;
			else if(!increase && (shift_size > buffer_size))
				true_shift = buffer_size;
			if(increase)
			{
				rd_index = (rd_index + true_shift) % buffer_size;
				buffer_avail -= true_shift;
			} else {
				rd_index = rd_index + ((true_shift > rd_index) * buffer_size) - true_shift;	
				buffer_avail += true_shift;
			}
		}
		/*!
		 * \brief Adds an element to buffer
		 *
		 * Adds an element to the end of the buffer in FIFO queue style. Updates length/number of available elements and write index.
		 *
		 * \param element element to add to end of buffer
		 * \return success of adding element (returns false if buffer is full)
		 * \sa Get()
		 */
		bool Put(T element)
		{
			bool success = false;
			if(buffer_avail < buffer_size)
			{
				fifo_buffer[wr_index] = element;
				buffer_avail++;
				wr_index = (wr_index+1) % buffer_size;
				success = true;
			}
			return success;
		}
		/*!
		 * \brief Gets element at the front of buffer
		 *
		 * Gets the first element in the buffer in FIFO queue style. Updates length/number of available elements and read index.
		 *
		 * \param output pointer to received element (default = nullptr) 
		 * \return success of retrieval (returns false if buffer is empty)
		 * \sa Put(), Peek()
		 */
		bool Get(T * output = nullptr)
		{
			bool result = false;
			if(buffer_avail)
			{
				result = true;
				if(output != nullptr) *output = fifo_buffer[rd_index];
				rd_index = (rd_index+1) % buffer_size;
				buffer_avail--;
			}
			return result;
		}
		/*!
		 * \brief Looks at first element in buffer.
		 *
		 * Returns the first element in the buffer in FIFO queue style without updating length or read index.
		 *
		 * \return first element in buffer, or 0 if buffer is empty.
		 * \sa Get()
		 */
		T Peek(void)
		{
			volatile T output = 0;
			if(buffer_avail) output = fifo_buffer[rd_index];
			return output;
		}
		uint32_t GetSize(void) const { return buffer_size; }				//!< Getter for size of array.
		uint32_t GetBufferAvailable(void) const { return buffer_avail; }	//!< Getter for length/number of elements available in buffer.
			
		BufferState GetBufferState(void) const								//!< Getter for buffer state (Empty, Full, NotEmptyNotFull)
		{
			if(!buffer_avail)
				return BufferState::Empty;
			if(buffer_avail == buffer_size)
				return BufferState::Full;
			return BufferState::NotEmptyNotFull;
		}
		private:
		//private members
		T * fifo_buffer;
		uint32_t buffer_size;
		uint32_t buffer_avail;
		uint32_t rd_index;
		uint32_t wr_index;
	};
}

#endif /* GENERIC_BUFFER_H_ */