/*
Copyright (c) 2003 Z3R0 One. All Rights Reserved.

Permission is hereby granted, free of charge, to any person 
obtaining a copy of this software and associated documentation 
files (the "Software"), to deal in the Software without 
restriction, including without limitation the rights to use, 
copy, modify, merge, publish, distribute, sublicense, and/or 
sell copies of the Software, and to permit persons to whom 
the Software is furnished to do so, subject to the following 
conditions: 

   The above copyright notice and this permission notice 
   shall be included in all copies or substantial portions 
   of the Software. 

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY 
KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE 
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR 
PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS 
OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR 
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR 
OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE 
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Original Author & Project Manager: Z3R0 One (z3r0_one@bbnk.dhs.org)
Project Start Date: October 18, 2000
*/

/*!
\file
\brief The network system's internal buffer class declaration.


*/
#ifndef _network_buffer_
#define _network_buffer_

#include <Locker.h>

namespace _Themis_Networking_ {

/*!
\brief The Buffer class holds a chunk of data for processing.

This class simply holds data that has been received until the higher protocol layer
can retrieve it through the Connection class.
*/
	class Buffer {
		private:
		/*!
		\brief The internal buffer.
		
		
		*/
			unsigned char *buffer;
		/*!
		\brief The size of the internal buffer.
		
		
		*/
			off_t mBufferSize;
		/*!
		\brief The size of the data in the internal buffer.
		
		
		*/
			off_t data_size;
		/*!
		\brief Has the data been read?
		
		
		*/
			bool been_read;
		/*!
		\brief Initializes internal variables to default values.
		
		
		*/
			void Init();
		/*!
		\brief Thread access restrictor.
		
		
		*/
			BLocker *lock;

		public:
		/*!
		\brief The default constructor.
		
		This version of the constructor initializes the internal variables to their default
		values. Data must be added to the object through the SetData() function call
		before the object can be useful.
		*/
			Buffer();
		/*!
		\brief A constructor.
		
		This version of the constructor will copy data passed in into the internal
		buffer. The buffer is 100% usable after construction.
		*/
			Buffer(void *data, off_t size, off_t buffer_size);
		/*!
		\brief The destructor.
		
		
		*/
			~Buffer();
		/*!
		\brief Returns the size of the buffer object.
		
		To be more specific, this function returns the size of the object's internal
		buffer.
		*/
			off_t Size();
		/*!
		\brief Has the buffer been read?
		
		Returns whether or not the buffer object's data has been retrieved.
		\retval	true	The data has been copied out of this object.
		\retval	false	The data has not been copied.
		*/
			bool HasBeenRead();
		/*!
		\brief Change the "read" status of this object.
		
		You may change the read status of a buffer object with this function.
		*/
			bool MarkRead(bool have_read=true);
		/*!
		\brief Is the buffer empty?
		
		Returns whether or not the buffer object is empty.
		\retval	true	The buffer is empty.
		\retval	false	The buffer is not empty.
		*/
			bool IsEmpty();
		/*!
		\brief Copies the data to the passed in buffer.
		
		This function copies the data stored in this object into the passed in buffer.
		*/
			off_t GetData(void *data,off_t max_size=0);
		/*!
		\brief Sets the object's internal buffer to the incoming data value and size.
		
		This function makes a copy of the data that is being passed in.
		*/
			off_t SetData(void *data,off_t size, off_t buffer_size);
		/*!
		\brief Empties the buffer's data.
		
		This function simply empties the buffer's data, and resets all variables to
		their original initialized state.
		*/
			void Empty();
	};
};

#endif
