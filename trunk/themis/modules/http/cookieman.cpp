#include "cookieman.h"
#include <stdlib.h>
//#include "stripwhite.h"
//remove this function before adding to repository; it is here only for development purposes.
#include <ctype.h>
#include <String.h>
#include <Autolock.h>
#ifdef _Themis_
#include "stripwhite.h"
#else
char *trim(char *target) 
{
	char *result=NULL;
	char *f=target,*t=target+(strlen(target)-1);
	while (isspace(*f))
		f++;
	while (isspace(*t)) {
		t--;
	}
	t++;
	int size=t-f;
	if (size>0) {
		result=new char[size+1];
		memset(result,0,size+1);
		strncpy(result,f,size);
		return result;
	}
	
	return NULL;
}
#endif

int qsort_cookies(const void *Alpha, const void *Beta) {
	cookie_st **alpha=(cookie_st**)Alpha,**beta=(cookie_st**)Beta;
	cookie_st *one=*alpha,*two=*beta;
	if (strlen(one->path)>strlen(two->path))
		return -1;
	if (strlen(one->path)<strlen(two->path))
		return 1;
	return 0;
}

CookieManager::CookieManager() {
	printf("CookieManager()\n");
	cookie_head=NULL;
	lock=new BLocker("cookie manager lock",true);
}

