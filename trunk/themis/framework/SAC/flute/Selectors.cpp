#include "Selectors.h"
#include "Selector.h"

using namespace sac;
using namespace std;
using namespace flute;

Selectors::Selectors()
:SelectorList(),
selectors(5),
current(0)
{
}

Selectors::~Selectors()
{
}

Selector* Selectors::item(int index)
{
	if ((index < 0) || (index >= current)) 
	{
	    return 0;
	}
	
	return selectors[index];
}

Selector* Selectors::itemSelector(int index)
{
	if ((index < 0) || (index >= current)) 
	{
	    return 0;
	}
	
	return selectors[index];
}

void Selectors::addSelector(Selector* selector)
{
	selectors.push_back(selector);
	current++;
}