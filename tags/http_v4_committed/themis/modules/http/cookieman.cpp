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
\brief This file contains the definition of the CookieManager class.
*/
#include "cookieman.h"
#include <stdlib.h>
//#include "stripwhite.h"
//remove this function before adding to repository; it is here only for development purposes.
#include <ctype.h>
#include <String.h>
#include <Autolock.h>
#include <Directory.h>
#include <FindDirectory.h>
#include <Path.h>
#include <Mime.h>
#include <Entry.h>
#include <VolumeRoster.h>
#include <Node.h>
#include <Volume.h>
#include <kernel/fs_index.h>
#include <fs_attr.h>
#include <Query.h>
#include "commondefs.h"
#include "httpv4.h"
#include "date.h"
#include "uriprocessor.h"
extern HTTPv4 *HTTP;
#include "PrefsDefs.h"
#ifdef _Themis_
#include "stripwhite.h"
#else
char *trim(char *target) 
{
	if (target==NULL)
		return NULL;
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
	if (one->version==two->version)
	{
		if (strlen(one->path)>strlen(two->path))
			return -1;
		if (strlen(one->path)<strlen(two->path))
			return 1;
	} else
	{
		if (one->version<two->version)
			return -1;
		return 1;
	}
	return 0;
}

CookieManager::CookieManager():MessageSystem("Cookie Manager") {
	MsgSysRegister(this);
	printf("CookieManager()\n");
	CookieSettings=new BMessage();
	cookie_head=NULL;
//	lock=new BLocker("cookie manager lock",true);
	printf("HTTPv4: %p\n",HTTP);
	printf("AppSettings: %p\n",HTTP->AppSettings);
	if ((HTTP!=NULL) && (HTTP->AppSettings!=NULL))
		if (HTTP->AppSettings->HasMessage("cookie_settings"))
			HTTP->AppSettings->FindMessage("cookie_settings",CookieSettings);
	printf("Checking MIME...\n");
	CheckMIME();
/*
At the moment, there is a debate going on within myself that asks whether cookies should
all be loaded into RAM at the start of the cookie manager or whether they should be dynamically
loaded when needed. It's not hard to do it either way, it's just a question of memory usage
versus coding challenge and time.

For the time being, I'm going to load all cookies at start up.
*/
//	printf("Checking Indicies...\n");
//	CheckIndicies();
	printf("Finding cookie directory...\n");
	FindCookieDirectory();
	LoadAllCookies();
}

