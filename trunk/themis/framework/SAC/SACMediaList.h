#ifndef __SACMEDIALIST_H__
#define __SACMEDIALIST_H__

#include <string>

namespace sac
{

class SACMediaList
{
	public:
		virtual int getLength() const = 0;
		virtual std::string* item(int index) const = 0;
};

}

#endif // __SACMEDIALIST_H__