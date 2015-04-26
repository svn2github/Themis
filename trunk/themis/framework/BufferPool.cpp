/*
	Copyright (c) 2015 Mark Hellegers. All Rights Reserved.
	
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
	
	Original Author: 	Mark Hellegers (mark@firedisk.net)
	Project Start Date: October 18, 2000
	Class Start Date: April 22, 2015
*/

/*	BufferPool implementation
	See BufferPool.hpp for more information
	
*/

#include <stdio.h>

// Themis headers
#include "BufferPool.hpp"
#include "netbuffer.h"

BufferPool :: BufferPool() {

	buffer_in = NULL;
}

void BufferPool :: AppendData(void * aData, off_t aSize, off_t aBufferSize) {

	buffer_list_st *current = buffer_in;
	if (current == NULL) {
		buffer_in = new buffer_list_st;
		current = buffer_in;
		current->next = NULL;
		current->buffer = new Buffer(aData, aSize, aBufferSize);
	} else {
		while (!current->buffer->HasBeenRead() && current->next != NULL)
			current = current->next;
		if (!current->buffer->HasBeenRead()) {
			current->next = new buffer_list_st;
			current = current->next;
			current->next = NULL;
			current->buffer = new Buffer(aData, aSize, aBufferSize);
		}
		else {
			current->buffer->SetData(aData, aSize, aBufferSize);
		}
	}

}

off_t BufferPool :: Receive(void *aData, off_t aMaxSize) {

	off_t size=0L;
	buffer_list_st *current=buffer_in;
	buffer_list_st *prev;
	int32 bytes;
	while (current != NULL) {
		if (current->buffer == NULL || current->buffer->HasBeenRead())
			break;
		if ((size + current->buffer->Size()) > aMaxSize) {
			break;
		}
		bytes = current->buffer->GetData((unsigned char*) aData + size, aMaxSize - size);
		size += bytes;
		prev = current;
		current = current->next;
	}

	return size;
}

off_t BufferPool :: DataSize() {

	off_t size=0L;
	buffer_list_st *current = buffer_in;
	while(current != NULL) {
		if (current->buffer != NULL)
			size += current->buffer->Size();
		current = current->next;
	}

	return size;	
}

void BufferPool :: EmptyBuffer() {

	if (buffer_in != NULL) {
		buffer_list_st *next;
		while (buffer_in != NULL) {
			next = buffer_in->next;
			delete buffer_in->buffer;
			delete buffer_in;
			buffer_in = next;
		}
		
	}

}

bool BufferPool :: IsEmpty() {
	
	return (buffer_in == NULL);
}