CookieManager::~CookieManager() {
	printf("~CookieManager()\n");
	if (cookie_head!=NULL) {
		cookie_st *cur=NULL;
		while (cookie_head!=NULL) {
			cur=cookie_head->next;
			delete cookie_head;
			cookie_head=cur;
		}
	}
	delete lock;
}
void CookieManager::PrintCookies() {
	printf("===============\n");
	if (cookie_head!=NULL) {
		cookie_st *cur=cookie_head;
		while (cur!=NULL) {
			PrintCookie(cur);
			cur=cur->next;
		}
	} else {
		printf("No cookies in system.\n");
	}
	printf("===============\n");
}
cookie_st **CookieManager::FindCookies(int32 *count,const char *domain, const char *uri,bool secure) {
 cookie_st **cookiearr=NULL;
 *count=0;
 if (cookie_head!=NULL) {
 	cookie_st *cur=cookie_head;
 	int32 mycount=0;
 	while (cur!=NULL) {
 		if (stristr(domain,cur->domain)!=NULL) { //is this the right domain?
 			//we have a candidate that matches the right domain.
 			if (strlen(uri)>=strlen(cur->path)) { //is cookie path possible parent of uri?
 				//the cookie's path must be less than or equal to the request uri's length
 				if (strncmp(cur->path,uri,strlen(cur->path))==0) {//correct path?
 					//the right cookie path!
 					if ((cur->secure) && (secure)) {//secure?
 						//bingo! we have a secure match!
 						mycount++;
 					} else {//secure?
 						if (cur->secure==false) {
 							//bingo we have an insecure match
 							mycount++;
 						} else {
 							//damn, so close. No match because the cookie must be sent securely.
 						}
 					}//secure?
 				}//correct path?
 			}//is cookie path possible parent of uri?
 		}//is this the right domain?
 		cur=cur->next;
 	}
 	if (mycount>0) {
 		cookiearr=new cookie_st*[mycount];
 		cur=cookie_head;
 		int32 i=0;
 		while (cur!=NULL) {
	 		if (stristr(domain,cur->domain)!=NULL) { //is this the right domain?
	 			//we have a candidate that matches the right domain.
	 			if (strlen(uri)>=strlen(cur->path)) { //is cookie path possible parent of uri?
	 				//the cookie's path must be less than or equal to the request uri's length
	 				if (strncmp(cur->path,uri,strlen(cur->path))==0) {//correct path?
	 					//the right cookie path!
	 					if ((cur->secure) && (secure)) {//secure?
	 						//bingo! we have a secure match!
	 						cookiearr[i]=cur;
	 						i++;
	 					} else {//secure?
	 						if (cur->secure==false) {
	 							//bingo we have an insecure match
	 							cookiearr[i]=cur;
	 							i++;
	 						} else {
	 							//damn, so close. No match because the cookie must be sent securely.
	 						}
	 					}//secure?
	 				}//correct path?
	 			}//is cookie path possible parent of uri?
	 		}//is this the right domain?
	 		cur=cur->next;
 		}
 		qsort(cookiearr,mycount,sizeof(cookie_st*),qsort_cookies);
 		*count=mycount;
 	}
 }
 return cookiearr;
}
int32 CookieManager::SetCookie(const char *header, const char *request_host, const char *request_uri) {
	int32 count=0;
	BAutolock alock(lock);
	if (alock.IsLocked()) {
		printf("SetCookie(): %s\n",header);
		cookie_st *cookie=ParseHeader(header, request_host,request_uri);
		if (cookie!=NULL) {
					int32 cookies=0;
					cookie_st **cookarr=FindCookies(&cookies,request_host,request_uri,true);
			cookie_st *cur=cookie;
			bool valid=false;
			while (cur!=NULL) {
				if ((cur->maxage==0) && (cur->expiredate==0)) {
					/*
						If there is no expiration date listed, and no maximum age listed,
						then allow the cookie to live for 24 hours (provided the browser is
						open that long) or until the browser is closed. (Thus the discard value.)
					*/
					cur->maxage=86400;
					cur->expiredate=cur->datereceived+cur->maxage;
					cur->discard=true;
				}
				valid=Validate(cur,request_host,request_uri);
				printf("Valid? (test) %s\n",valid? "yes":"no");
				if (valid) {
					if ((cookies>0) && (cookarr!=NULL)) {
						bool duplicated=false;
						for (int32 i=0; i<cookies; i++) {
							printf("Cookie comparisons:\nPointer:\t%p\t%p\n",cookarr[i],cur);
							printf("Name:\t%s\t%s\t%d\n",cookarr[i]->name,cur->name,strcmp(cookarr[i]->name,cur->name));
							printf("Domain:\t%s\t%s\t%d\n",cookarr[i]->domain,cur->domain,strcasecmp(cookarr[i]->domain,cur->domain));
							printf("Path:\t%s\t%s\t%d\n",cookarr[i]->path,cur->path,strcmp(cookarr[i]->path,cur->path));
							if ((strcmp(cookarr[i]->name,cur->name)==0) &&
							(strcasecmp(cookarr[i]->domain,cur->domain)==0) &&
							(strcmp(cookarr[i]->path,cur->path)==0)) {
								//hey! we have this cookie already... just update the existing one...
								printf("updating #1\n");
								delete cookarr[i]->value;
								cookarr[i]->value=new char[1+strlen(cur->value)];
								memset(cookarr[i]->value,0,1+strlen(cur->value));
								strcpy(cookarr[i]->value,cur->value);
								cookarr[i]->expiredate=cur->expiredate;
								cookarr[i]->maxage=cur->maxage;
								cookarr[i]->secure=cur->secure;
								cookarr[i]->discard=cur->discard;
								cookie_st *cur2=cur->next;
								delete cur;
								cur=cur2;
								duplicated=true;
								break;
							} 
						}
						if (duplicated)
							continue;
					}
					cookie_st *cur2=cookie_head;
					count++;
					if (cur2==NULL) {
						cookie_head=cur;
						cur2=cur->next;
						cur->next=NULL;
						cur=cur2;
						continue;
					} else {
						while (cur2->next!=NULL)
							cur2=cur2->next;
						cur2->next=cur;
						cur2=cur->next;
						cur->next=NULL;
						cur=cur2;
						continue;
					}
				} else {
					if ((cookies>0) && (cookarr!=NULL)) {
						bool duplicated=false;
						for (int32 i=0; i<cookies; i++) {
							printf("Cookie comparisons:\nPointer:\t%p\t%p\n",cookarr[i],cur);
							printf("Name:\t%s\t%s\t%d\n",cookarr[i]->name,cur->name,strcmp(cookarr[i]->name,cur->name));
							printf("Domain:\t%s\t%s\t%d\n",cookarr[i]->domain,cur->domain,strcasecmp(cookarr[i]->domain,cur->domain));
							printf("Path:\t%s\t%s\t%d\n",cookarr[i]->path,cur->path,strcmp(cookarr[i]->path,cur->path));
							if ((strcmp(cookarr[i]->name,cur->name)==0) &&
							(strcasecmp(cookarr[i]->domain,cur->domain)==0) &&
							(strcmp(cookarr[i]->path,cur->path)==0)) {
								//hey! we have this cookie already... just update the existing one...
								printf("updating #2\n");
								delete cookarr[i]->value;
								cookarr[i]->value=new char[1+strlen(cur->value)];
								memset(cookarr[i]->value,0,1+strlen(cur->value));
								strcpy(cookarr[i]->value,cur->value);
								cookarr[i]->expiredate=cur->expiredate;
								cookarr[i]->maxage=cur->maxage;
								cookarr[i]->secure=cur->secure;
								cookarr[i]->discard=cur->discard;
								cookie_st *cur2=cur->next;
								delete cur;
								cur=cur2;
								duplicated=true;
								break;
							} 
						}
						if (duplicated)
							continue;
					}
					cookie_st *cur2=cur->next;
					delete cur;
					cur=cur2;
					continue;
				}
	//			cur=cur->next;
			}
			delete []cookarr;
	
		}
		ClearExpiredCookies();
	}
	return count;
		
}

