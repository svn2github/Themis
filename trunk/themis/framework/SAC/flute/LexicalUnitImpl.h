#ifndef __FLUTE_PARSER_LEXICALUNITIMPL_H__
#define __FLUTE_PARSER_LEXICALUNITIMPL_H__

#include "LexicalUnit.h"
#include <string>
/****************************************************************

This class needs to be finished.  
Constructors need to use flute::Parser

****************************************************************/

namespace flute
{

class LexicalUnitImpl: public sac::LexicalUnit
{
	public:
		virtual ~LexicalUnitImpl();
		
		int getLineNumber() const { return line; }
		int getColumnNumber() const { return column; }
		int getLexicalUnitType() const { return type; }
		
		LexicalUnit* getNextLexicalUnit() { return next; }
		LexicalUnit* getPreviousLexicalUnit() { return prev; }
		
		int getIntegerValue() const { return i; }
		void setIntegerValue(int integer) { i = integer; }
		float getFloatValue() const { return f; }
		void setFloatValue(float f) { this->f = f; }
		

	private:
		LexicalUnitImpl(int line, int column, LexicalUnitImpl* p, int i);
		LexicalUnitImpl(int type, int line, int column, LexicalUnitImpl* p);
		LexicalUnitImpl(int line, int column, LexicalUnitImpl* p, short dimension, std::string sdimension, float f);
		LexicalUnitImpl(int line, int column, LexicalUnitImpl* p, int type, std::string s);
		LexicalUnitImpl(int type, int line, int column, LexicalUnitImpl* p, std::string fname, LexicalUnitImpl* params);
	
		sac::LexicalUnit* prev;
		sac::LexicalUnit* next;
		
		int type;
		int line;
		int column;
		
		int i;
		float f;
		short dimension;
		std::string sdimension;
		std::string s;
		std::string fname;
		LexicalUnitImpl* params;
};

}

#endif // __FLUTE_PARSER_LEXICALUNITIMPL_H__