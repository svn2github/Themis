#ifndef __FLUTE_PARSER_SELECTORLISTIMPL_H__
#define __FLUTE_PARSER_SELECTORLISTIMPL_H__

#include "SelectorList.h"
#include <vector>

namespace sac
{
	class Selector;
}

namespace flute
{

class SelectorListImpl: public sac::SelectorList
{
	public:
		SelectorListImpl();
		virtual ~SelectorListImpl();
		
		sac::Selector* item(int index);
		sac::Selector* itemSelector(int index);
		int getLength() const { return current; }
		
		void addSelector(sac::Selector* selector);
		
	private:
		std::vector<sac::Selector*> selectors;
		int current;
		
};

}

#endif // __FLUTE_PARSER_SELECTORLISTIMPL_H__