CookieManager::~CookieManager() {
	printf("~CookieManager()\n");
	MsgSysUnregister(this);
	SaveAllCookies();
	BAutolock alock(lock);
	if (alock.IsLocked()) {
		if (cookie_head!=NULL) {
			cookie_st *cur=NULL;
			while (cookie_head!=NULL) {
				cur=cookie_head->next;
				delete cookie_head;
				cookie_head=cur;
			}
		}
	}
//	delete lock;
	delete CookieSettings;
	printf("cookie mime: %s\n",ThemisCookieFile);
	printf("shutting down at: %ld\n",time(NULL));
}
void CookieManager::CheckMIME() {
	BMimeType mime(ThemisCookieFile);//application/x-Themis-cookie
	printf("InitCheck: %ld\n",mime.InitCheck());
	printf("MIME Type valid? %s\n",mime.IsValid()?"yes":"no");
	if (!mime.IsInstalled()) {
		status_t stat=mime.Install();
		printf("Cookie MIME Type Installed (%s): %ld\n",ThemisCookieFile,stat);
	}
	char type[B_MIME_TYPE_LENGTH];
	mime.GetShortDescription(type);
	if (strcasecmp(type,"Themis Cookie File")!=0)
		mime.SetShortDescription("Themis Cookie File");
	mime.GetLongDescription(type);
	if (strcasecmp(type,"Themis Cookie File")!=0)
		mime.SetLongDescription("Themis Cookie File");
	mime.GetPreferredApp(type);
	if (strcasecmp(type,ThemisAppSig)!=0)
		mime.SetPreferredApp(ThemisAppSig);
	BMessage attrinf;
	mime.GetAttrInfo(&attrinf);
	bool installall=false;
	int32 attrcount=0;
	if (attrinf.IsEmpty())
		installall=true;
	else {
		type_code typec;
		attrinf.GetInfo("attr:name",&typec,&attrcount);
	}
	bool found=false;
	//Make Themis:creceivedate a visible attribute
	for (int32 i=0;i<attrcount;i++) {
		BString item;
		attrinf.FindString("attr:name",i,&item);
		if (item=="Themis:creceivedate") {
			found=true;
			break;
		}
	}
	if ((!found) || (installall)) {
		attrinf.AddString("attr:name","Themis:creceivedate");
		attrinf.AddString("attr:public_name","Date Received");
		attrinf.AddInt32("attr:type",B_TIME_TYPE);
		attrinf.AddInt32("attr:width",100);
		attrinf.AddInt32("attr:alignment",B_ALIGN_CENTER);
		attrinf.AddBool("attr:public",true);
		attrinf.AddBool("attr:editable",true);
		attrinf.AddBool("attr:viewable",true);
		attrinf.AddBool("attr:extra",false);
	} else
		found=false;
	//Make Themis:domain a visible attribute
	for (int32 i=0;i<attrcount;i++) {
		BString item;
		attrinf.FindString("attr:name",i,&item);
		if (item=="Themis:domain") {
			found=true;
			break;
		}
	}
	if ((!found) || (installall)) {
		attrinf.AddString("attr:name","Themis:domain");
		attrinf.AddString("attr:public_name","Domain");
		attrinf.AddInt32("attr:type",B_STRING_TYPE);
		attrinf.AddInt32("attr:width",200);
		attrinf.AddInt32("attr:alignment",B_ALIGN_CENTER);
		attrinf.AddBool("attr:public",true);
		attrinf.AddBool("attr:editable",true);
		attrinf.AddBool("attr:viewable",true);
		attrinf.AddBool("attr:extra",false);
	} else
		found=false;
	//Make Themis:cookiepath a visible attribute
	for (int32 i=0;i<attrcount;i++) {
		BString item;
		attrinf.FindString("attr:name",i,&item);
		if (item=="Themis:cookiepath") {
			found=true;
			break;
		}
	}
	if ((!found) || (installall)) {
		attrinf.AddString("attr:name","Themis:cookiepath");
		attrinf.AddString("attr:public_name","Cookie Path");
		attrinf.AddInt32("attr:type",B_STRING_TYPE);
		attrinf.AddInt32("attr:width",200);
		attrinf.AddInt32("attr:alignment",B_ALIGN_CENTER);
		attrinf.AddBool("attr:public",true);
		attrinf.AddBool("attr:editable",true);
		attrinf.AddBool("attr:viewable",true);
		attrinf.AddBool("attr:extra",false);
	} else
		found=false;
	//Make Themis:cookiename a visible attribute
	for (int32 i=0;i<attrcount;i++) {
		BString item;
		attrinf.FindString("attr:name",i,&item);
		if (item=="Themis:cookiename") {
			found=true;
			break;
		}
	}
	if ((!found) || (installall)) {
		attrinf.AddString("attr:name","Themis:cookiename");
		attrinf.AddString("attr:public_name","Cookie Name");
		attrinf.AddInt32("attr:type",B_STRING_TYPE);
		attrinf.AddInt32("attr:width",200);
		attrinf.AddInt32("attr:alignment",B_ALIGN_CENTER);
		attrinf.AddBool("attr:public",true);
		attrinf.AddBool("attr:editable",true);
		attrinf.AddBool("attr:viewable",true);
		attrinf.AddBool("attr:extra",false);
	} else
		found=false;
	//Make Themis:cookievalue a visible attribute
	for (int32 i=0;i<attrcount;i++) {
		BString item;
		attrinf.FindString("attr:name",i,&item);
		if (item=="Themis:cookievalue") {
			found=true;
			break;
		}
	}
	if ((!found) || (installall)) {
		attrinf.AddString("attr:name","Themis:cookievalue");
		attrinf.AddString("attr:public_name","Cookie Value");
		attrinf.AddInt32("attr:type",B_STRING_TYPE);
		attrinf.AddInt32("attr:width",200);
		attrinf.AddInt32("attr:alignment",B_ALIGN_CENTER);
		attrinf.AddBool("attr:public",true);
		attrinf.AddBool("attr:editable",true);
		attrinf.AddBool("attr:viewable",true);
		attrinf.AddBool("attr:extra",false);
	} else
		found=false;
	//Make Themis:expiredate a visible attribute
	for (int32 i=0;i<attrcount;i++) {
		BString item;
		attrinf.FindString("attr:name",i,&item);
		if (item=="Themis:expiredate") {
			found=true;
			break;
		}
	}
	if ((!found) || (installall)) {
		attrinf.AddString("attr:name","Themis:expiredate");
		attrinf.AddString("attr:public_name","Expiration Date");
		attrinf.AddInt32("attr:type",B_TIME_TYPE);
		attrinf.AddInt32("attr:width",100);
		attrinf.AddInt32("attr:alignment",B_ALIGN_CENTER);
		attrinf.AddBool("attr:public",true);
		attrinf.AddBool("attr:editable",true);
		attrinf.AddBool("attr:viewable",true);
		attrinf.AddBool("attr:extra",false);
	} else
		found=false;
	//Make Themis:cookiesecure a visible attribute
	for (int32 i=0;i<attrcount;i++) {
		BString item;
		attrinf.FindString("attr:name",i,&item);
		if (item=="Themis:cookiesecure") {
			found=true;
			break;
		}
	}
	if ((!found) || (installall)) {
		attrinf.AddString("attr:name","Themis:cookiesecure");
		attrinf.AddString("attr:public_name","Secure");
		attrinf.AddInt32("attr:type",B_BOOL_TYPE);
		attrinf.AddInt32("attr:width",50);
		attrinf.AddInt32("attr:alignment",B_ALIGN_CENTER);
		attrinf.AddBool("attr:public",true);
		attrinf.AddBool("attr:editable",true);
		attrinf.AddBool("attr:viewable",true);
		attrinf.AddBool("attr:extra",false);
	} else
		found=false;
	//Make Themis:cookieports a visible attribute
	for (int32 i=0;i<attrcount;i++) {
		BString item;
		attrinf.FindString("attr:name",i,&item);
		if (item=="Themis:cookieports") {
			found=true;
			break;
		}
	}
	if ((!found) || (installall)) {
		attrinf.AddString("attr:name","Themis:cookieports");
		attrinf.AddString("attr:public_name","Ports");
		attrinf.AddInt32("attr:type",B_STRING_TYPE);
		attrinf.AddInt32("attr:width",100);
		attrinf.AddInt32("attr:alignment",B_ALIGN_CENTER);
		attrinf.AddBool("attr:public",true);
		attrinf.AddBool("attr:editable",true);
		attrinf.AddBool("attr:viewable",true);
		attrinf.AddBool("attr:extra",false);
	} else
		found=false;
	//Make Themis:cookiecomment a visible attribute
	for (int32 i=0;i<attrcount;i++) {
		BString item;
		attrinf.FindString("attr:name",i,&item);
		if (item=="Themis:cookiecomment") {
			found=true;
			break;
		}
	}
	if ((!found) || (installall)) {
		attrinf.AddString("attr:name","Themis:cookiecomment");
		attrinf.AddString("attr:public_name","Comment");
		attrinf.AddInt32("attr:type",B_STRING_TYPE);
		attrinf.AddInt32("attr:width",200);
		attrinf.AddInt32("attr:alignment",B_ALIGN_CENTER);
		attrinf.AddBool("attr:public",true);
		attrinf.AddBool("attr:editable",true);
		attrinf.AddBool("attr:viewable",true);
		attrinf.AddBool("attr:extra",false);
	} else
		found=false;
	//Make Themis:cookiecommenturl a visible attribute
	for (int32 i=0;i<attrcount;i++) {
		BString item;
		attrinf.FindString("attr:name",i,&item);
		if (item=="Themis:cookiecommenturl") {
			found=true;
			break;
		}
	}
	if ((!found) || (installall)) {
		attrinf.AddString("attr:name","Themis:cookiecommenturl");
		attrinf.AddString("attr:public_name","Comment URL");
		attrinf.AddInt32("attr:type",B_STRING_TYPE);
		attrinf.AddInt32("attr:width",200);
		attrinf.AddInt32("attr:alignment",B_ALIGN_CENTER);
		attrinf.AddBool("attr:public",true);
		attrinf.AddBool("attr:editable",true);
		attrinf.AddBool("attr:viewable",true);
		attrinf.AddBool("attr:extra",false);
	} else
		found=false;
	//Make Themis:cookieversion a visible attribute
	for (int32 i=0;i<attrcount;i++) {
		BString item;
		attrinf.FindString("attr:name",i,&item);
		if (item=="Themis:cookieversion") {
			found=true;
			break;
		}
	}
	if ((!found) || (installall)) {
		attrinf.AddString("attr:name","Themis:cookieversion");
		attrinf.AddString("attr:public_name","Version");
		attrinf.AddInt32("attr:type",B_INT8_TYPE);
		attrinf.AddInt32("attr:width",50);
		attrinf.AddInt32("attr:alignment",B_ALIGN_CENTER);
		attrinf.AddBool("attr:public",true);
		attrinf.AddBool("attr:editable",true);
		attrinf.AddBool("attr:viewable",true);
		attrinf.AddBool("attr:extra",false);
	} else
		found=false;
	type_code code=B_STRING_TYPE;
	int32 count=0;
	attrinf.GetInfo("type",&code,&count);
	if (code==B_STRING_TYPE) {
		for (int i=1; i<count;i++)
			attrinf.RemoveData("type",1);
	}
//	attrinf.PrintToStream();
	mime.SetAttrInfo(&attrinf);
}
void CookieManager::CheckIndicies() {
  BVolumeRoster *volr=new BVolumeRoster;
  BVolume vol;
  dirent *ent;
  DIR *d;
  char voln[256];
  bool found;
  while (volr->GetNextVolume(&vol)==B_NO_ERROR) {
    if ((vol.KnowsQuery()) && (vol.KnowsAttr()) && (!vol.IsReadOnly())) {
      memset(voln,0,256);
      vol.GetName(voln);
      printf("Checking %s...\n",voln);
      d=fs_open_index_dir(vol.Device());
      if (!d) {
        printf("\tcouldn't open index directory.\n");
        continue;
       }
	  found=false;
      //duplicate and modify the next block as necessary to add more indices
      //begin block
      //Themis:creceivedate 
      printf("\t\tlooking for \"Themis:creceivedate\" index...");
      fflush(stdout);
      while((ent=fs_read_index_dir(d)))
       {
        if (strcasecmp(ent->d_name,"Themis:creceivedate")==0)
         {
          printf("found it.\n");
          found=true;
          break;
         }
       }
      if (!found)
       {
        printf("created it.\n");
        fs_create_index(vol.Device(),"Themis:creceivedate",B_INT32_TYPE,0);
       }
      fs_rewind_index_dir(d);
      found=false;
      //end block
      //begin block
      //Themis:domain
      printf("\t\tlooking for \"Themis:domain\" index...");
      fflush(stdout);
      while((ent=fs_read_index_dir(d)))
       {
        if (strcasecmp(ent->d_name,"Themis:domain")==0)
         {
          printf("found it.\n");
          found=true;
          break;
         }
       }
      if (!found)
       {
        printf("created it.\n");
        fs_create_index(vol.Device(),"Themis:domain",B_STRING_TYPE,0);
       }
      fs_rewind_index_dir(d);
      found=false;
      //end block
      //begin block
      //Themis:cookiepath
      printf("\t\tlooking for \"Themis:cookiepath\" index...");
      fflush(stdout);
      while((ent=fs_read_index_dir(d)))
       {
        if (strcasecmp(ent->d_name,"Themis:cookiepath")==0)
         {
          printf("found it.\n");
          found=true;
          break;
         }
       }
      if (!found)
       {
        printf("created it.\n");
        fs_create_index(vol.Device(),"Themis:cookiepath",B_STRING_TYPE,0);
       }
      fs_rewind_index_dir(d);
      found=false;
      //end block
      //begin block
      //Themis:cookiename
      printf("\t\tlooking for \"Themis:cookiename\" index...");
      fflush(stdout);
      while((ent=fs_read_index_dir(d)))
       {
        if (strcasecmp(ent->d_name,"Themis:cookiename")==0)
         {
          printf("found it.\n");
          found=true;
          break;
         }
       }
      if (!found)
       {
        printf("created it.\n");
        fs_create_index(vol.Device(),"Themis:cookiename",B_STRING_TYPE,0);
       }
      fs_rewind_index_dir(d);
      found=false;
      //end block
      //begin block
      //Themis:cookievalue
      printf("\t\tlooking for \"Themis:cookievalue\" index...");
      fflush(stdout);
      while((ent=fs_read_index_dir(d)))
       {
        if (strcasecmp(ent->d_name,"Themis:cookievalue")==0)
         {
          printf("found it.\n");
          found=true;
          break;
         }
       }
      if (!found)
       {
        printf("created it.\n");
        fs_create_index(vol.Device(),"Themis:cookievalue",B_STRING_TYPE,0);
       }
      fs_rewind_index_dir(d);
      found=false;
      //end block
      //begin block
      //Themis:expiredate
      printf("\t\tlooking for \"Themis:expiredate\" index...");
      fflush(stdout);
      while((ent=fs_read_index_dir(d)))
       {
        if (strcasecmp(ent->d_name,"Themis:expiredate")==0)
         {
          printf("found it.\n");
          found=true;
          break;
         }
       }
      if (!found)
       {
        printf("created it.\n");
        fs_create_index(vol.Device(),"Themis:expiredate",B_INT32_TYPE,0);
       }
      fs_rewind_index_dir(d);
      found=false;
      //end block
      //begin block
      //Themis:cookiesecure
      printf("\t\tlooking for \"Themis:cookiesecure\" index...");
      fflush(stdout);
      while((ent=fs_read_index_dir(d)))
       {
        if (strcasecmp(ent->d_name,"Themis:cookiesecure")==0)
         {
          printf("found it.\n");
          found=true;
          break;
         }
       }
      if (!found)
       {
        printf("created it.\n");
        fs_create_index(vol.Device(),"Themis:cookiesecure",B_BOOL_TYPE,0);
       }
      fs_rewind_index_dir(d);
      found=false;
      //end block
/*
No index is created for Themis:cookieports, Themis:cookiecomment, or Themis:cookiecommenturl,
as these are primarily informational and rarely used items in the cookie spec. If any of these
items were to be utilized, it would probably be Themis:cookieports; which is a list of server
port numbers that the cookie should be valid on. It's not something that needs to be searched.
*/
	 }
   }
   
}
void CookieManager::FindCookieDirectory() {
	BPath path;
	if (!CookieSettings->IsEmpty()) {
		if ((*HTTP->AppSettings_p)==HTTP->AppSettings) {
			FindCookieDirStartPoint:
			BString temp;
			if (CookieSettings->FindString("cookie_storage_directory",&temp)!=B_OK) 
				if (CookieSettings->FindRef("cookie_ref",&cookiedirref)==B_OK) {
					path.SetTo(&cookiedirref);
					temp=path.Path();
				} else {
					HTTP->AppSettings->FindString(kPrefsSettingsFilePath,&temp);
					temp<<"/cookies";
				}
			BEntry ent;
			FindCookieDirPoint1:
			ent.SetTo(temp.String());
			if (ent.Exists()) {
				if (ent.IsDirectory()) {
					ent.GetRef(&cookiedirref);
					if (!CookieSettings->HasRef("cookie_ref"))
						CookieSettings->AddRef("cookie_ref",&cookiedirref);
				} else {
					temp<<time(NULL);
					goto FindCookieDirPoint1;
				}
			} else {
			create_directory(temp.String(),0700);
			ent.GetRef(&cookiedirref);
			if (!CookieSettings->HasRef("cookie_ref"))
				CookieSettings->AddRef("cookie_ref",&cookiedirref);
			}
		} else {
			HTTP->AppSettings=*HTTP->AppSettings_p;
			goto FindCookieDirStartPoint;
		}
	} else {
		BString temp="/boot/home/config/settings/Themis/cookies";
		BEntry ent;
		FindCookieDirPoint2:
		ent.SetTo(temp.String());
		if (ent.Exists()) {
			if (ent.IsDirectory()) {
				ent.GetRef(&cookiedirref);
				if (!CookieSettings->HasRef("cookie_ref"))
					CookieSettings->AddRef("cookie_ref",&cookiedirref);
			} else {
				temp<<time(NULL);
				goto FindCookieDirPoint2;
			}
		} else {
			create_directory(temp.String(),0700);
			ent.GetRef(&cookiedirref);
			if (!CookieSettings->HasRef("cookie_ref"))
				CookieSettings->AddRef("cookie_ref",&cookiedirref);
		}
	}
		path.SetTo(&cookiedirref);
	if (!CookieSettings->HasString("cookie_storage_directory")) {
		CookieSettings->AddString("cookie_storage_directory",path.Path());
	} else
		CookieSettings->ReplaceString("cookie_storage_directory",path.Path());
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
int32 CookieManager::SetCookie(const char *header, const char *request_host, const char *request_uri,uint16 port,bool secure)
{
//	printf("CookieManager::SetCookie() header info: %s\n",header);
	/*
		Determine how many cookies we might be receiving in this header. The default
		is 1 as that seems to be the common and simplest case, however RFC 2109 specifies
		that multiple may be sent separated by commas. Bastards. RFC 2965 doesn't really
		state whether or not there can be multiple cookies in a Set-Cookie2 header...
	*/
	const char *known_attributes[]=	{
										"Comment",
										"CommentURL",
										"Domain",
										"Max-Age",
										"Path",
										"Version",
										"Expires",
										"Port",
										"Secure",//secure and discard don't have values
										"Discard",
										NULL
									};
	int32 header_length=strlen(header);
	char *header_copy=new char[header_length+1];
	memset(header_copy,0,header_length+1);
	strcpy(header_copy,header);
	int32 cookie_count=1;
	int32 comma_count=0;
	char *comma=NULL;
	char *equal=NULL;
	char *temp=NULL,*temp2=NULL;
	int32 distance=0;
	int32 counter=0;
	bool found=false;
	comma=strchr(header_copy,',');
//	printf("comma: %s\n",comma);
	while (comma!=NULL)
	{
//		printf("searching for cookies...\n");
		comma_count++;
		if (comma!=NULL)
		{
			//Check to see if there is a attribute-value pair after this comma
			equal=strchr(comma,'=');
			if (equal!=NULL)
			{
				//Ok, there is at least one attribute-value pair after this comma.
				//Let's copy the data and do some analyzing.
				distance=equal-(comma+1);
				temp=new char[distance+1];
				memset(temp,0,distance+1);
				strncpy(temp,comma+1,distance);
				temp2=trim(temp);//strip away extra whitespace
				delete temp;
				temp=temp2;//I prefer working with the original temp variable
				temp2=NULL;
				//we should now have the attribute part of the attribute-value pair.
				//either that or we have everything after the day of the week of an
				//expiration date expression plus semicolon, and the attribute part of
				//an attribute-value pair.
//				printf("temp: %s\n",temp);
				found=false;
				//Make sure we don't have part of the date stuck in there...
					int32 length=strlen(temp);
					found=false;
					for (int i=0; i<length; i++)
						if (isspace(temp[i]))
						{
							found=true;	
							break;
						}
					if (found)
					{
						int32 real_length=0;
						//we do have white space in the string, probably part of the date.
						//lets start at the end of the string, and work backwards...
						for (int32 i=length; i>=0; i--)
						{
							if (isspace(temp[i]))
							{
								//we found the first white space character, stop moving backwards.
								real_length=length-i;
								temp2=new char[real_length+1];
								memset(temp2,0,real_length+1);
								strncpy(temp2,temp+i+1,real_length);
								memset(temp,0,length);
								delete temp;
								temp=temp2;
								temp2=NULL;
//								printf("New version of temp: %s\n",temp);
								break;
							}
						}
					}
				while (known_attributes[counter]!=NULL)
				{
					if (strcasecmp(temp,known_attributes[counter])==0)
					{
//						printf("This is a known attribute.\n");
						found=true;
						break;
					}
					counter++;
				}
				if (!found)
				{
					//if there are spaces in the string, then we likely have the date
					//and part of an attribute string...
//					printf("This is an excellent cookie attribute candidate: %s\n",temp);
					cookie_count++;
					
				}
				counter=0;
				delete temp;
				temp=NULL;
			}
		}
		comma=strchr(comma+1,',');
	}
//	printf("estimated cookie count: %ld\n",cookie_count);
	/*
		Rinse and repeat: make it count this time.
	*/
	char **cookie_array=new char*[cookie_count];
	cookie_array[0]=header_copy;
	if (cookie_count>1)
	{
	comma=strchr(header_copy,',');
//	printf("comma: %s\n",comma);
	char *start=NULL;
	int32 offset=0;
	int32 cookie_count2=1;
	while (comma!=NULL)
	{
		comma_count++;
		if (comma!=NULL)
		{
			//Check to see if there is a attribute-value pair after this comma
			equal=strchr(comma,'=');
			if (equal!=NULL)
			{
				//Ok, there is at least one attribute-value pair after this comma.
				//Let's copy the data and do some analyzing.
				offset=equal-header_copy+1;
				distance=equal-(comma+1);
				temp=new char[distance+1];
				memset(temp,0,distance+1);
				strncpy(temp,comma+1,distance);
				temp2=trim(temp);//strip away extra whitespace
				delete temp;
				temp=temp2;//I prefer working with the original temp variable
				temp2=NULL;
				//we should now have the attribute part of the attribute-value pair.
				//either that or we have everything after the day of the week of an
				//expiration date expression plus semicolon, and the attribute part of
				//an attribute-value pair.
//				printf("temp: %s\n",temp);
				found=false;
				//Make sure we don't have part of the date stuck in there...
				//If it's the date, there will be a semicolon between the comma and equal sign.
					int32 length=strlen(temp);
					found=false;
					if (strchr(temp,';')!=NULL)
						found=true;
//					for (int i=0; i<length; i++)
//						if (temp[i]==';')
//						{
//							found=true;	
//							break;
//						}
					if (found)
					{
						int32 real_length=0;
						//we do have a semicolon in the string, we probably have part of the date.
						//lets start at the end of the string, and work backwards...
						for (int32 i=length; i>=0; i--)
						{
							offset--;
							if (isspace(temp[i]))
							{
								//we found the first white space character, stop moving backwards.
								offset++;
								real_length=length-i;
								temp2=new char[real_length+1];
								memset(temp2,0,real_length+1);
								strncpy(temp2,temp+i+1,real_length);
								memset(temp,0,length);
								delete temp;
								temp=temp2;
								temp2=NULL;
//								printf("New version of temp: %s\n",temp);
//								printf("header copy offset: %ld - %s\n",offset,header_copy+offset);
								break;
							}
						}
					} else
					{
						for (int32 i=length; i>=0; i--)
						{
							offset--;
							if (isspace(temp[i]))
							{
								offset++;
//								printf("header copy offset: %ld - %s\n",offset,header_copy+offset);
								break;
							}
						}
					}
				while (known_attributes[counter]!=NULL)
				{
					if (strcasecmp(temp,known_attributes[counter])==0)
					{
//						printf("This is a known attribute.\n");
						found=true;
						break;
					}
					counter++;
				}
				if (!found)
				{
					//if there are spaces in the string, then we likely have the date
					//and part of an attribute string...
//					printf("This is an excellent cookie attribute candidate: %s\n",temp);
//								start=
//					printf("header copy offset: %ld - %s\n",offset,header_copy+offset);
					cookie_array[cookie_count2]=header_copy+offset;
					*(header_copy+offset-1)=(char)NULL;
					cookie_count2++;
					*comma=(char)NULL;
				}
				counter=0;
				delete temp;
				temp=NULL;
			}
		}
		comma=strchr(comma+1,',');
	}
	}
//	for (int32 i=0; i<cookie_count; i++)
//		printf("cookie %d:\t%s\n",i+1,cookie_array[i]);
	/*
		Now lets start eating those cookies...
	*/
	int32 semicolons=0,segments=1;;
	char *semicolon=NULL;
	char **attributes=NULL;
	cookie_st *current_cookie=cookie_head,*new_cookie=NULL,*last_cookie=cookie_head;
	int32 currenttime=0;
	while(last_cookie!=NULL)
	{
		if (last_cookie->next==NULL)
			break;
		last_cookie=last_cookie->next;
	}
	for (int32 i=0; i<cookie_count; i++)
	{
		/*
			Count the attributes
		*/
		semicolons=0;
		segments=1;
		semicolon=strchr(cookie_array[i],';');
		while (semicolon!=NULL)
		{
			semicolons++;
			semicolon=strchr(semicolon+1,';');
		}
		segments=semicolons+1;
		attributes=new char *[segments];
		if (semicolons>0)
		{
			attributes[0]=trim(strtok(cookie_array[i],";"));
			for (int32 j=1; j<segments; j++)
				attributes[j]=trim(strtok(NULL,";"));
		}
		new_cookie=new cookie_st;
		currenttime=real_time_clock();
		new_cookie->datereceived=mktime(localtime(&currenttime));
		bool saved=false;
		for (int32 j=0; j<segments; j++)
		{
			saved=false;
//			printf("segments: %s\n",attributes[j]);
			if (attributes[j]!=NULL)
			{
				equal=strchr(attributes[j],'=');
				if (equal==NULL)
				{
					if (strcasecmp("secure",attributes[j])==0)
						new_cookie->secure=false;
					if (strcasecmp("discard",attributes[j])==0)
						new_cookie->discard=true;
				} else
				{
					int32 alen=equal-attributes[j];
					int32 vlen=strlen(attributes[j])-(alen+1);
					if (strncasecmp("expires",attributes[j],min_c(alen,7))==0)
					{
						if (vlen>0)
						{
							temp=new char[vlen+1];
							memset(temp,0,vlen+1);
							strncpy(temp,equal+1,vlen);
							Date date(temp);
							memset(temp,0,vlen+1);
							delete temp;
							temp=NULL;
							new_cookie->expiredate=date.Timestamp();
//							new_cookie->expiredate=mktime(localtime(&new_cookie->expiredate));
							new_cookie->maxage=new_cookie->expiredate-currenttime;
							new_cookie->discard=false;
						}
							continue;
					}
					if (strncasecmp("max-age",attributes[j],min_c(alen,7))==0)
					{
						if (vlen>0)
						{
							temp=new char[vlen+1];
							memset(temp,0,vlen+1);
							strncpy(temp,equal+1,vlen);
							new_cookie->maxage=atol(temp);
							memset(temp,0,vlen+1);
							delete temp;
							temp=NULL;
							new_cookie->expiredate=currenttime+new_cookie->maxage;
							new_cookie->discard=false;
						}
							continue;
					}
					if (strncasecmp("version",attributes[j],min_c(alen,7))==0)
					{
						if (vlen>0)
						{
							temp=new char[vlen+1];
							memset(temp,0,vlen+1);
							strncpy(temp,equal+1,vlen);
							new_cookie->version=atoi(temp);
							memset(temp,0,vlen+1);
							delete temp;
							temp=NULL;
						}
							continue;
					}
					if (strncasecmp("domain",attributes[j],min_c(alen,6))==0)
					{
						if (vlen>0)
						{
							new_cookie->domain=new char[vlen+1];
							memset(new_cookie->domain,0,vlen+1);
							strncpy(new_cookie->domain,equal+1,vlen);
						} else
						{
							new_cookie->domain=new char[strlen(request_host)+1];
							memset(new_cookie->domain,0,strlen(request_host)+1);
							strcpy(new_cookie->domain,request_host);
						}
							continue;
					}
					if (strncasecmp("path",attributes[j],min_c(alen,4))==0)
					{
/*
	This section should eventually be migrated to utilize the URI processor with code
	similar to:
						URIProcessor uri(equal+1);
						if ((uri.String())[uri.Length()-1]!='/')
							uri.GetParent(&uri);
						new_cookie->path=new char[uri.Length()+1];
						memset(new_cookie->path,0,uri.Length()+1);
						strcpy(new_cookie->path,uri.String());
	The catch is that cookie paths are not supposed to end in a '/' unless the path is
	the root path. There is another instance with similar needs below for path, in case
	the Set-cookie processing exits with a NULL value for path.
*/
						if (vlen>0)
						{
							new_cookie->path=new char[vlen+1];
							memset(new_cookie->path,0,vlen+1);
							strncpy(new_cookie->path,equal+1,vlen);
						} else
						{
							if (strlen(request_uri)>1)
							{
								char *last_slash=strrchr(request_uri,'/');
								int32 len=(last_slash-request_uri);
								new_cookie->path=new char[len+1];
								memset(new_cookie->path,0,len+1);
								strncpy(new_cookie->path,request_uri,len);
							} else
							{
								new_cookie->path=new char[2];
								strcpy(new_cookie->path,"/\0");
							}
						}
							continue;
					}
					if (strncasecmp("comment",attributes[j],min_c(alen,7))==0)
					{
						if (vlen>0)
						{
							new_cookie->comment=new char[vlen+1];
							memset(new_cookie->comment,0,vlen+1);
							strncpy(new_cookie->comment,equal+1,vlen);
						}
							continue;
					}
					if (strncasecmp("commenturl",attributes[j],min_c(alen,10))==0)
					{
						if (vlen>0)
						{
							new_cookie->commenturl=new char[vlen+1];
							memset(new_cookie->commenturl,0,vlen+1);
							strncpy(new_cookie->commenturl,equal+1,vlen);
						}
							continue;
					}
					if (strncasecmp("port",attributes[j],min_c(alen,4))==0)
					{
						if (vlen>0)
						{
							new_cookie->ports=new char[vlen+1];
							memset(new_cookie->ports,0,vlen+1);
							strncpy(new_cookie->ports,equal+1,vlen);
						} else
						{
							new_cookie->ports=new char[8];
							memset(new_cookie->ports,0,8);
							sprintf(new_cookie->ports,"\"%u\"",port);
						}
							continue;
					}
					/*
						If it's none of the above attributes, then it has to be the
						cookie name as set by the URL.
					*/
					new_cookie->name=new char[alen+1];
					new_cookie->value=new char[vlen+1];
					memset(new_cookie->name,0,alen+1);
					memset(new_cookie->value,0,vlen+1);
					strncpy(new_cookie->name,attributes[j],alen);
					strncpy(new_cookie->value,equal+1,vlen);
				}
				memset(attributes[j],0,strlen(attributes[j])+1);
				delete attributes[j];
				attributes[j]=NULL;
			}
		}
		if (new_cookie->domain==NULL)
		{
							new_cookie->domain=new char[strlen(request_host)+1];
							memset(new_cookie->domain,0,strlen(request_host)+1);
							strcpy(new_cookie->domain,request_host);
		}
		if (new_cookie->path==NULL)
		{
							if (strlen(request_uri)>1)
							{
								char *last_slash=strrchr(request_uri,'/');
								int32 len=(last_slash-request_uri);
								new_cookie->path=new char[len+1];
								memset(new_cookie->path,0,len+1);
								strncpy(new_cookie->path,request_uri,len);
							} else
							{
								new_cookie->path=new char[2];
								strcpy(new_cookie->path,"/\0");
							}
		}
		if (new_cookie->expiredate==0)
			new_cookie->expiredate=currenttime+new_cookie->maxage;
		/*
			The DuplicatedCookie check was moved to before the validation check
			to make sure that any attempt to expire a cookie (by updating the expiration
			date to an already past date/time) will succeed. The original will then die
			off on its own. Otherwise, the original would linger until its original
			expiration date/time passed.
		*/
		if (DuplicatedCookie(new_cookie))
		{
			cookie_st *original=FindCookie(new_cookie->name,new_cookie->path,new_cookie->domain);
			original->expiredate=new_cookie->expiredate;
			original->discard=new_cookie->discard;
			original->secure=new_cookie->secure;
			original->version=new_cookie->version;
			original->maxage=new_cookie->maxage;
			if (!original->discard)
			{
				SaveCookie(original);
				saved=true;
			}
//			printf("Original Cookie Updated\n");
//			PrintCookie(original);
			original->maxage=-1;//trigger a validation failure to delete the new copy.
		}
		if (!Validate(new_cookie,request_host,request_uri))
		{
			delete new_cookie;
			continue;
		}
		if (!saved)
		{
			SaveCookie(new_cookie);
			saved=true;
		}
		delete []attributes;
		if (cookie_head==NULL)
		{
			cookie_head=new_cookie;
			last_cookie=cookie_head;
			
		} else
		{
			last_cookie->next=new_cookie;
			last_cookie=last_cookie->next;
		}
	}
	delete []cookie_array;
	cookie_array=NULL;
	memset(header_copy,0,header_length+1);
	delete header_copy;
	header_copy=NULL;
}
cookie_st *CookieManager::FindCookie(const char *name,const char *path,const char *domain)
{
	cookie_st *current=cookie_head;
	while (current!=NULL)
	{
		if (
				strcmp(name,current->name)==0 &&
				strcmp(path,current->path)==0 &&
				strcasecmp(domain,current->domain)==0
			)
		{
			break;
		}
		current=current->next;
	}
	return current;
}
bool CookieManager::DuplicatedCookie(cookie_st *cookie)
{
	bool found=false;
	cookie_st *current=cookie_head;
	if (cookie==NULL)
		return false;
	while (current!=NULL)
	{
		if (
				strcmp(cookie->name,current->name)==0 &&
				strcmp(cookie->path,current->path)==0 &&
				strcasecmp(cookie->domain,current->domain)==0
			)
		{
			found=true;
			break;
		}
		current=current->next;
	}
	return found;
}
const char *CookieManager::GetCookie(const char *host, const char *path,uint16 port, bool secure) {
	BAutolock alock(lock);
	if (alock.IsLocked()) {
//		printf("GetCookie()\n");
		int32 count=0;
		int32 rfc2109_count=0, rfc2965_count=0;
		cookie_st **cookies=FindCookies(&count,host,path,secure);
		if ((cookies!=NULL) && (count>0)) {
			const char *cookiestr=NULL;
			BString rfc2109str("Cookie: "),rfc2965str("Cookie2: ");
			for (int32 i=0; i<count; i++)
			{
				if (cookies[i]->version==0)
				{
					if (rfc2109_count>0)
						rfc2109str<<";";
					rfc2109str<<cookies[i]->name<<"="<<cookies[i]->value;
					rfc2109_count++;
				} else
				{
					if (cookies[i]->version==1)
					{
						bool port_matches=false;
						if (cookies[i]->ports!=NULL)
						{
							//make sure we have a port match before moving forward.
							int32 port_count=0;
							char **ports=NULL;
							char *ports_copy=new char[strlen(cookies[i]->ports)+1];
							memset(ports_copy,0,strlen(cookies[i]->ports)+1);
							strcpy(ports_copy,cookies[i]->ports);
							if (strchr(ports_copy,'"')!=NULL)
							{
								char *quote;
								for (int32 j=0; j<strlen(ports_copy); j++)
								{
									quote=strchr(ports_copy,'"');
									*quote=0x20;//convert quotes to spaces.
								}	
							}
							if (strchr(ports_copy,',')!=NULL)
							{
								char *comma=strchr(ports_copy,',');
								while (comma!=NULL)
								{
									port_count++;
									comma=strchr(comma+1,',');
								}
								port_count+=1;
								ports=new char *[port_count];
								ports[0]=strtok(ports_copy,",");
								for (int32 j=1; j<port_count; j++)
									ports[j]=strtok(NULL,",");
							} else
							{
								port_count=1;
								ports=new char *[1];
								ports[0]=ports_copy;
							}
							for (int32 j=0; j<port_count; j++)
							{
								if (atol(ports[j])==port)
								{
									port_matches=true;
									break;
								}
							}
							memset(ports_copy,0,strlen(cookies[i]->ports)+1);
							delete ports_copy;
							ports_copy=NULL;
							delete []ports;
						} else
							port_matches=true;
						if (port_matches)
						{
							if (rfc2965_count>0)
								rfc2965str<<";";
							rfc2965str<<cookies[i]->name<<"="<<cookies[i]->value;
								rfc2965_count++;
						}
					}
				}
			}
			delete []cookies;
			BString cstr;
			if (rfc2109_count>0)
				cstr<<rfc2109str<<"\r\n";
			if (rfc2965_count>0)
				cstr<<rfc2965str<<"\r\n";
			cookiestr=new char[cstr.Length()+1];
			memset((char*)cookiestr,0,cstr.Length()+1);
			strcpy((char*)cookiestr,cstr.String());
			return cookiestr;
		}
	}
	return NULL;
}

void CookieManager::ClearAllCookies() {
	BAutolock alock(lock);
	if (alock.IsLocked()) {
//		printf("ClearAllCookies()\n");
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
//		printf("Clearing expired cookies...\n");
		time_t currenttime=time(NULL);
		currenttime=mktime(localtime(&currenttime));
		cookie_st *cur=cookie_head, *last=NULL;
		while (cur!=NULL) {
//			printf("Expire %s: current time: %ld\texpire time: %ld\tdifference: %2.2f\n",cur->name,currenttime,cur->expiredate,difftime(cur->expiredate,currenttime));
			if (cur->expiredate<=currenttime) {
				printf("The following cookie has expired:\n");
				PrintCookie(cur);
				if (cur==cookie_head) {
					cookie_head=cookie_head->next;
					BEntry ent(&cur->ref);
					if (ent.Exists())
						ent.Remove();
					delete cur;
					last=cur=cookie_head;
					continue;
				} else {
					last->next=cur->next;
					BEntry ent(&cur->ref);
					if (ent.Exists())
						ent.Remove();
					delete cur;
					cur=last->next;
					continue;
				}
			} else {
				if (cur->discard) {
					BEntry ent(&cur->ref);
					if (ent.Exists()) {
						ent.Remove();
					}
					ent.Unset();
					
				}
				
			}
			
			last=cur;
			cur=cur->next;
		}
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
		printf("\tVersion:\t%d\n",cookie->version);
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
		currenttime=mktime(localtime(&currenttime));
//		printf("expiredate: %ld\ncurrent time: %ld\n",cookie->expiredate, currenttime);
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
with Lou Montulli (one of the authors of the RFC 2109 and 2965), I came under the impression
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
//			printf("path: %s\n",cookie->path);
			if (strncmp(cookie->path,request_uri,strlen(cookie->path))!=0)
				return ValidateFail("Cookie path is not a prefix of the request URI. (4.3.2:1)");
		}
	}
	return valid;
}
status_t CookieManager::LoadCookie(cookie_st *cookie) {
	if (cookie==NULL)
		return B_ERROR;
		struct attr_info ai;
		char attname[B_ATTR_NAME_LENGTH+1];
		memset(attname,0,B_ATTR_NAME_LENGTH+1);
		BNode node(&cookie->ref);
		cookie->discard=false;
		unsigned char *data=NULL;
		BString aname;
		node.Lock();
		while (node.GetNextAttrName(attname)==B_OK) {
			node.GetAttrInfo(attname,&ai);
			aname.Truncate(0);
			switch(ai.type) {
				case B_INT32_TYPE:{
					if (strcasecmp(attname,"Themis:creceivedate")==0) {
						node.ReadAttr("Themis:creceivedate",B_INT32_TYPE,0,&cookie->datereceived,ai.size);
					}
					if (strcasecmp(attname,"Themis:expiredate")==0) {
						node.ReadAttr("Themis:expiredate",B_INT32_TYPE,0,&cookie->expiredate,ai.size);
					}
					
				}break;
				case B_INT8_TYPE:
				{
					if (strcasecmp(attname,"Themis:cookieversion")==0)
					{
						node.ReadAttr("Themis:cookieversion",B_INT8_TYPE,0,&cookie->version,ai.size);
					}
				}break;
				case B_STRING_TYPE:{
					if (strcasecmp(attname,"Themis:domain")==0) {
						cookie->domain=new char[ai.size+1];
						memset(cookie->domain,0,ai.size+1);
						node.ReadAttr("Themis:domain",B_STRING_TYPE,0,cookie->domain,ai.size);
					}
					if (strcasecmp(attname,"Themis:cookiepath")==0) {
						cookie->path=new char[ai.size+1];
						memset(cookie->path,0,ai.size+1);
						node.ReadAttr("Themis:cookiepath",B_STRING_TYPE,0,cookie->path,ai.size);
					}
					if (strcasecmp(attname,"Themis:cookiename")==0) {
						cookie->name=new char[ai.size+1];
						memset(cookie->name,0,ai.size+1);
						node.ReadAttr("Themis:cookiename",B_STRING_TYPE,0,cookie->name,ai.size);
					}
					if (strcasecmp(attname,"Themis:cookievalue")==0) {
						cookie->value=new char[ai.size+1];
						memset(cookie->value,0,ai.size+1);
						node.ReadAttr("Themis:cookievalue",B_STRING_TYPE,0,cookie->value,ai.size);
					}
					if (strcasecmp(attname,"Themis:cookieports")==0) {
						cookie->ports=new char[ai.size+1];
						memset(cookie->ports,0,ai.size+1);
						node.ReadAttr("Themis:cookieports",B_STRING_TYPE,0,cookie->ports,ai.size);
					}
					if (strcasecmp(attname,"Themis:cookiecomment")==0) {
						cookie->comment=new char[ai.size+1];
						memset(cookie->comment,0,ai.size+1);
						node.ReadAttr("Themis:cookiecomment",B_STRING_TYPE,0,cookie->comment,ai.size);
					}
					if (strcasecmp(attname,"Themis:cookiecommenturl")==0) {
						cookie->commenturl=new char[ai.size+1];
						memset(cookie->commenturl,0,ai.size+1);
						node.ReadAttr("Themis:cookiecommenturl",B_STRING_TYPE,0,cookie->commenturl,ai.size);
					}
				}break;
	
				case B_BOOL_TYPE:{
					if (strcasecmp(attname,"Themis:cookiesecure")==0)
						node.ReadAttr("Themis:cookiesecure",B_BOOL_TYPE,0,&cookie->secure,ai.size);
				}break;
			}
			delete data;
			data=NULL;
			memset(attname,0,B_ATTR_NAME_LENGTH+1);
		}
		node.Unlock();
		if ((cookie->datereceived!=0) && (cookie->expiredate!=0)) {
			cookie->maxage=cookie->expiredate-cookie->datereceived;
		}
		
	return B_OK;
}

