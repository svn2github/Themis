#include "LexicalUnitImpl.h"

/****************************************************************

This class needs to be finished.  
Constructors need to use flute::Parser

****************************************************************/

using namespace sac;
using namespace flute;
using namespace std;

LexicalUnitImpl::LexicalUnitImpl(int line, int column, LexicalUnitImpl* p, int i)
:LexicalUnit(),
type(SAC_INTEGER),
line(line),
column(column),
i(i)
{
	if(p)
	{
		prev = p;
		p->next = this;
	}
}

LexicalUnitImpl::LexicalUnitImpl(int type, int line, int column, LexicalUnitImpl* p)
:LexicalUnit(),
type(type),
line(line),
column(column)
{
	if(p)
	{
		prev = p;
		p->next = this;
	}
}

LexicalUnitImpl::LexicalUnitImpl(int line, int column, LexicalUnitImpl* p, short dimension, std::string sdimension, float f)
:LexicalUnit(),
line(line),
column(column),
f(f),
dimension(dimension),
sdimension(sdimension)
{
	if(p)
	{
		prev = p;
		p->next = this;
	}
}

LexicalUnitImpl::LexicalUnitImpl(int line, int column, LexicalUnitImpl* p, int type, std::string s)
:LexicalUnit(),
type(type),
line(line),
column(column),
s(s)
{
	if(p)
	{
		prev = p;
		p->next = this;
	}
}

LexicalUnitImpl::LexicalUnitImpl(int type, int line, int column, LexicalUnitImpl* p, std::string fname, LexicalUnitImpl* params)
:LexicalUnit(),
type(type),
line(line),
column(column),
fname(fname),
params(params)
{
	if(p)
	{
		prev = p;
		p->next = this;
	}
}

LexicalUnitImpl::~LexicalUnitImpl()
{
}