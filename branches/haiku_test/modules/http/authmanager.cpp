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
*/#include "authmanager.h"
#include "httpv4.h"
#include <string.h>
#include <Autolock.h>
#include "authtype.h"
#include "authtypebasic.h"
#include "authtypedigest.h"

AuthManager::AuthManager(HTTPv4 *HTTPptr,int32 AuthenticationMode)
{
	HTTP=HTTPptr;
	auth_mode=AuthenticationMode;
	auth_list=NULL;
}

AuthManager::~AuthManager()
{
	if (auth_list!=NULL)
	{
		auth_info_st *current=auth_list;
		while(current!=NULL)
		{
			auth_list=auth_list->next;
			delete current;
			current=auth_list;
		}
		
	}
	
}

auth_info_st *AuthManager::DistributableCopy(auth_info_st *auth_item)
{
	auth_info_st *copy=NULL;
		if (auth_item!=NULL)
		{
			copy=new auth_info_st;
			copy->auth_type=auth_item->auth_type;
			copy->auth_object=auth_item->auth_object;
			copy->original_item=auth_item;
			//we leave the next pointer set to NULL; that way the new owner of this
			//structure item can delete it without worrying about screwing up the list
		}
	return copy;
}
void AuthManager::InvalidateAuthInfo(http_request_info_st *http_request)
{
	BAutolock autolock(auth_lock);
	if (autolock.IsLocked())
	{
		if (http_request->auth_info==NULL)
			return;
		auth_info_st *current=auth_list,*last=NULL;
		while (current!=NULL)
		{
			if (current->auth_object->ValidFor(http_request->host,http_request->port,http_request->uri))
			{
				if (last==NULL)
				{
					auth_list=auth_list->next;
					delete current;
					current=NULL;
				}
				else
				{
					last->next=current->next;
					delete current;
					current=NULL;
				}
				http_request->auth_info=NULL;
				if (http_request->auth_win!=NULL)
				{
					http_request->auth_win->Lock();
					http_request->auth_win->Quit();
					http_request->auth_win=NULL;
				}
				break;
			}
			last=current;
			current=current->next;
		}
	}
}
bool AuthManager::HasAuthInfo(http_request_info_st *http_request)
{
	bool found=false;
	BAutolock autolock(auth_lock);
	if (autolock.IsLocked())
	{
		if (http_request->auth_info!=NULL)
			found=true;
		else
		{
			auth_info_st *current=auth_list;
			while (current!=NULL)
			{
				if( current->auth_object->ValidFor(http_request->host,http_request->port,http_request->uri))
					{
						if (http_request->auth_info==NULL)
							http_request->auth_info=current;
						found=true;
						break;
					}
				current=current->next;
			}
		}
	}
	return found;
}

