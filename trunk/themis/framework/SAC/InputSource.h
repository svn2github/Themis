#ifndef __INPUTSOURCE_H__
#define __INPUTSOURCE_H__

#include <iostream>
#include <string>

namespace sac
{

class InputSource
{
	private:
		std::string uri;
		std::istream* byteStream;
		std::string encoding;
		std::istream* characterStream;
		std::string title;
		std::string media;
		
	public:
		InputSource();
		InputSource(const std::string& uri);
		InputSource(std::istream& characterStream);
		~InputSource();
		
		void setURI(const std::string& uri);
		std::string getURI() const;
		void setByteStream(std::istream& byteStream);
		std::istream& getByteStream() const;
		void setEncoding(const std::string& encoding);
		std::string getEncoding() const;
		void setCharacterStream(std::istream& characterStream);
		std::istream& getCharacterStream() const;
		void setTitle(const std::string& title);
		std::string getTitle() const;
		void setMedia(const std::string& media);
		std::string getMedia() const;
};

}

#endif // __INPUTSOURCE_H__