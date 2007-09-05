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
#ifndef _http_auth_man_
#define _http_auth_man_

#include <SupportKit.h>
#include <Locker.h>
#include "authtype.h"
class AuthManager;

struct auth_info_st
{
	private:
	auth_info_st *original_item;
	friend class AuthManager;	
	public:
	int32 auth_type;
	AuthType *auth_object;
	auth_info_st *next;
	auth_info_st()
	{
		auth_type=0;
		original_item=NULL;
		auth_object=NULL;
		next=NULL;
	}
	~auth_info_st()
	{
		if (original_item==NULL)
			delete auth_object;
		else
			original_item=NULL;
		auth_type=0;
		next=NULL;
		auth_object=NULL;
	}
};

struct http_request_info_st;
class HTTPv4;
/*!
	\brief The authentication manager class.
	
	This class manages the authentication support for the HTTP add-on.
	
	@todo I need to implement Digest support in AuthTypeDigest.
*/
class AuthManager
{
	private:
		int32 auth_mode;
		auth_info_st *auth_list;
	
		auth_info_st *DistributableCopy(auth_info_st *auth_item);
		BLocker auth_lock;
		HTTPv4 *HTTP;
	public:
		AuthManager(HTTPv4 *HTTPptr,int32 AuthenticationMode=AUTHENTICATION_TYPE_ALL);
		~AuthManager();
		bool HasAuthInfo(http_request_info_st *http_request);
		status_t GetAuth(http_request_info_st *http_request);
		void InvalidateAuthInfo(http_request_info_st *http_request);
		status_t AddProxyAuthentication(http_request_info_st *http_request,const char *user, const char *pass,const char *realm,int32 auth_method);
		status_t AddAuthentication(http_request_info_st *http_request,const char *user, const char *pass,const char *realm,int32 auth_method);
		const char *AuthCreditials(http_request_info_st *http_request);
		const static int32 AUTHENTICATION_SERVER_AUTH=0x01;
		const static int32 AUTHENTICATION_PROXY_AUTH=0x02;
		const static int32 AUTHENTICATION_TYPE_BASIC=0x04;
		const static int32 AUTHENTICATION_TYPE_DIGEST=0x08;
		const static int32 AUTHENTICATION_TYPE_NONE=0x10;
		const static int32 AUTHENTICATION_TYPE_UNKNOWN=0x20;
		const static int32 AUTHENTICATION_TYPE_ALL=0x40;
};

#endif
