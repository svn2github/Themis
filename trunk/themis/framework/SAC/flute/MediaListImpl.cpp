#include "MediaListImpl.h"

using namespace flute;
using namespace std;
using namespace sac;

MediaListImpl::MediaListImpl()
:SACMediaList(),
array(10),
current(0)
{
}

MediaListImpl::~MediaListImpl()
{
}

string* MediaListImpl::item(int index) const
{
	if((index < 0) || (index >=current))
	{
		return 0;
	}
	return array[index];
}

void MediaListImpl::addItem(string medium)
{
	if(medium == string("all"))
	{
		array[0] = new string("all");
		current = 1;
		return;
	}
	
	for(int i=0; i < current; i++)
	{
		if(medium == *(array[i]))
			return;
	}
	
	array.push_back(new string(medium));
	current++;
}

string MediaListImpl::toString() const
{
	switch(current)
	{
		case 0:
			return string("");
		case 1:
			return *(array[0]);
		default:
			{
				bool not_done = true;
				int i=0;
				string buf("");
				do
				{
					buf += *(array[i++]);
					if(i == current)
						not_done = false;
					else
						buf += ", ";
				}while(not_done);
				
				return buf;
			}
	}
}