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


//Set-Cookie: VisitorID=4; expires=Wed, 18-Feb-2004 04:15:52 GMT; path=/
//Set-Cookie: NewVisitor=Yes; expires=Wed, 19-Feb-2003 06:15:52 GMT; path=/    
//Set-Cookie: PREF=ID=431d7dc03b37acb1:TM=1045628490:LM=1045628490:S=sq2HIMlzaeuvRebF; expires=Sun, 17-Jan-2038 19:14:07 GMT; path=/; domain=.google.com

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
		discard=true;
		secure=false;
		name=NULL;
		value=NULL;
		path=NULL;
		domain=NULL;
		expiredate=0;
		datereceived=0;
		maxage=0;
		comment=NULL;
		commenturl=NULL;
		ports=NULL;
		next=NULL;
	}
	~cookie_st() {
		if (name) {
			printf("%p - name: %s\n",this,name);
			
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

/*!
\brief The cookie management subsystem main class.

This class is the heart of the cookie management subsystem.
*/
class CookieManager {
	private:
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
		void ProcessAttributes(const char *attributes, cookie_st *cookie, const char *request_host, const char *request_uri);
	
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
		
		BLocker *lock;
	public:
		cookie_st ** FindCookies(int32 *count,const char *domain, const char *uri,bool secure=false);
		CookieManager();
		~CookieManager();
		int32 SetCookie(const char *header, const char *request_host, const char *request_uri);
		char *GetCookie(const char *host, const char *path, bool secure=false);
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
		
};

#endif
