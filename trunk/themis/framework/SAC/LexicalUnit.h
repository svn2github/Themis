#ifndef __LEXICALUNIT_H__
#define __LEXICALUNIT_H__

#include <string>

namespace sac
{

class LexicalUnit
{
	public:
		enum
		{
			SAC_OPERATOR_COMMA,
			SAC_OPERATOR_PLUS,
			SAC_OPERATOR_MINUS,
			SAC_OPERATOR_MULTIPLY,
			SAC_OPERATOR_SLASH,
			SAC_OPERATOR_MOD,
			SAC_OPERATOR_EXP,
			SAC_OPERATOR_LT,
			SAC_OPERATOR_GT,
			SAC_OPERATOR_LE,
			SAC_OPERATOR_GE,
			SAC_OPERATOR_TILDE,
			SAC_INHERIT,
			SAC_INTEGER,
			SAC_REAL,
			SAC_EM,
			SAC_EX,
			SAC_PIXEL,
			SAC_INCH,
			SAC_CENTIMETER,
			SAC_MILLIMETER,
			SAC_POINT,
			SAC_PICA,
			SAC_PERCENTAGE,
			SAC_URI,
			SAC_COUNTER_FUNCTION,
			SAC_COUNTERS_FUNCTION,
			SAC_RGBCOLOR,
			SAC_DEGREE,
			SAC_GRADIAN,
			SAC_RADIAN,
			SAC_MILLISECOND,
			SAC_SECOND,
			SAC_HERTZ,
			SAC_KILOHERTZ,
			SAC_IDENT,
			SAC_STRING_VALUE,
			SAC_ATTR,
			SAC_RECT_FUNCTION,
			SAC_UNICODERANGE,
			SAC_SUB_EXPRESSION,
			SAC_FUNCTION,
			SAC_DIMENSION
		};
		
		virtual int getLexicalUnitType() = 0;
		virtual LexicalUnit* getNextLexicalUnit() = 0;
		virtual LexicalUnit* getPerviousLexicalUnit() = 0;
		virtual float getFloatValue() = 0;
		virtual std::string getDimensionUnitText() = 0;
		virtual std::string getFunctionName() = 0;
		virtual LexicalUnit* getParameters() = 0;
		virtual std::string getStringValue() = 0;
		virtual LexicalUnit* getSubValue() = 0;
};

}

#endif // __LEXICALUNIT_H__