/*
Copyright (c) 2000 Z3R0 One. All Rights Reserved.

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

Original Author & Project Manager: Z3R0 One (z3r0_one@yahoo.com)
Project Start Date: October 18, 2000
*/
#ifndef _ProtocolPlugClass
#define _ProtocolPlugClass

#include <String.h>
#include "plugclass.h"
class ProtocolPlugClass: public PlugClass
 {
  public:
   BString URL;
   virtual const char *SetURL(const char* url) {URL=url; return URL.String();}
   virtual BMessage *SupportedTypes(void) {return NULL;}
   virtual void Config(BMessage *msg) {}
   virtual void FindURI(const char *url,BString &host,int &port,BString &uri) {}
   virtual void ParseResponse(unsigned char *resp,size_t size) {}
   virtual unsigned char *GetDoc(BString &host,int &port,BString &uri) {return NULL;}
   virtual unsigned char *GetDoc(const char* url) {return NULL;}
   virtual int32 GetURL(BMessage *info) {return 0;} //use in combination with SpawnThread and StartThread
   virtual unsigned char *GetURL(const char* url) {return NULL;}
 };

typedef ProtocolPlugClass protocol_plugin;
//enum PlugType{ProtocolPlugin=0,ContentPlugin=1};
/*
class protocol_plugin
 {
  public:
   protocol_plugin(){}
   virtual ~protocol_plugin(){};
   virtual char *GetPluginName(void)=0;
   virtual int32 GetPluginID(void)=0;
   virtual float GetPluginVers(void)=0;
   virtual PlugType PluginType(void)=0;
   virtual status_t Go(void)=0;
   virtual bool IsPersistant(void)=0;
   virtual BMessage *SupportedTypes(void)=0;
   virtual void FindURI(const char *url,BString &host,int &port,BString &uri)=0;
   virtual void ParseResponse(unsigned char *resp,size_t size)=0;
   virtual unsigned char *GetDoc(BString &host,int &port,BString &uri)=0;
   virtual unsigned char *GetDoc(const char* url)=0;
   virtual unsigned char *GetURL(const char* url)=0;
   volatile uint8 Cancel;
   BWindow *Window;
 };
*/
#endif