status_t CookieManager::SaveCookie(cookie_st *cookie) {
	if (cookie==NULL)
		return B_ERROR;
	if (cookie->discard)
		return B_NO_ERROR;
	BEntry ent(&cookie->ref,true);
	if (ent.InitCheck()==B_OK) {
		SaveCookiePoint1:
		BFile file(&ent,B_CREATE_FILE|B_ERASE_FILE|B_WRITE_ONLY);
		file.Unset();
		BNode node(&cookie->ref);
		node.Lock();
		char *type=new char[strlen(ThemisCookieFile)+1];
		memset(type,0,strlen(ThemisCookieFile)+1);
		strcpy(type,ThemisCookieFile);
		node.WriteAttr("BEOS:TYPE",B_STRING_TYPE,0,type,strlen(ThemisCookieFile)+1);
		delete type;
		node.WriteAttr("Themis:cookiesecure",B_BOOL_TYPE,0,&cookie->secure,sizeof(cookie->secure));
		node.WriteAttr("Themis:creceivedate",B_INT32_TYPE,0,&cookie->datereceived,sizeof(cookie->datereceived));
		node.WriteAttr("Themis:expiredate",B_INT32_TYPE,0,&cookie->expiredate,sizeof(cookie->expiredate));
		if (cookie->domain!=NULL)
			node.WriteAttr("Themis:domain",B_STRING_TYPE,0,cookie->domain,strlen(cookie->domain)+1);
		if (cookie->path!=NULL)
			node.WriteAttr("Themis:cookiepath",B_STRING_TYPE,0,cookie->path,strlen(cookie->path)+1);
		if (cookie->name!=NULL)
			node.WriteAttr("Themis:cookiename",B_STRING_TYPE,0,cookie->name,strlen(cookie->name)+1);
		if (cookie->value!=NULL)
			node.WriteAttr("Themis:cookievalue",B_STRING_TYPE,0,cookie->value,strlen(cookie->value)+1);
		if (cookie->version==1)
			if (cookie->ports!=NULL)
				node.WriteAttr("Themis:cookieports",B_STRING_TYPE,0,cookie->ports,strlen(cookie->ports)+1);
		if (cookie->comment!=NULL)
			node.WriteAttr("Themis:cookiecomment",B_STRING_TYPE,0,cookie->comment,strlen(cookie->comment)+1);
		if (cookie->commenturl!=NULL)
			node.WriteAttr("Themis:cookiecommenturl",B_STRING_TYPE,0,cookie->commenturl,strlen(cookie->commenturl)+1);
		node.WriteAttr("Themis:cookieversion",B_INT8_TYPE,0,&cookie->version,sizeof(cookie->version));
		node.Unlock();
	} else {
		BString fname;
		fname<<cookie->domain<<cookie->path<<cookie->name;
		BPath path(&cookiedirref);
		fname.ReplaceAll('/','_');
		path.Append(fname.String());
		ent.SetTo(path.Path());
		ent.GetRef(&cookie->ref);
		goto SaveCookiePoint1;
		
	}
	
	return B_OK;
}

