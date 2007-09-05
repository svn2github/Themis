/*
Copyright (c) 2004 Raymond "Z3R0 One" Rodgers. All Rights Reserved. 

Permission is hereby granted, free of charge, to any person 
obtaining a copy of this software and associated documentation 
files (the "Software"), to deal in the Software without 
restriction, including without limitation the rights to use, 
copy, modify, merge, publish, distribute, sublicense, and/or 
sell copies of the Software, and to permit persons to whom 
the Software is furnished to do so, subject to the following 
conditions: 

   The above copyright notice and this permission notice 
   shall be included in all copies or substantial portions 
   of the Software. 

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY 
KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE 
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR 
PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS 
OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR 
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR 
OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE 
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Original Author & Project Manager: Raymond "Z3R0 One" Rodgers (z3r0_one@users.sourceforge.net)
Project Start Date: October 18, 2000
*/
/*!
\file
\brief This file contains the definition of the URIProcessor class.
*/
#include "uriprocessor.h"
#include <string.h>
#include <stdio.h>
URIProcessor::URIProcessor(void)
{
	uri=NULL;
	Set("/");
}
URIProcessor::URIProcessor(const char *URI)
{
	uri=NULL;
	Set(URI);
}
URIProcessor::~URIProcessor()
{
}
status_t URIProcessor::GetParent(URIProcessor *URI)
{
//	 char *slash=(char*)uri;
	char *lastslash=NULL;
		int32 length=0;
	if (*(uri+(strlen(uri)-1))=='/')
	{
		lastslash=strrchr(uri,'/');
		char *old=lastslash;
		(*old)=0x0;
		lastslash=strrchr(uri,'/');
		(*old)='/';
		length=(lastslash-uri)+1;
		const char *tempuri=new char[length+1];
		memset((char*)tempuri,0,length+1);
		strncpy((char*)tempuri,uri,length);
		URI->Set(tempuri);
		delete tempuri;
		memset((char*)tempuri,0,length+1);
		tempuri=NULL;
	}
	else
	{
		lastslash=strrchr(uri,'/');
		length=(lastslash-uri)+1;
		const char *tempuri=new char[length+1];
		memset((char*)tempuri,0,length+1);
		strncpy((char*)tempuri,uri,length);
		URI->Set(tempuri);
		delete tempuri;
		memset((char*)tempuri,0,length+1);
		tempuri=NULL;
	}
	return B_OK;
}

const char *URIProcessor::Set(const char *URI)
{
	
	if (uri!=NULL)
	{
		memset((char*)uri,0,strlen(uri)+1);
		delete uri;
		uri=NULL;
	}
	if (URI==NULL)
	{
		uri=new char[2];
		strcpy((char*)uri,"/\0");
		return uri;
	}
	if (URI[0]!='/')
		return NULL;
	int32 length=strlen(URI);
	uri=new char[length+1];
	memset((char*)uri,0,length+1);
	strcpy((char*)uri,URI);
	printf("URI: %s\n",uri);
	return uri;
}
bool URIProcessor::Contains(const char *URI)
{
	uint32 length=strlen(uri);
	if (strlen(URI)>length)
		return false;
	if (strncmp(uri,URI,min_c(length,strlen(URI)))==0)
		return true;
	return false;
}
void URIProcessor::operator=(const char *URI)
{
	Set(URI);
}

void URIProcessor::operator=(URIProcessor &URI)
{
	Set(URI.uri);
}
const char *URIProcessor::String(void)
{
	return uri;
}
int32 URIProcessor::Length(void)
{
	return strlen(uri);
}
