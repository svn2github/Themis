#include "SelectorListImpl.h"
#include "Selector.h"

using namespace sac;
using namespace std;
using namespace flute;

SelectorListImpl::SelectorListImpl()
:SelectorList(),
selectors(5),
current(0)
{
}

SelectorListImpl::~SelectorListImpl()
{
}

Selector* SelectorListImpl::item(int index)
{
	if ((index < 0) || (index >= current)) 
	{
	    return 0;
	}
	
	return selectors[index];
}

Selector* SelectorListImpl::itemSelector(int index)
{
	if ((index < 0) || (index >= current)) 
	{
	    return 0;
	}
	
	return selectors[index];
}

void SelectorListImpl::addSelector(Selector* selector)
{
	selectors.push_back(selector);
	current++;
}