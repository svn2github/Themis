#include "CSS_Types.h"

CSSType::CSSType(int value)
{
	CSSType::value = value;
	typer = true;
}

CSSType::CSSType(float value)
{
	CSSType::value = value;
	typer = false;
}

CSSType::~CSSType()
{
}


CSSNumber::CSSNumber(int value) : CSSType(value)
{}

CSSNumber::~CSSNumber()
{}


CSSLength::CSSLength(float value) : CSSType(value)
{}

CSSLength::~CSSLength()
{}