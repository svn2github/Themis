#ifndef __PARSERFACTORY_H__
#define __PARSERFACTORY_H__

#include "Parser.h"

namespace sac_helper
{

template<class T>
class ParserFactory
{
	public:
		sac::Parser* makeParser();
};

}

template<class T>
sac_parser::ParserFactory<T>::makeParser()
{
	return new T();
}

#endif // __PARSERFACTORY_H__