char *CookieManager::GetCookie(const char *host, const char *path, bool secure) {
	BAutolock alock(lock);
	if (alock.IsLocked()) {
		printf("GetCookie()\n");
		int32 count=0;
		cookie_st **cookies=FindCookies(&count,host,path,secure);
		if ((cookies!=NULL) && (count>0)) {
			char *cookiestr=NULL;
			BString cstr("Cookie: ");
			for (int32 i=0; i<count; i++) {
				cstr << cookies[i]->name<<"="<<cookies[i]->value;
				if ((i+1)<count)
					cstr << " ; ";
			}
			delete []cookies;
			cstr<<"\r\n";
			cookiestr=new char[cstr.Length()+1];
			memset(cookiestr,0,cstr.Length()+1);
			strcpy(cookiestr,cstr.String());
			return cookiestr;
		}
	}
	return NULL;
}

void CookieManager::ClearAllCookies() {
	BAutolock alock(lock);
	if (alock.IsLocked()) {
		printf("ClearAllCookies()\n");
		if (cookie_head!=NULL) {
			cookie_st *cur=NULL;
			while (cookie_head!=NULL) {
				cur=cookie_head->next;
				delete cookie_head;
				cookie_head=cur;
			}
		}
	}
}
void CookieManager::ClearExpiredCookies() {
	BAutolock alock(lock);
	if (alock.IsLocked()) {
		printf("Clearing expired cookies...\n");
		time_t currenttime=time(NULL);
		currenttime=mktime(gmtime(&currenttime));
		cookie_st *cur=cookie_head, *last=NULL;
		while (cur!=NULL) {
			printf("Expire %s: current time: %ld\texpire time: %ld\tdifference: %2.2f\n",cur->name,currenttime,cur->expiredate,difftime(cur->expiredate,currenttime));
			if (cur->expiredate<=currenttime) {
				if (cur==cookie_head) {
					cookie_head=cookie_head->next;
					delete cur;
					last=cur=cookie_head;
					continue;
				} else {
					last->next=cur->next;
					delete cur;
					cur=last->next;
					continue;
				}
			}
			last=cur;
			cur=cur->next;
		}
	}
}
void CookieManager::ProcessAttributes(const char *attributes,cookie_st *cookie, const char *request_host, const char *request_uri) {
	printf("ProcessAttributes: cookie - \"%s\" attributes - \"%s\"\n",cookie->name,attributes);
	cookie->datereceived=time(NULL);
	cookie->datereceived=mktime(gmtime(&cookie->datereceived));
	
	char *attr=new char[strlen(attributes)+1];
	memset(attr,0,strlen(attributes)+1);
	strcpy(attr,attributes);
	int commas=0;
	char *comma=attr;
	while (comma!=NULL) {
		comma=strchr(comma,',');
		if (comma!=NULL) {
			comma=comma+1;
			commas++;
		}
	}
	char **atts=new char*[commas+1];
	atts[0]=strtok(attr,",");
	for (int i=1; i<(commas+1); i++)
		atts[i]=strtok(NULL,",");
	char *equal=NULL;
	char *item=NULL;
	char *value=NULL;
	int len=0;
	for (int i=0; i<(commas+1); i++) {
		equal=strchr(atts[i],'=');
		if (equal!=NULL) {
			
			len=(equal-atts[i]);
			item=new char[1+len];
			memset(item,0,1+len);
			strncpy(item,atts[i],len);
			if (isspace(item[0])) {
				int j=0;
				while ((isspace(atts[i][j])) && (atts[i][j]!=NULL))
					j++;
				atts[i]=atts[i]+j;
//				int l2=strlen(item)-j;
//				char *tmp=new char[l2+1];
//				memset(tmp,0,l2+1);
//				strncpy(tmp,item+j,l2);
//				memset(item,0,strlen(item));
//				strcpy(item,tmp);
//				delete tmp;
//				tmp=NULL;
				i--;
				continue;
			}
			len=strlen(atts[i])-(len+1);
			value=new char[len+1];
			memset(value,0,len+1);
			strcpy(value,equal+1);
			printf("\tattribute: %s\n\tvalue: %s\n",item,value);
			//damn it, the string that was broken up because of the comma might be a date...
			if (strcasecmp(item,"expires")==0) {
				//yep, it was... so...
				//reassemble the date string for further processing...
				atts[i][strlen(atts[i])+1]=' ';
				printf("atts: %s%s\n",atts[i],atts[i+1]);
	//			commas--;
				char *datestr;
				len=strlen(atts[i])+strlen(atts[i+1]);
				datestr=new char[len+1];
				memset(datestr,0,len+1);
				strcpy(datestr,atts[i]);
				strcat(datestr,atts[i+1]);
				i++;
				//finish processing the expiration date...
				char *date=NULL;
				char *tyme=NULL;
				char *tz=NULL;
				struct tm time_st;
				char *space=NULL;
				char *last=NULL;
				space=strchr(datestr,' ');
				last=strchr(space+1,' ');
				len=last-space;
				date=new char[len+1];
				memset(date,0,len+1);
				strncpy(date,space+1,len);
				space=strchr(last+1,' ');
				last=last+1;
				len=space-last;
				tyme=new char[len+1];
				memset(tyme,0,len+1);
				strncpy(tyme,last,len);
				tz=space+1;
				printf("Expiration:\n\tdate: %s\n\ttime: %s\n",date,tyme);
				char *tymearr[3];
				tymearr[0]=strtok(tyme,":");
				for (int i=1; i<3; i++)
					tymearr[i]=strtok(NULL,":");
				time_st.tm_hour=atoi(tymearr[0]);
				time_st.tm_min=atoi(tymearr[1]);
				time_st.tm_sec=atoi(tymearr[2]);
printf("time: %d:%d:%d\n",time_st.tm_hour,time_st.tm_min,time_st.tm_sec);			
				delete tyme;
				char *datearr[3];
				datearr[0]=strtok(date,"-");
				for (int i=1; i<3; i++) 
					datearr[i]=strtok(NULL,"-");
				int32 val=0;
				for (int i=0; i<3; i++) {
					if (isdigit(datearr[i][0])) {
						val=atol(datearr[i]);
						if (val>31) {
							time_st.tm_year=val-1900;
						} else {
							time_st.tm_mday=val;
						}
						
					} else {
						switch(tolower(datearr[i][0])) {
							case 'j': {//january, june, july
								if (strncasecmp(datearr[i],"jan",3)==0) {
									val=0;
								} else {
									if (strncasecmp(datearr[i],"jun",3)==0){
										val=5;
										
									} else
										val=6;
								}
								
								
								time_st.tm_mon=val;
							}break;
							case 'f': {//february
								time_st.tm_mon=1;
							}break;
							case 'm': {//march, may
								if (strncasecmp(datearr[i],"mar",3)==0)
									val=2;
								else
									val=4;
								
								time_st.tm_mon=val;
							}break;
							case 'a': {//april, august
								if (strncasecmp(datearr[i],"apr",3)==0)
									val=3;
								else
									val=7;
								time_st.tm_mon=val;
							}break;
							case 's': {//september
								time_st.tm_mon=8;
							}break;
							case 'o': {//october
								time_st.tm_mon=9;
							}break;
							case 'n': {//november
								time_st.tm_mon=10;
							}break;
							case 'd': {//december 
								time_st.tm_mon=11;
							}break;
							
						}
						
					}
					
				}
				printf("tz: %s\n",tz);
/*!
\note
Currently, this code does not take into account the presence of a GMT offset. This can
be added later if necessary, but I have not seen any cookies at this time (21/Feb/2003)
that have an offset present.
*/				
				time_st.tm_zone=tz;
//				time_st.tm_gmtoff=-8;
//				time_st.tm_isdst=0;
				
				cookie->expiredate=mktime(&time_st);
				cookie->maxage=cookie->expiredate-cookie->datereceived;
				printf("time experiment: %s\n",ctime(&cookie->expiredate));
				
				delete date;
				delete datestr;
				time_t curtime=time(NULL);
				if (cookie->expiredate>mktime(gmtime(&curtime)))
					cookie->discard=false;
			}
			if (strcasecmp(item,"version")==0) {
				cookie->version=atoi(value);
			}
			if (strcasecmp(item,"max-age")==0) {
				cookie->maxage=atol(value);
				cookie->expiredate=cookie->maxage+cookie->datereceived;
				time_t curtime=time(NULL);
				if (cookie->expiredate>mktime(gmtime(&curtime)))
					cookie->discard=false;
				
			}
			if (strcasecmp(item,"path")==0) {
//				len=strlen(value);
				
				cookie->path=new char[len+1];
				memset(cookie->path,0,len+1);
				strcpy(cookie->path,value);
			}
			if (strcasecmp(item,"domain")==0) {
//				len=strlen(value);
				cookie->domain=new char[len+1];
				memset(cookie->domain,0,len+1);
				strcpy(cookie->domain,value);
				
			}
			if (strcasecmp(item,"comment")==0) {
				cookie->comment=new char[len+1];
				memset(cookie->comment,0,len+1);
				strcpy(cookie->comment,value);
			}
			if (strcasecmp(item,"commenturl")==0) {
				cookie->commenturl=new char[len+1];
				memset(cookie->commenturl,0,len+1);
				strcpy(cookie->commenturl,value);
			}
			if (strcasecmp(item,"ports")==0) {
				cookie->ports=new char[len+1];
				memset(cookie->ports,0,len+1);
				strcpy(cookie->ports,value);
			}
			
			delete item;
			delete value;
		} else {
			//the attribute was a single word; not an name/value pair
			if (strcasecmp(atts[i],"discard")==0) {
				cookie->discard=true;
				printf("\t*** DISCARD THIS COOKIE\n");
				
			}
			if (strcasecmp(atts[i],"secure")==0) {
				cookie->secure=true;
				printf("\t*** ONLY SEND THIS COOKIE IF SECURE\n");
			}
			
		}
		
	
	}
	
	delete attr;
	delete []atts;
	if (cookie->domain==NULL) {
		len=strlen(request_host);
		cookie->domain=new char[len+1];
		memset(cookie->domain,0,len+1);
		strcpy(cookie->domain,request_host);
	}
	if (cookie->path==NULL) {
		char *lastslash=strrchr(request_uri,'/');
		if (lastslash==request_uri)
			lastslash=lastslash+1;
		len=lastslash-request_uri;
		cookie->path=new char[len+1];
		memset(cookie->path,0,len+1);
		strncpy(cookie->path, request_uri,len);
	}
}
bool CookieManager::ValidateFail(const char *reason) {
	printf("Cookie Validation Failure: %s\n",reason);
	return false;
}
void CookieManager::PrintCookie(cookie_st *cookie) {
	BAutolock alock(lock);
	if (alock.IsLocked()) {
		printf("\t=======\n\tDomain:\t%s\n\tPath:\t%s\n",cookie->domain,cookie->path);
		printf("\tName:\t%s\n\tValue:\t%s\n",cookie->name,cookie->value);
		printf("\tExpiration Date:\t%s\n\tMax-Age:\t%ld\n",ctime(&cookie->expiredate),cookie->maxage);
		printf("\tDate Received:\t%s\n",ctime(&cookie->datereceived));
		printf("\tDiscard On Close:\t%s\n",cookie->discard?"yes":"no");
		printf("\tRequires Secure Connection:\t%s\n",cookie->secure?"yes":"no");
		printf("\t=======\n");
	}
}
char *CookieManager::stristr(const char *str1, const char *str2) {
	BAutolock alock(lock);
	if (alock.IsLocked()) {
		int32 alen=strlen(str1);
		int32 blen=strlen(str2);
		char *alpha=new char[alen+1];
		memset(alpha,0,alen+1);
		char *bravo=new char[blen+1];
		memset(bravo,0,blen+1);
		strcpy(alpha,str1);
		strcpy(bravo,str2);
		for (int32 i=0; i<alen; i++)
			alpha[i]=tolower(alpha[i]);
		for (int32 i=0; i<blen; i++)
			bravo[i]=tolower(bravo[i]);
		char *delta=NULL;
		delta=strstr(alpha,bravo);
		if (delta!=NULL) {
			int32 dlen=delta-alpha;
			memset(alpha,0,alen+1);
			memset(bravo,0,blen+1);
			delete alpha;
			delete bravo;
			delta=alpha=bravo=NULL;
			return (char*)(str1+dlen);
		}
		memset(alpha,0,alen+1);
		memset(bravo,0,blen+1);
		delete alpha;
		delete bravo;
		alpha=bravo=NULL;
	}
	return NULL;
}
bool CookieManager::Validate(cookie_st *cookie, const char *request_host, const char *request_uri) {
	PrintCookie(cookie);
	BAutolock alock(lock);
	bool valid=false;
	if (alock.IsLocked()) {
		valid=true;
		time_t currenttime=time(NULL);
		currenttime=mktime(gmtime(&currenttime));
		printf("expiredate: %ld\ncurrent time: %ld\n",cookie->expiredate, currenttime);
		if (cookie->expiredate<=currenttime)
			return ValidateFail("Cookie has already expired.");
		if ((cookie->maxage+cookie->datereceived)<=currenttime)
			return ValidateFail("Cookie has already expired.");
		if (cookie->domain==NULL)
			return ValidateFail("Cookie domain is NULL.");
		else {
			if (cookie->domain[0]!='.') {
/*
Strictly speaking, all domains must start with a period. However, in an e-mail conversation
with Lou Montulli (one of the authors of the RFC 2109 and 2965), I came udner the impression
that there could be an exception to this rule. If the domain isn't supplied, then the
request host [name] or ip address would be a valid alternative, however the cookie should
only be sent back to that host; no others even in the same domain.

With this in mind, I'm making this possibility available in code here.
*/
				if (stristr(request_host,cookie->domain)==NULL)
					return ValidateFail("Cookie domain does not match request host's domain, and does not begin with a \".\". (4.3.2:2,3)");
			} else {
				int dots=1;
				for (uint i=2; i<strlen(cookie->domain); i++) {
					//there must be at least two dots in the domain; and those must be separated
					//by at least one character.
					if (cookie->domain[i]=='.')
						dots++;
				}
				if (dots<2)
					return ValidateFail("There domain contained no embedded dots. (4.3.2:2)");
				if (strlen(cookie->domain)>strlen(request_host)) {
					return ValidateFail("Cookie domain is longer than request host's domain.");
				}
				if (stristr(request_host,cookie->domain)==NULL)
					return ValidateFail("Cookie domain does not match request host's domain. (4.3.2:3)");
			}
			if (strlen(cookie->domain)<3)
				return ValidateFail("Cookie domain is too small. 3 or fewer characters.");
		}
		if (cookie->path==NULL)
			return ValidateFail("Cookie path is NULL.");
		else {
			printf("path: %s\n",cookie->path);
			if (strncmp(cookie->path,request_uri,strlen(cookie->path))!=0)
				return ValidateFail("Cookie path is not a prefix of the request URI. (4.3.2:1)");
		}
	}
	return valid;
}

