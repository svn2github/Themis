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
#include "networkableobject.h"
#include <Locker.h>
struct connection;

/*!
\brief This is the base class for the protocol plug-ins.
*/
class ProtocolPlugClass: public PlugClass, public _Themis_Networking_::NetworkableObject {
	protected:
		static BLocker UOU_lock;
	public:
		ProtocolPlugClass(BMessage *info=NULL,const char *msg_system_name=NULL);
		BString URL;
	
		virtual const char *SetURL(const char* url);
		virtual BMessage *SupportedTypes(void);
		virtual void Config(BMessage *msg);
		virtual void FindURI(const char *url,BString &host,int &port,BString &uri);
		virtual void ParseResponse(unsigned char *resp,size_t size);
		virtual unsigned char *GetDoc(BString &host,int &port,BString &uri);
		virtual unsigned char *GetDoc(const char* url);
		virtual int32 GetURL(BMessage *info);
		int32 TypePrimary();
//		virtual void ConnectionEstablished(connection *conn)=0;
//		virtual void ConnectionDisconnected(connection *conn,uint32 reason=0)=0;
//		virtual void DataWaiting(connection *conn)=0;
		static int32 UnObfuscateURL(const char *obfuscated,char *unobfuscated, int32 max_size, bool host_only=false);
		static int32 UnObfuscateHost(const char *obfuscated, char *unobfuscated, int32 max_size);
		
};

typedef ProtocolPlugClass protocol_plugin;
#endif
