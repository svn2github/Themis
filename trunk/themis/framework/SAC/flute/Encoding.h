#ifndef __FLUTE_UTIL_ENCODING_H__
#define __FLUTE_UTIL_ENCODING_H__

#include <string>
#include <map>

namespace flute_util
{

class Encoding
{
	public:
		static std::string getEncoding(std::string encoding);
		~Encoding();
	
	private:
		Encoding();
		
		static Encoding* _encoding;
		static std::map<std::string, std::string> encodings;
};

}

#endif // __FLUTE_UTIL_ENCODING_H__