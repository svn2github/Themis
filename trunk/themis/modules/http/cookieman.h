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
\brief The header file for the cookie management subsystem.


*/
#ifndef _cookie_manager_
#define _cookie_manager_

#include <string.h>
#include <stdio.h>
#include <SupportKit.h>
#include <time.h>
#include <Entry.h>
#include <Locker.h>
#include <Message.h>
#include "msgsystem.h"
#include "prefsman.h"

//Set-Cookie: VisitorID=4; expires=Wed, 18-Feb-2004 04:15:52 GMT; path=/
//Set-Cookie: NewVisitor=Yes; expires=Wed, 19-Feb-2003 06:15:52 GMT; path=/    
//Set-Cookie: PREF=ID=431d7dc03b37acb1:TM=1045628490:LM=1045628490:S=sq2HIMlzaeuvRebF; expires=Sun, 17-Jan-2038 19:14:07 GMT; path=/; domain=.google.com
/*!
	\brief Sort the cookies.
	
	This function sorts cookies first on their version number, and then on their paths.
	The reason that they are sorted first on their version number is to make sure that
	RFC 2109 cookies (which use the Cookie & Set-Cookie headers) are listed together and
	separate from the RFC 2965 cookies (which use Cookie2 and Set-Cookie2 headers). In
	truth, there's very little difference between the two, but I believe that it's important
	to send the cookies back the way they came.
*/
int qsort_cookies(const void *Alpha, const void *Beta);

/*!
\brief The linked list structure for cookies.


*/
struct cookie_st {
	//! The cookie's name.
	char *name;
	//! The cookie's value.
	char *value;
	//! The path on the server in which the cookie is valid. This should be case sensitive.
	char *path;
	//! The domain in which the cookie is valid. This should not be case sensitive.
	char *domain;
	//! Whether the cookie can be transmitted over an unsecure connection or not.
	bool secure;
	//! Determines whether the cookie will be stored more permanently than the current browser session.
	bool discard;
	//! Session cookie
	bool session;
	//! Determines whether the cookie should be transmitted for HTTP requests only, or should allow Javascript ones as well.
	bool httponly;
	//! The Unix time stamp of when the cookie should expire.
	int32 expiredate;
	//! The Unix time stamp from when the cookie was received. Used in combination with maxage.
	int32 datereceived;
	//! The maximum age (in seconds?) that a cookie can live.
	int32 maxage;
	//! The cookie specification version number.
	int8 version;
	//! A comment supplied with the cookie.
	char *comment;
	//! (The url of the cookie was set at?)
	char *commenturl;
	//! A list of the ports that are valid for this cookie.
	char *ports;
	//! pointer to the next cookie in the linked list
	cookie_st *next;
	//! Location of the cookie on disk (if applicable)
	entry_ref ref;
	cookie_st() {
		discard=false;//discard cookie on shutdown if not set to false later
		session = true; //same as discard, mostly, except there is a specific cookie flag for discard, but not for session
		secure=false;
		httponly = false;//not particularly useful right now. RR 1/9/2011
		name=NULL;
		value=NULL;
		path=NULL;
		domain=NULL;
		expiredate=0;
		datereceived=0;
		maxage=86400;//1 day
		comment=NULL;
		commenturl=NULL;
		ports=NULL;
		next=NULL;
		version=0;
	}
	cookie_st(cookie_st *DEFAULT) {
		discard = DEFAULT->discard;
		session = DEFAULT->session;
		secure = DEFAULT->secure;
		httponly = DEFAULT->httponly;
		int len = strlen(DEFAULT->path);
		path = new char[len+1];
		memset(path,0,len+1);
		strncpy(path,DEFAULT->path,len);
		len = strlen(DEFAULT->domain);
		domain = new char[len+1];
		memset(domain,0,len+1);
		strncpy(domain,DEFAULT->domain,len);
		expiredate = DEFAULT->expiredate;
		datereceived = DEFAULT->datereceived;
		maxage = DEFAULT->maxage;
		ports = NULL;
		next = NULL;
		version = DEFAULT->version;
		if( DEFAULT->comment != NULL)
		{
			len = strlen(DEFAULT->comment);
			comment = new char[len+1];
			memset(comment,0,len+1);
			strncpy(comment,DEFAULT->comment,len);
		}
		else comment = NULL;
		if( DEFAULT->commenturl != NULL)
		{
			len = strlen(DEFAULT->commenturl);
			commenturl = new char[len+1];
			memset(commenturl,0,len+1);
			strncpy(commenturl,DEFAULT->commenturl,len);
		}
		else commenturl = NULL;
	}
	~cookie_st() {
		if (name) {
			//printf("%p - name: %s\n",this,name);
			
			memset(name,0,strlen(name));
			delete name;
			name=NULL;
		}
		if (value) {
			memset(value,0,strlen(value));
			delete value;
			value=NULL;
		}
		if (path){
			memset(path,0,strlen(path));
			delete path;
			path=NULL;
		}
		if (domain){
			memset(domain,0,strlen(domain));
			delete domain;
			domain=NULL;
		}
		if (comment){
			memset(comment,0,strlen(comment));
			delete comment;
			comment=NULL;
		}
		if (commenturl){
			memset(commenturl,0,strlen(commenturl));
			delete commenturl;
			commenturl=NULL;
		}
		if (ports) {
			memset(ports,0,strlen(ports));
			delete ports;
			ports=NULL;
		}
		
		next=NULL;
		
	}
	
};

