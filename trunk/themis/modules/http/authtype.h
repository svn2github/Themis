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
#ifndef _auth_type_
#define _auth_type_
#include "cryptlib.h"

#include "SupportDefs.h"
/*!
	\brief The base class for the various authentication types.
	
	This base class keeps the encrypted representation of the password in memory.
*/
class AuthType
{
	private:
	CRYPT_CONTEXT cryptContext;
	CRYPT_ENVELOPE encryptEnvelope,decryptEnvelope;
	protected:
	const char *auth_realm;
	const char *auth_user;
	unsigned char *auth_pass;
	const char *auth_host;
	int16 auth_pass_len;
	uint16 auth_port;
	const char *auth_uri;
	public:
	AuthType(void);
	AuthType(const char *host,uint16 port, const char *uri,const char *user,const char *password,const char *realm);
	virtual ~AuthType(void);
	virtual int32 AuthMode(void);
	const char *Realm(void);
	const char *User(void);
	const char *Password(void);
	const char *Host(void);
	const char *URI(void);
	uint16 Port(void);
	bool SetRealm(const char *realm);
	bool SetUser(const char *user);
	bool SetPassword(const char *password);
	bool SetHost(const char *host);
	bool SetURI(const char *uri);
	bool SetPort(uint16 port);
	virtual void Clear(void);
	virtual const char *Credentials(void);
	virtual bool ValidFor(const char *Host,uint16 Port, const char *URI);

};

#endif
