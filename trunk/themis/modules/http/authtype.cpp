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
#include "authtype.h"
#include "authtypedigest.h"
#include "authtypebasic.h"
#include "authmanager.h"
#include "uriprocessor.h"
#include <kernel/OS.h>
#include <string.h>
AuthType::AuthType(void)
{
	auth_realm=auth_user=NULL;
	auth_pass=NULL;
	auth_host=auth_uri=NULL;
	auth_port=0;
	auth_pass_len=0;
	cryptCreateContext(&cryptContext,CRYPT_UNUSED,CRYPT_ALGO_IDEA);
	char key[17];
	memset(key,0,17);
	bigtime_t curtime=real_time_clock(),systime=system_time();
	sprintf(key,"%08x%08x",(uint32)curtime,(uint32)systime);
	cryptSetAttributeString(cryptContext,CRYPT_CTXINFO_KEY,key,16);
	memset(key,0,17);
}
AuthType::AuthType(const char *host,uint16 port, const char *uri,const char *user,const char *password,const char *realm)
{
	cryptCreateContext(&cryptContext,CRYPT_UNUSED,CRYPT_ALGO_IDEA);
	char key[17];
	memset(key,0,17);
	bigtime_t curtime=real_time_clock(),systime=system_time();
	sprintf(key,"%08x%08x",(uint32)curtime,(uint32)systime);
	cryptSetAttributeString(cryptContext,CRYPT_CTXINFO_KEY,key,16);
	auth_realm=auth_user=NULL;
	auth_pass=NULL;
	auth_host=auth_uri=NULL;
	auth_pass_len=0;
	auth_port=0;
	SetHost(host);
	SetPort(port);
	SetURI(uri);
	SetUser(user);
	SetPassword(password);
	SetRealm(realm);
}
AuthType::~AuthType(void)
{
	Clear();
	cryptDestroyContext(cryptContext);
}

int32 AuthType::AuthMode(void)
{
	return AuthManager::AUTHENTICATION_TYPE_NONE;
}
const char *AuthType::Realm(void)
{
	return auth_realm;
}
const char *AuthType::User(void)
{
	return auth_user;
}
const char *AuthType::Password(void)
{
	//unencrypt password before returning.
	cryptCreateEnvelope(&decryptEnvelope,CRYPT_UNUSED,CRYPT_FORMAT_AUTO);
	int bytes=0;
	cryptPushData(decryptEnvelope,auth_pass,auth_pass_len,&bytes);
	cryptSetAttribute(decryptEnvelope,CRYPT_ENVINFO_SESSIONKEY,cryptContext);
	cryptFlushData(decryptEnvelope);
	const char *password=new char[32769];
	memset((char*)password,0,32769);
	cryptPopData(decryptEnvelope,(char*)password,32768,&bytes);
	cryptDestroyEnvelope(decryptEnvelope);
	return password;
}
bool AuthType::SetHost(const char *host)
{
	if (auth_host!=NULL)
	{
		memset((char*)auth_host,0,strlen(auth_host)+1);
		delete auth_host;
	}
	if (host==NULL)
	{
		auth_host=NULL;
		return true;
	}
	auth_host=new char[strlen(host)+1];
	if (auth_host==NULL)
		return false;
	memset((char*)auth_host,0,strlen(host)+1);
	strcpy((char*)auth_host,host);
	if (strcmp(auth_host,host)!=0)
		return false;
	else
		return true;
}
bool AuthType::SetURI(const char *uri)
{
	if (auth_uri!=NULL)
	{
		memset((char*)auth_uri,0,strlen(auth_uri)+1);
		delete auth_uri;
	}
	if (uri==NULL)
	{
		auth_uri=NULL;
		return true;
	}
	auth_uri=new char[strlen(uri)+1];
	if (auth_uri==NULL)
		return false;
	memset((char*)auth_uri,0,strlen(uri)+1);
	strcpy((char*)auth_uri,uri);
	if (strcmp(auth_uri,uri)!=0)
		return false;
	else
		return true;
}
bool AuthType::SetPort(uint16 port)
{
	auth_port=port;
	return true;
}
bool AuthType::SetRealm(const char *realm)
{
	if (auth_realm!=NULL)
	{
		memset((char*)auth_realm,0,strlen(auth_realm)+1);
		delete auth_realm;
	}
	if (realm==NULL)
	{
		auth_realm=NULL;
		return true;
	}
	auth_realm=new char[strlen(realm)+1];
	if (auth_realm==NULL)
		return false;
	memset((char*)auth_realm,0,strlen(realm)+1);
	strcpy((char*)auth_realm,realm);
	if (strcmp(auth_realm,realm)!=0)
		return false;
	else
		return true;
}
bool AuthType::SetUser(const char *user)
{
	if (auth_user!=NULL)
	{
		memset((char*)auth_user,0,strlen(auth_user)+1);
		delete auth_user;
	}
	if (user==NULL)
	{
		auth_user=NULL;
		return true;
	}
	auth_user=new char[strlen(user)+1];
	if (auth_user==NULL)
		return false;
	memset((char*)auth_user,0,strlen(user)+1);
	strcpy((char*)auth_user,user);
	if (strcmp(auth_user,user)!=0)
		return false;
	else
		return true;
}
bool AuthType::SetPassword(const char *password)
{
	if (auth_pass!=NULL)
	{
		memset(auth_pass,0,auth_pass_len+1);
		delete auth_pass;
	}
	if (password==NULL)
	{
		auth_pass=NULL;
		return true;
	}
	cryptCreateEnvelope(&encryptEnvelope,CRYPT_UNUSED,CRYPT_FORMAT_CRYPTLIB);
	cryptSetAttribute(encryptEnvelope,CRYPT_ENVINFO_SESSIONKEY,cryptContext);	
	cryptSetAttribute(encryptEnvelope,CRYPT_ENVINFO_DATASIZE,strlen(password));
	int bytes=0;
	cryptPushData(encryptEnvelope,password,strlen(password),&bytes);
	cryptFlushData(encryptEnvelope);
	auth_pass=new unsigned char[32769];
	if (auth_pass==NULL)
		return false;
	memset(auth_pass,0,32769);
	cryptPopData(encryptEnvelope,auth_pass,32768,&bytes);
	auth_pass_len=bytes;
	cryptDestroyEnvelope(encryptEnvelope);
	return true;
}
void AuthType::Clear(void)
{
	if (auth_realm!=NULL)
	{
		memset((char*)auth_realm,0,strlen(auth_realm)+1);
		delete auth_realm;
		auth_realm=NULL;
	}
	if (auth_user!=NULL)
	{
		memset((char*)auth_user,0,strlen(auth_user)+1);
		delete auth_user;
		auth_user=NULL;
	}
	if (auth_pass!=NULL)
	{
		memset(auth_pass,0,auth_pass_len+1);
		delete auth_pass;
		auth_pass=NULL;
	}
}
const char *AuthType::Credentials(void)
{
	return NULL;
}
bool AuthType::ValidFor(const char *Host,uint16 Port, const char *URI)
{
	URIProcessor urip(auth_uri);
	if ((strcasecmp(Host,auth_host)==0) &&
		(Port==auth_port) &&
		(urip.Contains(URI)))
		return true;
	return false;
}
