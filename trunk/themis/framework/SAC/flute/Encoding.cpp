#include "Encoding.h"

using namespace flute_util;
using namespace std;

Encoding* Encoding::_encoding = 0;
map<string, string> Encoding::encodings;

Encoding::Encoding()
{
	// load the map
	encodings["iso-ir-6"] = "ASCII";
	encodings["ANSI_X3.4-1986"] = "ASCII";
	encodings["ISO_646.irv:1991"] = "ASCII";
	encodings["ASCII"] = "ASCII";
	encodings["ISO646-US"] = "ASCII";
	encodings["US-ASCII"] = "ASCII";
	encodings["us"] = "ASCII";
	encodings["IBM367"] = "ASCII";
	encodings["cp367"] = "ASCII";
	encodings["csASCII"] = "ASCII";
	
	encodings["iso-ir-100"] = "ISO8859_1";
	encodings["ISO_8859-1:1987"] = "ISO8859_1";
	encodings["ISO_8859-1"] = "ISO8859_1";
	encodings["ISO-8859-1"] = "ISO8859_1";
	encodings["latin1"] = "ISO8859_1";
	encodings["l1"] = "ISO8859_1";
	encodings["IBM819"] = "ISO8859_1";
	encodings["CP819"] = "ISO8859_1";
	encodings["csISOLatin1"] = "ISO8859_1";
}

Encoding::~Encoding()
{
}

string Encoding::getEncoding(std::string encoding)
{
	if(_encoding == 0)
	{
		_encoding = new Encoding();
	}
	
	return encodings[encoding];
}