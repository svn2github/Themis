#include "LocatorImpl.h"
#include "Parser.h"
#include "Token.h"

using namespace std;
using namespace sac;
using namespace flute;

LocatorImpl::LocatorImpl(Parser* p)
:Locator()
{
}

LocatorImpl::LocatorImpl(Parser* p, Token* tok)
:Locator(),
line(tok->beginLine),
column(tok->beginColumn)
{
}

LocatorImpl::LocatorImpl(Parser* p, int line, int column)
:Locator(),
line(line),
column(column)
{
}

LocatorImpl* LocatorImpl::reInit(Parser* p)
{
	return this;
}

LocatorImpl* LocatorImpl::reInit(sac::Parser* p, int line, int column)
{
	return this;
}

LocatorImpl::~LocatorImpl()
{
}