status_t AuthManager::GetAuth(http_request_info_st *http_request)
{
	status_t status=B_ENTRY_NOT_FOUND;
	BAutolock autolock(auth_lock);
	if (autolock.IsLocked())
	{
		if (http_request->auth_win!=NULL)
			http_request->auth_win->Activate(true);
		else
		{
			const char *realm=NULL;
			int32 method=AUTHENTICATION_TYPE_BASIC;
			const char *value=NULL;
			switch(http_request->http_status_code)
			{
				case 401:
				{
					value=HTTP->FindHeader(http_request,"WWW-Authenticate");
					method|=AUTHENTICATION_SERVER_AUTH;
				}break;
				case 407:
				{
					value=HTTP->FindHeader(http_request,"Proxy-Authenticate");
					method|=AUTHENTICATION_PROXY_AUTH;
				}break;
			}
//			char *type=NULL;
			char *space=strchr(value,0x20);
			int32 length=space-value;
			if (strncasecmp("basic",value,5)==0)
				method|=AUTHENTICATION_TYPE_BASIC;
			else
				if (strncasecmp("digest",value,6)==0)
					method|=AUTHENTICATION_TYPE_DIGEST;
			char *start=strstr(value,"realm=\"");
			start+=7;
			char *stop=strchr(start,'"');
			length=stop-start;
			realm=new char[length+1];
			memset((char*)realm,0,length+1);
			strncpy((char*)realm,start,length);
			printf("Auth Manager auth code: %ld\n",method);
			//if digest, then search for nonce, algorithm,and qop
			if ((method&AUTHENTICATION_SERVER_AUTH)!=0)
			{
			if (http_request->auth_info==NULL)	
				http_request->auth_win=new authwin(this,"Password Required",http_request,realm,method,false);
			else
				http_request->auth_win=new authwin(this,"Password Required",http_request,realm,method,true);
			} else
			{
				HTTP->SetProxyAuthInfo(NULL);
					if (HTTP->proxy_auth_win==NULL)
						HTTP->proxy_auth_win=new authwin(this,"Proxy Server Requires Password",http_request,realm,method,false);
					else
						HTTP->proxy_auth_win->Activate(true);
			}
		}
	}
	return status;
}
status_t AuthManager::AddProxyAuthentication(http_request_info_st *http_request,const char *user, const char *pass,const char *realm,int32 auth_method)
{
	BAutolock autolock(auth_lock);
	if (autolock.IsLocked())
	{
	printf("AddProxyAuthentication: %s %s %ld\n",http_request->host,pass,(auth_method&AUTHENTICATION_TYPE_BASIC));
	auth_info_st *proxy_auth=new auth_info_st;
	if ((auth_method&AUTHENTICATION_TYPE_BASIC)!=0)
	{
		printf("Adding basic proxy info to HTTP\n");
		proxy_auth->auth_object=new AuthTypeBasic(http_request->host,http_request->port,"/",user,pass,realm);
		proxy_auth->auth_type=auth_method;
		HTTP->SetProxyAuthInfo(proxy_auth);
		
	} else
	{
		if ((auth_method&AUTHENTICATION_TYPE_DIGEST)!=0)
		{
		}
	}
	HTTP->proxy_auth_win=NULL;
	printf("AddProxyAuthentication done\n");
	}
	return B_OK;
}
status_t AuthManager::AddAuthentication(http_request_info_st *http_request,const char *user, const char *pass,const char *realm,int32 auth_method)
{
	status_t status=B_ERROR;
	BAutolock autolock(auth_lock);
	if (autolock.IsLocked())
	{
		if (auth_mode!=AUTHENTICATION_TYPE_ALL)
			if (auth_mode!=auth_method)
				return status;
		printf("request->auth_info: %p\n",http_request->auth_info);
		http_request->auth_win=NULL;
		if (http_request->auth_info!=NULL)
		{
			if ((http_request->auth_info->auth_object!=NULL) &&
			(http_request->auth_info->auth_object==http_request->auth_info->original_item->auth_object))
			{
				auth_info_st *temp=http_request->auth_info;
				temp->auth_object->SetUser(user);
				temp->auth_object->SetPassword(pass);
				return B_OK;
			}
		}
		auth_info_st *new_item=new auth_info_st;
		new_item->auth_type=auth_method;
			http_request->auth_info=DistributableCopy(new_item);
		if ((auth_method&AUTHENTICATION_TYPE_DIGEST)!=0)
		{
			new_item->auth_object=new AuthTypeDigest(user,pass,realm);
		}
		if ((auth_method&AUTHENTICATION_TYPE_BASIC)!=0)
		{
			new_item->auth_object=new AuthTypeBasic(http_request->host,http_request->port,http_request->uri,user,pass,realm);
		}
		if (auth_list==NULL)
		{
			auth_list=new_item;
			
		} else
		{
			auth_info_st *last=auth_list;
			while (last->next!=NULL)
				last=last->next;
			last->next=new_item;
		}
	}
	return status;
}
const char *AuthManager::AuthCreditials(http_request_info_st *http_request)
{
	return NULL;
}
