#include "InputSource.h"

using namespace sac;
using namespace std;

InputSource::InputSource()
:byteStream(NULL),
 characterStream(NULL),
 media("all")
{
}

InputSource::InputSource(const string& uri)
:uri(uri),
 byteStream(NULL),
 characterStream(NULL),
 media("all")
{
}

InputSource::InputSource(istream& characterReader)
:byteStream(NULL),
 characterStream(&characterReader),
 media("all")
{
}

InputSource::~InputSource()
{
}

void InputSource::setURI(const string& uri)
{
	this->uri = uri;
}

string InputSource::getURI() const
{
	return uri;
}

void InputSource::setByteStream(istream& byteStream)
{
	this->byteStream = &byteStream;
}

istream& InputSource::getByteStream() const
{
	return *byteStream;
}

void InputSource::setEncoding(const string& encoding)
{
	this->encoding = encoding;
}

string InputSource::getEncoding() const
{
	return encoding;
}

void InputSource::setCharacterStream(istream& characterStream)
{
	this->characterStream = &characterStream;
}

istream& InputSource::getCharacterStream() const
{
	return *characterStream;
}

void InputSource::setTitle(const string& title)
{
	this->title = title;
}

string InputSource::getTitle() const
{
	return title;
}

void InputSource::setMedia(const string& media)
{
	this->media = media;
}

string InputSource::getMedia() const
{
	return media;
}