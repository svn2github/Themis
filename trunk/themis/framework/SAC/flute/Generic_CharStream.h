#ifndef __FLUTE_PARSER_GENERIC_CHARSTREAM_H__
#define __FLUTE_PARSER_GENERIC_CHARSTREAM_H__

#include "CharStream.h"
#include <iostream>
#include <string>
#include <vector>

namespace flute
{

class Generic_CharStream: public CharStream
{
	public:
		Generic_CharStream(std::istream& dstream, int startline, int startcolumn, int buffersize);
		Generic_CharStream(std::istream& dstream, int startline, int startcolumn);
		virtual ~Generic_CharStream();
		
		void ReInit(std::istream& dstream, int startline, int startcolumn, int buffersize);
		void ReInit(std::istream& dstream, int startline, int startcolumn);
		
		void ExpandBuff(bool wrapAround); // might not need
		void FillBuff() throw(IOException);
		void UpdateLineColumn(char c);
		void adjustBeginLineColumn(int newLine, int newCol);
		
		
		virtual char BeginToken() throw(IOException);
		virtual char readChar() throw(IOException);
		virtual int getColumn() const;
		virtual int getLine() const;
		virtual int getEndColumn() const;
		virtual int getEndLine() const;
		virtual int getBeginColumn() const;
		virtual int getBeginLine() const;
		virtual void backup(int amount);
		virtual std::string GetImage();
		virtual std::vector<char> GetSuffix(int len);
		virtual void Done();

	public:
		static const bool staticFlag;
		int bufpos;
		std::vector<int> bufline;
		std::vector<int> bufcolumn;
		
	private:
		int bufsize;
		int available;
		int tokenBegin;
		int column;
		int line;
		bool prevCharIsCR;
		bool prevCharIsLF;
		std::istream* reader;
		std::vector<char> buffer;
		int maxNextCharInd;
		int inBuf;
		
		static std::string createString(std::vector<char> buff, int start, int length);
};

}

#endif // __FLUTE_PARSER_GENERIC_CHARSTREAM_H__