#ifndef __FLUTE_PARSER_MEDIALISTIMPL_H__
#define __FLUTE_PARSER_MEDIALISTIMPL_H__

#include "SACMediaList.h"
#include <string>
#include <vector>

namespace flute
{

class MediaListImpl: public sac::SACMediaList
{
	private:
		std::vector<std::string*> array;
		int current;
	
	public:
		MediaListImpl();
		virtual ~MediaListImpl();
		
		virtual int getLength() const { return current; }		
		virtual std::string* item(int index) const;
		
		void MediaListImpl::addItem(std::string medium);
		std::string toString() const;
};

}

#endif // __FLUTE_PARSER_MEDIALISTIMPL_H__