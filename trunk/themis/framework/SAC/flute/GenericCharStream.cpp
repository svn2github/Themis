#include "Generic_CharStream.h"

using namespace flute;
using namespace std;

const bool Generic_CharStream::staticFlag = false;

Generic_CharStream::Generic_CharStream(istream& dstream, int startline,
	int startcolumn, int buffersize)
:CharStream(),
bufline(buffersize),
bufcolumn(buffersize),
bufsize(buffersize),
available(buffersize),
column(startcolumn-1),
line(startline),
reader(&dstream),
buffer(buffersize)
{
}

Generic_CharStream::Generic_CharStream(istream& dstream, int startline,
	int startcolumn)
:CharStream(),
bufline(4096),
bufcolumn(4096),
bufsize(4096),
available(4096),
column(startcolumn-1),
line(startline),
reader(&dstream),
buffer(4096)
{
}

Generic_CharStream::~Generic_CharStream()
{
}

void Generic_CharStream::ReInit(istream& dstream, int startline,
	int startcolumn, int buffersize)
{
	reader = &dstream;
	line = startline;
	column = startcolumn - 1 ;
	
	if((int)buffer.capacity() != buffersize)
	{
		available = bufsize = buffersize;
		buffer.resize(buffersize);
	}
}

void Generic_CharStream::ReInit(istream& dstream, int startline,
	int startcolumn)
{
	ReInit(dstream, startline, startcolumn, 4096);
}

void Generic_CharStream::ExpandBuff(bool wrapAround)
{
	if(wrapAround)
	{	
	// MAY HAVE A PROBLEM, need to figure out what the wrapAround does
		buffer.resize(bufsize + 2048);
		bufline.resize(bufsize + 2048);
		bufcolumn.resize(bufsize + 2048);
		maxNextCharInd = (bufpos += (bufsize - tokenBegin));
	}
	else
	{
		buffer.resize(bufsize + 2048);
		bufline.resize(bufsize + 2048);
		bufcolumn.resize(bufsize + 2048);
		maxNextCharInd = (bufpos -= tokenBegin);
	}
	
	bufsize += 2048;
	available = bufsize;
	tokenBegin = 0;
}

void Generic_CharStream::FillBuff() throw(IOException)
{
	if(maxNextCharInd == available)
	{
		if(available == bufsize)
		{
			if(tokenBegin > 2048)
			{
				bufpos = maxNextCharInd = 0;
				available = tokenBegin;
			}
			else if(tokenBegin < 0)
				bufpos = maxNextCharInd = 0;
			else
				ExpandBuff(false);
		}
		else if(available > tokenBegin)
			available = bufsize;
		else if((tokenBegin - available) < 2048)
			ExpandBuff(true);
		else
			available = tokenBegin;
	}
	
	int i;
	try
	{
		int tempBufferSize = available - maxNextCharInd;
		char* tempBuffer = new char[tempBufferSize];
		reader->read(tempBuffer, tempBufferSize);
		i = reader->gcount();
		// copy the temp buffer
		for(int j=0; j<i; j++)
			buffer[maxNextCharInd + j] = tempBufferSize;
		if(i == -1)
		{
			throw IOException();
		}
		else
		{
			maxNextCharInd += i;
		}
	}catch(IOException& e){
		--bufpos;
		backup(0);
		if(tokenBegin == -1)
			tokenBegin = bufpos;
		throw;
	}
}

void Generic_CharStream::UpdateLineColumn(char c)
{
	column++;
	
	if(prevCharIsLF)
	{
		prevCharIsLF = false;
		line += (column = 1);
	}
	else if(prevCharIsCR)
	{
		prevCharIsCR = false;
		if(c == '\n')
		{
			prevCharIsLF = true;
		}
		else
			line += (column = 1);
	}
	
	switch(c)
	{
        case '\r' :
           prevCharIsCR = true;
           break;
        case '\n' :
           prevCharIsLF = true;
           break;
        case '\t' :
           column--;
           column += (8 - (column & 07));
           break;
        default :
           break;
	}
	
	bufline[bufpos] = line;
	bufcolumn[bufpos] = column;
}

