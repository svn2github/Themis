#ifndef __SELECTORLIST_H__
#define __SELECTORLIST_H__

namespace sac
{

class Selector;

class SelectorList
{
	public:
		virtual int getLength() const = 0;
		virtual Selector* item(int index) = 0;
};

}

#endif // __SELECTORLIST_H__