void CookieManager::LoadAllCookies() {
	BAutolock alock(lock);
	if (alock.IsLocked()) {
	BPath trashpath;
	BDirectory *trashdir;
	if (find_directory(B_TRASH_DIRECTORY,&trashpath)==B_OK) {
		trashdir=new BDirectory(trashpath.Path());
	} else {
		trashdir=new BDirectory("/boot/home/Desktop/Trash");
	}
		BEntry ent(&cookiedirref,true);
		struct stat devstat;
		ent.GetStat(&devstat);
		BVolume vol(devstat.st_dev);
		BQuery query;
		query.SetVolume(&vol);
		query.PushAttr("BEOS:TYPE");
		query.PushString(ThemisCookieFile);
		query.PushOp(B_EQ);
		query.Fetch();
		ent.Unset();
		BPath path;
		entry_ref ref;
	cookie_st *curcookie=cookie_head;
	cookie_st *nucookie=NULL;
	int32 count=0;
	while (query.GetNextEntry(&ent,false)==B_OK) {
		if (trashdir->Contains(&ent))
			continue;
		ent.GetRef(&ref);
		path.SetTo(&ref);
		printf("cookie file at: %s\n",path.Path());
		nucookie=new cookie_st;
		nucookie->ref=ref;
		if (curcookie==NULL) {
			if (cookie_head==NULL) {
				curcookie=cookie_head=nucookie;
			} else {
				curcookie=cookie_head;
				while (curcookie->next!=NULL)
					curcookie=curcookie->next;
				curcookie->next=nucookie;
			}
			
			
		} else {
			while (curcookie->next!=NULL)
				curcookie=curcookie->next;
			curcookie->next=nucookie;
		}
		LoadCookie(nucookie);
		
		count++;
	}
	printf("%ld cookie(s) loaded.\n",count);
		delete trashdir;
	}
	PrintCookies();
}

void CookieManager::SaveAllCookies() {
	BAutolock alock(lock);
	if (alock.IsLocked()) {
		cookie_st *cur=cookie_head;
		while (cur!=NULL) {
			if (!cur->discard) {
				SaveCookie(cur);
			}
			cur=cur->next;	
		}
		
	}
}

status_t CookieManager::ReceiveBroadcast(BMessage *msg) 
{
//	printf("CookieManager::ReceiveBroadcast()\n");
	return B_OK;
}
uint32 CookieManager::BroadcastTarget() 
{
//	printf("CookieManager\n");
	return MS_TARGET_COOKIE_MANAGER;
}

