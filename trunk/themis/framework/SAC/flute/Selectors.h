#ifndef __FLUTE_PARSER_SELECTORS_H__
#define __FLUTE_PARSER_SELECTORS_H__

#include "SelectorList.h"
#include <vector>

namespace sac
{
	class Selector;
}

namespace flute
{

class Selectors: public sac::SelectorList
{
	public:
		Selectors();
		virtual ~Selectors();
		
		sac::Selector* item(int index);
		sac::Selector* itemSelector(int index);
		int getLength() const { return current; }
		
		void addSelector(sac::Selector* selector);
		
	private:
		std::vector<sac::Selector*> selectors;
		int current;
		
};

}

#endif // __FLUTE_PARSER_SELECTORS_H__