class HTTPv4;
/*!
\brief The cookie management subsystem main class.

This class is the heart of the cookie management subsystem.
@todo Add a SetCookie2 function; this function will allow for cookie names
that are identical to other attribute names. The reason is that the Cookie2 specification
states that the first attribute-value pair is the cookie name & value pair. The original
specification doesn't include that, and so it's possible that cookie names that are
identical to an attribute name will be mistaken for that attribute. Our implementation
can be affected by this!
*/
class CookieManager:public MessageSystem {
	private:
		PrefsManager *prefMan;
		bool strictMode;
		/*!
		\brief This function breaks a cookie header up into it's name value pairs.
		
		This function will break a cookie header line up into an appropriate cookie_st structure. If
		a cookie header line contains more than one cookie, this function will return an appropriately
		sized linked list containing all of the cookie values.
		*/
		cookie_st * ParseHeader(const char *header, const char *request_host, const char *request_uri);
		/*!
		\brief This function processes a cookie's attributes into the provided cookie structure.
		*/
		void ProcessAttributes(char **attributes, cookie_st *cookie, const char *request_host, const char *request_uri);
	
		//! The start of the cookie linked list.
		cookie_st *cookie_head;
		//! After cookie creation, the cookie must go through validation checks before it is added officially.
		bool Validate(cookie_st *cookie, const char *request_host, const char *request_uri);
		//! This function prints a validation failure message and returns false.
		bool ValidateFail(const char *reason);
		void PrintCookie(cookie_st *cookie);
		/*!
		\brief A case-insensitve version of strstr().
		
		Since domain names are not case sensitive, it was necessary to find or write a verison of
		the strstr() function found in string.h.  As it was a simple thing to do, in concept at least,
		I took the liberty of writing it.
		*/
		char *stristr(const char *str1,const char *str2);
		/*!
		\brief Locates and/or creates the cookie storage directory.
		*/
		void FindCookieDirectory();
		/*!
		\brief Checks the cookie mimetype.
		*/
		void CheckMIME();
		/*!
		\brief Checks and creates necessary indices.
		*/
		void CheckIndicies();
		//! Stores the location of the cookie directory.
		entry_ref cookiedirref;
		//! Stores the cookie system's settings
		BLocker lock;
		/*!
			\brief Loads a specific cookie file from disk.
		*/
		status_t LoadCookie(cookie_st *cookie);
		/*!
			\brief Saves a specific cookie to disk.	
		*/
		status_t SaveCookie(cookie_st *cookie);
		/*!
			\brief Searches the cookies in memory for one that matches.
		*/
		cookie_st *FindCookie(const char *name,const char *path,const char *domain);
		/*!
			\brief Checks to see if the cookie passed in is a duplicate.
		*/
		bool DuplicatedCookie(cookie_st *cookie);
	public:
		static const uint32 EXPIRED = 'expc';
		BMessage *CookieSettings;
		/*!
			\brief Finds cookies appropriate for the requested domain and URI.
		*/
		cookie_st ** FindCookies(int32 *count,const char *domain, const char *uri,bool secure=false);
		CookieManager();
		~CookieManager();
		int32 SetCookie(const char *header, const char *request_host, const char *request_uri,uint16 port,bool secure=false);
		const char *GetCookie(const char *host, const char *path, uint16 port,bool secure=false);
		void PrintCookies();
		/*!
		\brief Clears all cookies out of memory and off disk.
		
		This function will remove all cookies out of the cookie system. This should only be used for
		system maintenance, or if the user requests.
		
		Note that this is different from what the CookieManager destructor does. The destructor simply
		clears the in memory record of all cookies, including any cookies that are marked "discard".
		*/
		void ClearAllCookies();
		/*!
		\brief This function removes expired cookies out of memory and off of disk.
		*/
		void ClearExpiredCookies();
		/*!
		\brief Make sure that the settings can be stored by the http_protocol class at shutdown.
		*/
		friend class HTTPv4;
		/*!
		\brief Loads all cookies on disk into RAM.
		*/
		void LoadAllCookies();
		/*!
		\brief Saves all unexpired, non-discardable cookies to disk.
		*/
		void SaveAllCookies();
		status_t ReceiveBroadcast(BMessage *msg);
		uint32 BroadcastTarget();
};

#endif
