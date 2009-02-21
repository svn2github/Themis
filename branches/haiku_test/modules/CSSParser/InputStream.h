#ifndef INPUT_STREAM_H
#define INPUT_STREAM_H
/* InputStream.h - encapsulates read access to a file, memory, or a socket
**
** Copyright 2001 pinc Software. All Rights Reserved.
*/


#include <fcntl.h>
#include <unistd.h>

#include "Types.h"


typedef bool (* CheckNameFunc_t)(uint8 mode,uint16 character);
class Encoding;


class InputStream
{
	public:
		InputStream();
		virtual ~InputStream();

		virtual status_t InitCheck() = 0;
#if 0
		virtual void SetMIMEType(char *mime);
#endif
		virtual char *MIMEType() { return "text/plain"; };
	
		virtual ssize_t Read(char *buffer,ssize_t length) = 0;
		virtual ssize_t ReadLine(char **buffer,int32 *line);

		virtual off_t Seek(off_t pos,int mode) = 0;
		virtual off_t Position() = 0;

//		virtual off_t FileSize() = 0;

		// Encoding
#if 0
		virtual void SetStreamEncoding(Encoding *encoding);
		virtual Encoding *StreamEncoding() { return fEncoding; }

		virtual int16 LookupLatin1();
		virtual int16 LookupUnicode16();
		virtual char *GetUnicode16Name(CheckNameFunc_t);
#endif
	private:
		bool SkipNewline();

		char	*fBuffer;
		int		fBufferSize,fBytesRead,fBufferPos,fLastLine;
		Encoding *fEncoding;
};


class FileInputStream : public InputStream
{
	public:
		FileInputStream(const char *name);
		virtual ~FileInputStream();

		virtual status_t InitCheck();
		virtual inline char *MIMEType() { return fMIMEType; };
	
		virtual ssize_t Read(char *buffer,ssize_t length);	

		virtual off_t Seek(off_t pos,int mode);
		virtual off_t Position();

//		virtual off_t FileSize() = 0;
		
	private:
		int		fFile;
		bool	fIsFileOpen;
		char	*fMIMEType;
};


#endif	/* INPUT_STREAM_H */