cookie_st * CookieManager::ParseHeader(const char *header, const char *request_host, const char *request_uri) {
	BAutolock alock(lock);
	cookie_st *cookie=NULL;
	if (alock.IsLocked()) {
		printf("ParseHeader()\n");
		
		int32 len=strlen(header);
		char *copy=new char[len+1];
		memset(copy,0,len+1);
		strcpy(copy,header);
		char *start=strstr(copy,": ");
		if (start!=NULL) {
			start=start+2;
			printf("cookie: \"%s\"\n",start);
			char *semi=start;
			int semis=0;
			int sections=0;
			
			do
			 {
				semi=strchr(semi,';');
				if (semi!=NULL) {
					semis++;
					semi=semi+1;
				}
			}while (semi!=NULL);
			printf("%d semicolon(s) found\n",semis);
			sections=semis+1;
			int commas=0;
			char *comma=start;
			
			//handle section 1 first, obviously. Section 1 should always contain the actual
			//cookie name/value pairs, per section 4.2.2 of RFC 2109.
			semi=strchr(start,';');
			do {
				comma=strchr(comma,',');
				if (comma!=NULL) {
					if (comma<semi) {
						printf("comma found: \"%s\"\n",comma);
						commas++;
						comma=comma+1;
					}
				}
			}while ((comma<semi) && (comma!=NULL));
			printf("%d comma[s] found in section 1\n",commas);
			char **nvpairs=NULL;
			
			nvpairs=new char*[commas+1];
			int counter=0;
			comma=start;
			char *a=NULL;
					char *temp=NULL;
			do {
				a=comma;
				comma=strchr(comma,',');
				if ((comma==NULL) || (comma>semi)) {
					int len=semi-a;
					temp=new char[len+1];
					memset(temp,0,len+1);
					strncpy(temp,a,len);
					nvpairs[counter]=trim(temp);
					delete temp;
					
				} else {
					int len=comma-a;
					temp=new char[len+1];
					memset(temp,0,len+1);
					strncpy(temp,a,len);
					nvpairs[counter]=trim(temp);
					delete temp;
					comma=comma+1;
				}
				
				counter++;
			}while (counter<(commas+1));
			cookie=new cookie_st;
			cookie_st *cur=cookie;
			char *equal=NULL;
			for (int i=0; i<(commas+1); i++) {
	//			printf("cookie: %s\n",nvpairs[i]);
				equal=strchr(nvpairs[i],'=');
				if (equal!=NULL) {
					int len=equal-nvpairs[i];
					cur->name=new char[len+1];
					memset(cur->name,0,len+1);
					strncpy(cur->name,nvpairs[i],len);
					len=strlen(nvpairs[i])-(len+1);
					cur->value=new char[len+1];
					memset(cur->value,0,len+1);
					strncpy(cur->value,equal+1,len);
					printf("\tcookie name: %s\n\tcookie value: %s\n",cur->name,cur->value);
					if ((i+1)<(commas+1))
					 {
					 	cur->next=new cookie_st;
						 cur=cur->next;
						 
					 }
					 
				}
				
			}
			for (int i=0; i<(commas+1); i++) 
				delete nvpairs[i];
			delete []nvpairs;
			
			//process all remaining attribute sections
			char **section=new char *[semis];
			char *last=semi+1;
			semi=semi+1;
			start=semi;
			
			for (int i=0 ; i<semis; i++) {
				semi=strchr(semi,';');
				if (semi==NULL) {
					semi=last+strlen(last);
				}
				int len=semi-last;
				temp=new char[len+1];
				memset(temp,0,len+1);
				strncpy(temp,last,len);
				printf("section: %s\n",temp);
				section[i]=trim(temp);
				delete temp;
				temp=NULL;
				last=semi+1;
				semi=semi+1;
				
			}
	//		if (section[semis-1]==NULL)
	//			semi--;
			cur=cookie;
			while (cur!=NULL) {
				for (int i=0; i<semis; i++) {
					if (section[i]!=NULL)
						ProcessAttributes(section[i],cur,request_host,request_uri);
				}
				cur=cur->next;
			}
			
			
			for (int i=0; i<semis; i++)
				delete section[i];
			delete []section;
			
			
			
			
		}
		
		delete copy;
	}	
	return cookie;
}