void Generic_CharStream::adjustBeginLineColumn(int newLine, int newCol)
{
	 int start = tokenBegin;
	 int len;
	
	 if (bufpos >= tokenBegin)
	 {
	    len = bufpos - tokenBegin + inBuf + 1;
	 }
	 else
	 {
	    len = bufsize - tokenBegin + bufpos + 1 + inBuf;
	 }
	
	 int i = 0, j = 0, k = 0;
	 int nextColDiff = 0, columnDiff = 0;
	
	 while (i < len &&
	        bufline[j = start % bufsize] == bufline[k = ++start % bufsize])
	 {
	    bufline[j] = newLine;
	    nextColDiff = columnDiff + bufcolumn[k] - bufcolumn[j];
	    bufcolumn[j] = newCol + columnDiff;
	    columnDiff = nextColDiff;
	    i++;
	 } 
	
	 if (i < len)
	 {
	    bufline[j] = newLine++;
	    bufcolumn[j] = newCol + columnDiff;
	
	    while (i++ < len)
	    {
	       if (bufline[j = start % bufsize] != bufline[++start % bufsize])
	          bufline[j] = newLine++;
	       else
	          bufline[j] = newLine;
	    }
	 }
	
	 line = bufline[j];
	 column = bufcolumn[j];
}

char Generic_CharStream::BeginToken() throw(IOException)
{
	tokenBegin = -1;
	char c = readChar();
	tokenBegin = bufpos;
	
	return c;
}

char Generic_CharStream::readChar() throw(IOException)
{
     if (inBuf > 0)
     {
        --inBuf;
        return (char)((char)0xff & buffer[(bufpos == bufsize - 1) ? (bufpos = 0) : ++bufpos]);
     }

     if (++bufpos >= maxNextCharInd)
        FillBuff();

     char c = (char)((char)0xff & buffer[bufpos]);

     UpdateLineColumn(c);
     return (c);
}

int Generic_CharStream::getColumn() const
{
	return bufcolumn[bufpos];
}

int Generic_CharStream::getLine() const
{
	return bufline[bufpos];
}

int Generic_CharStream::getEndColumn() const
{
	return bufcolumn[bufpos];
}

int Generic_CharStream::getEndLine() const
{
	return bufline[bufpos];
}

int Generic_CharStream::getBeginColumn() const
{
	return bufcolumn[tokenBegin];
}

int Generic_CharStream::getBeginLine() const
{
	return bufline[tokenBegin];
}

void Generic_CharStream::backup(int amount)
{
	inBuf += amount;
	if((bufpos -= amount) < 0)
		bufpos += bufsize;
}

string Generic_CharStream::GetImage()
{
	if(bufpos >= tokenBegin)
		return createString(buffer, tokenBegin, bufpos - tokenBegin + 1);
	else
		return createString(buffer, tokenBegin, bufsize - tokenBegin) +
			createString(buffer, 0, bufpos + 1);
}

vector<char> Generic_CharStream::GetSuffix(int len)
{
	vector<char> ret(len);
	
	if((bufpos + 1) >= len)
	{
		for(int i=0; i<len; i++)
		{
			ret[i] = buffer[bufpos - len + 1 + i];
		}
	}
	else
	{
		int tempLen = len - bufpos - 1;
		for(int i=0; i<tempLen; i++)
		{
			ret[i] = buffer[(bufsize - (len - bufpos - 1)) + i];
		}
		
		for(int j=0, i=tempLen, tempLen2 = (tempLen + (bufpos +1));
			i<tempLen2;
			i++, j++)
		{
			ret[i] = buffer[j];
		}
	}
	
	return ret;
}

void Generic_CharStream::Done()
{
	buffer.clear();
	bufline.clear();
	bufcolumn.clear();
}

// algorithim needs to be improved, based functionality will work though
string Generic_CharStream::createString(vector<char> buff, int start, int length)
{
	string ret("");
	
	for(int i=0; i<length; i++, start++)
	{
		ret += buff[start];
	}
	
	return ret;
}