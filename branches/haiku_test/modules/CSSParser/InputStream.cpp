/* InputStream.cpp - encapsulates read access to a file, memory, or a socket
**
** Copyright 2001 pinc Software. All Rights Reserved.
*/


#include <stdio.h>
#include <string.h>

#include "InputStream.h"

#define INITIAL_BUFFER_SIZE	16


InputStream::InputStream()
{
	fBuffer = NULL;
}


InputStream::~InputStream()
{
	delete[] fBuffer;
}


bool InputStream::SkipNewline()
{
	if (fBuffer[fBufferPos] == '\n')
	{
		fBufferPos++;
		return true;
	}
	return false;
}


ssize_t InputStream::ReadLine(char **buffer,int32 *line)
{
	// init buffer, if necessary
	if (!fBuffer)
	{
		if ((fBuffer = new char[fBufferSize = INITIAL_BUFFER_SIZE]) == NULL)
			return RC_NO_MEMORY;

		fBufferPos = fBytesRead = 0;  fLastLine = 1;
	}

	while(true)
	{
		// fill buffer
		if (fBytesRead <= fBufferPos || fBytesRead < fBufferSize)
		{
//printf("fill buffer: pos = %d, bytesRead = %d, bufferSize = %d\n",fBufferPos,fBytesRead,fBufferSize);
			ssize_t size;
			if ((size = Read(fBuffer+fBytesRead,fBufferSize - fBytesRead)) < 0)
				return RC_IO_ERROR;
	
			fBytesRead += size;
		}
		// end of file
		if (fBufferPos > fBytesRead)
			return RC_NO_ERROR;

		// split buffer in lines
		int i;
		for(i = fBufferPos;i < fBytesRead;i++)
		{
			if (fBuffer[i] == '\n')		// newline found
			{
				int oldPos = fBufferPos;
	
				fBufferPos = i;
				
				*line = fLastLine;
				while(SkipNewline())
					fLastLine++;

//printf("found line marker at %d, new pos = %d\n",i,fBufferPos);
				fBuffer[i] = '\0';

				*buffer = fBuffer + oldPos;
				return fBufferPos - 1 - oldPos;
			}
		}
		// end of file reached
		if (fBytesRead < fBufferSize)
		{
			int oldPos = fBufferPos;
	
			fBufferPos = i+1;
//printf("file end reached at %d\n",fBufferPos);
			fBuffer[i] = '\0';

			*buffer = fBuffer + oldPos;
			return fBufferPos - 1 - oldPos;
		}
		
		if (fBufferPos && fBytesRead > fBufferPos)	// move old buffer
		{
//printf("move old buffer, pos = %d, bytesRead = %d, bufferSize = %d\n",fBufferPos,fBytesRead,fBufferSize);
			memmove(fBuffer,fBuffer + fBufferPos,fBytesRead - fBufferPos);
			fBytesRead -= fBufferPos;
		}
		else if (fBufferPos == 0)	// enlarge buffer
		{
//printf("enlarge buffer, pos = %d, bytesRead = %d, bufferSize = %d\n",fBufferPos,fBytesRead,fBufferSize);
			char *newBuffer = new char[fBufferSize + INITIAL_BUFFER_SIZE];
			if (!newBuffer)
				return RC_NO_MEMORY;

			memcpy(newBuffer,fBuffer,fBytesRead);
			delete[] fBuffer;
			fBuffer = newBuffer;
			fBufferSize += INITIAL_BUFFER_SIZE;
		}
		fBufferPos = 0;
	}
printf("what am I doing here?\n");
}


/************************** FileInputStream **************************/
// #pragma mark -


FileInputStream::FileInputStream(const char *file)
{
	if ((fFile = open(file,O_RDONLY)) >= RC_NO_ERROR)
		fIsFileOpen = true;
	else
		fIsFileOpen = false;

	fMIMEType = "text/plain";
}


FileInputStream::~FileInputStream()
{
	if (fIsFileOpen)
		close(fFile);
}


status_t FileInputStream::InitCheck()
{
	if (fFile < RC_NO_ERROR)
		return fFile;

	return RC_OK;
}


ssize_t FileInputStream::Read(char *buffer,ssize_t length)
{
	return read(fFile,buffer,length);
}


off_t FileInputStream::Seek(off_t pos,int mode)
{
	return lseek(fFile,pos,mode);
}


off_t FileInputStream::Position()
{
	return lseek(fFile,0,SEEK_CUR);
}

