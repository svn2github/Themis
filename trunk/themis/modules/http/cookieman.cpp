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
#include <NodeInfo.h>
#include <Volume.h>
#include <kernel/fs_index.h>
#include <fs_attr.h>
#include <Query.h>
#include "commondefs.h"
#include "prefsman.h"
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
char *attvalue(char *attrib)
{
	char *eq = strchr(attrib,'=');
	if( eq != NULL) eq = (eq+1);
	return eq;
}
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
	prefMan = new PrefsManager("cookie_settings",PrefsManager::FLAG_CREATE_FILE|PrefsManager::FLAG_REPLACE_FILE);
//	printf("CookieManager()\n");
	CookieSettings = prefMan->loadPrefs();
	if( CookieSettings == NULL) CookieSettings=new BMessage();
	if( CookieSettings->HasBool("strict_mode") ) strictMode = CookieSettings->FindBool("strict_mode");
	else
	{
		strictMode = false;
		CookieSettings->AddBool("strict_mode",strictMode);
	}
	cookie_head=NULL;
//	lock=new BLocker("cookie manager lock",true);
//	printf("HTTPv4: %p\n",HTTP);
//	printf("Checking MIME...\n");
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
//	printf("Finding cookie directory...\n");
	FindCookieDirectory();
	LoadAllCookies();
}

CookieManager::~CookieManager() {
//	printf("~CookieManager()\n");
	MsgSysUnregister(this);
	ClearExpiredCookies();
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
	prefMan->savePrefs(CookieSettings);
//	delete lock;
	delete CookieSettings;
//	printf("cookie mime: %s\n",ThemisCookieFile);
//	printf("shutting down at: %ld\n",time(NULL));
	delete prefMan;
}
void CookieManager::CheckMIME() {
	BMimeType mime(ThemisCookieFile);//application/x-Themis-cookie
//	printf("InitCheck: %ld\n",mime.InitCheck());
//	printf("MIME Type valid? %s\n",mime.IsValid()?"yes":"no");
	if (!mime.IsInstalled()) {
		status_t stat=mime.Install();
//		printf("Cookie MIME Type Installed (%s): %ld\n",ThemisCookieFile,stat);
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
	//Make Themis:httponly a visible attribute
	for (int32 i=0;i<attrcount;i++) {
		BString item;
		attrinf.FindString("attr:name",i,&item);
		if (item=="Themis:httponly") {
			found=true;
			break;
		}
	}
	if ((!found) || (installall)) {
		attrinf.AddString("attr:name","Themis:httponly");
		attrinf.AddString("attr:public_name","HTTP Only");
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
//      printf("Checking %s...\n",voln);
      d=fs_open_index_dir(vol.Device());
      if (!d) {
 //       printf("\tcouldn't open index directory.\n");
        continue;
       }
	  found=false;
      //duplicate and modify the next block as necessary to add more indices
      //begin block
      //Themis:creceivedate 
//      printf("\t\tlooking for \"Themis:creceivedate\" index...");
      fflush(stdout);
      while((ent=fs_read_index_dir(d)))
       {
        if (strcasecmp(ent->d_name,"Themis:creceivedate")==0)
         {
 //         printf("found it.\n");
          found=true;
          break;
         }
       }
      if (!found)
       {
 //       printf("created it.\n");
        fs_create_index(vol.Device(),"Themis:creceivedate",B_INT32_TYPE,0);
       }
      fs_rewind_index_dir(d);
      found=false;
      //end block
      //begin block
      //Themis:domain
 //     printf("\t\tlooking for \"Themis:domain\" index...");
      fflush(stdout);
      while((ent=fs_read_index_dir(d)))
       {
        if (strcasecmp(ent->d_name,"Themis:domain")==0)
         {
 //         printf("found it.\n");
          found=true;
          break;
         }
       }
      if (!found)
       {
//        printf("created it.\n");
        fs_create_index(vol.Device(),"Themis:domain",B_STRING_TYPE,0);
       }
      fs_rewind_index_dir(d);
      found=false;
      //end block
      //begin block
      //Themis:cookiepath
//      printf("\t\tlooking for \"Themis:cookiepath\" index...");
      fflush(stdout);
      while((ent=fs_read_index_dir(d)))
       {
        if (strcasecmp(ent->d_name,"Themis:cookiepath")==0)
         {
//          printf("found it.\n");
          found=true;
          break;
         }
       }
      if (!found)
       {
//        printf("created it.\n");
        fs_create_index(vol.Device(),"Themis:cookiepath",B_STRING_TYPE,0);
       }
      fs_rewind_index_dir(d);
      found=false;
      //end block
      //begin block
      //Themis:cookiename
//      printf("\t\tlooking for \"Themis:cookiename\" index...");
      fflush(stdout);
      while((ent=fs_read_index_dir(d)))
       {
        if (strcasecmp(ent->d_name,"Themis:cookiename")==0)
         {
//          printf("found it.\n");
          found=true;
          break;
         }
       }
      if (!found)
       {
//        printf("created it.\n");
        fs_create_index(vol.Device(),"Themis:cookiename",B_STRING_TYPE,0);
       }
      fs_rewind_index_dir(d);
      found=false;
      //end block
      //begin block
      //Themis:cookievalue
//      printf("\t\tlooking for \"Themis:cookievalue\" index...");
      fflush(stdout);
      while((ent=fs_read_index_dir(d)))
       {
        if (strcasecmp(ent->d_name,"Themis:cookievalue")==0)
         {
//          printf("found it.\n");
          found=true;
          break;
         }
       }
      if (!found)
       {
//        printf("created it.\n");
        fs_create_index(vol.Device(),"Themis:cookievalue",B_STRING_TYPE,0);
       }
      fs_rewind_index_dir(d);
      found=false;
      //end block
      //begin block
      //Themis:expiredate
//      printf("\t\tlooking for \"Themis:expiredate\" index...");
      fflush(stdout);
      while((ent=fs_read_index_dir(d)))
       {
        if (strcasecmp(ent->d_name,"Themis:expiredate")==0)
         {
//          printf("found it.\n");
          found=true;
          break;
         }
       }
      if (!found)
       {
//        printf("created it.\n");
        fs_create_index(vol.Device(),"Themis:expiredate",B_INT32_TYPE,0);
       }
      fs_rewind_index_dir(d);
      found=false;
      //end block
      //begin block
      //Themis:cookiesecure
//      printf("\t\tlooking for \"Themis:cookiesecure\" index...");
      fflush(stdout);
      while((ent=fs_read_index_dir(d)))
       {
        if (strcasecmp(ent->d_name,"Themis:cookiesecure")==0)
         {
//          printf("found it.\n");
          found=true;
          break;
         }
       }
      if (!found)
       {
//        printf("created it.\n");
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
					HTTP->AppSettings->FindString(kPrefsSettingsDirectory,&temp);
					temp<<"/cookies";
				}
				printf("Cookie Directory: %s\n",temp.String() );
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
	BAutolock alock(lock);
	if (alock.IsLocked()) {
	//printf("CookieManager::SetCookie() header info: %s\n",header);
	/*
		Determine how many cookies we might be receiving in this header. The default
		is 1 as that seems to be the common and simplest case, however RFC 2109 specifies
		that multiple may be sent separated by commas. Bastards. RFC 2965 doesn't really
		state whether or not there can be multiple cookies in a Set-Cookie2 header...
	*/
	bool reject_cookie = false;
	URIProcessor *URI = new URIProcessor(request_uri), *parentURI = new URIProcessor(), *pathURI = new URIProcessor();
	if( request_uri[strlen(request_uri)-1] == '/') parentURI->Set(request_uri);
	else URI->GetParent(parentURI);
	const char *a = URI->String(), *b = parentURI->String();
	//printf("URI/Parent:\n\t%s\n\t%s\n",a,b);
	//printf("moving on...\n");
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
										"HTTPOnly",
										NULL
									};
	int32 header_length=strlen(header);
	char *header_copy=new char[header_length+1];
	memset(header_copy,0,header_length+1);
	strncpy(header_copy,header,header_length);
	int32 cookie_count=1;
	int32 comma_count=0;
	char *comma=NULL;
	char *equal=NULL;
	char *temp=NULL,*temp2=NULL, *nvpairs = NULL;
	int32 distance=0;
	int32 counter=0;
	int32 current_time = real_time_clock();
	bool abort = false;
	bool found=false;
	char **cookie_array=NULL;
	bool has_attributes = false;
//	comma=strchr(header_copy,',');
//	printf("comma: %s\n",comma);
	// First, lets deal only with the cookie setting portion. If there are any attributes, they will be after a semicolon, so we're going to deal only with the part before any
	// present semicolons.
	char *semicolon = strchr(header_copy,';');
	if( semicolon != NULL)
	{// semicolon found, isolate cookie name-value pairs
		distance = semicolon-header_copy;
		has_attributes = true;
	}// semicolon found, isolate cookie name-value pairs
	else
	{// no semicolon found, no attributes, so just copy the header portion
		distance = header_length;
	}// no semicolon found, no attributes, so just copy the header portion
	//printf("distance (cookie bytes): %d\n",distance);
	nvpairs = new char[distance+1];
	memset(nvpairs,0,distance+1);
	strncpy(nvpairs,header_copy,distance);
	//printf("isolated pair(s): %s\n",nvpairs);
	// Check to see if there are multiple name-value pairs within the nvpairs variable
	comma = strchr(nvpairs,',');
	uint32 nvpair_counter = 0;
	if( comma != NULL)
	{ // we have at least one comma, check out the rest for nv pairs
		nvpair_counter = 1;
		for(int32 i=0; i< (strlen(nvpairs)-1); i++)
		{// kick it old school
			if( nvpairs[i] == ',' && strchr((nvpairs+i),'=') != NULL)
				nvpair_counter++;
		}// kick it old school
		printf("name=value pairs: %d\n",nvpair_counter);
		cookie_array = new char*[nvpair_counter];
		int32 pair_length = 0;
		if( nvpair_counter > 1)
		{
			for(uint32 i=0; i<nvpair_counter; i++)
			{
				if( i == 0)
				{
					temp = strtok(nvpairs,",");
				}
				else
				{
					temp = strtok(NULL,",");
				}
				if( temp != NULL)
				{
					temp2 = trim(temp);
					pair_length = strlen(temp2);
					
					cookie_array[i] = new char[pair_length+1];
					memset(cookie_array[i],0,pair_length+1);
					strncpy(cookie_array[i],temp2,pair_length);
					memset(temp2,0,pair_length+1);
					delete temp2;
				}
			}
		}
		else
		{
			cookie_array = new char*[1];
			cookie_array[0] = new char[distance+1];
			memset(cookie_array[0],0,distance+1);
			strncpy(cookie_array[0],nvpairs,distance);
		}
	} // we have at least one comma, check out the rest for nv pairs
	else
	{// we only have the one name-value pair, so fake it for simplicity's sake
		nvpair_counter = 1;
		cookie_array = new char*[1];
		cookie_array[0] = new char[distance+1];
		memset(cookie_array[0],0,distance+1);
		strncpy(cookie_array[0],nvpairs,distance);
	}// we only have the one name-value pair, so fake it for simplicity's sake
	//for(uint32 i = 0; i < nvpair_counter; i++)
	//	printf("\t\tcookie pair %d: %s\n",(i+1),cookie_array[i]);
	// ok, create entries for each name-value pair.
	cookie_st *default_cookie = new cookie_st;
	int32 tlen = strlen(parentURI->String());
	default_cookie->path = new char[tlen+1];
	memset(default_cookie->path,0,tlen+1);
	strcpy(default_cookie->path,parentURI->String());
	default_cookie->datereceived = current_time;
	tlen = strlen(request_host);
	default_cookie->domain = new char[tlen+1];
	memset(default_cookie->domain,0,tlen+1);
	strncpy(default_cookie->domain,request_host,tlen);
	if( semicolon != NULL)
	{ // process the remaining cookie header info
		int semicolons = 0, segments = 0;
		char **attributes=NULL;
		bool loop_first = true;
		do
		{
			if( loop_first ) 
			{
				temp = strchr(semicolon+1,';');
				loop_first = false;
			}
			else if( temp != NULL)
				temp = strchr(temp+1,';');
			if( temp != NULL) semicolons++;
		} while( temp != NULL);
		segments = semicolons+1;
		printf("segments: %d\n",segments);
		attributes = new char *[segments];
		attributes[0] = trim(strtok(semicolon+1,";"));
		for( int32 i=1; i<segments; i++)
		{
			attributes[i] = trim(strtok(NULL,";"));
		}
		for(int32 i = 0; i< segments; i++)
		{
			//printf("\t\tcookie attributes: %s\n",attributes[i]);
			for(int8 kac = 0; kac < 10; kac++)
			{
				if(strncasecmp(attributes[i],known_attributes[kac],min_c(strlen(known_attributes[kac]),strlen(attributes[i]))) == 0)
				{
					//printf("\t\t\tattrib: %s\n",known_attributes[kac]);
					if( strcasecmp(known_attributes[kac],"path") == 0 ) {
						pathURI->Set(attvalue(attributes[i]));
					if( default_cookie->path != NULL)
					{
						memset(default_cookie->path,0,strlen(default_cookie->path)+1);
						delete default_cookie->path;
						default_cookie->path = NULL;
					}
						if( strictMode)
						{
								default_cookie->path = new char[pathURI->Length()+1];
								memset(default_cookie->path,0,pathURI->Length()+1);
								strncpy(default_cookie->path,pathURI->String(),pathURI->Length());
							if( pathURI->Contains(parentURI->String()) || parentURI->Contains(pathURI->String()) )
							{
								//printf("\t\t\t\tsetting cookie path to %s\n",pathURI->String());
							}
							else
							{
									//printf("cookie path isn't parent of request uri and isn't a child of the request uri's parent path; flagging cookie for rejection.\n");
									reject_cookie = true;
							}
						}
						else
						{
							default_cookie->path = new char[pathURI->Length()+1];
							memset(default_cookie->path,0,pathURI->Length()+1);
							strncpy(default_cookie->path,pathURI->String(),pathURI->Length());
						}
					}
					else if( strcasecmp(known_attributes[kac],"expires") == 0) {
						Date date(attvalue(attributes[i]));
						default_cookie->expiredate = date.Timestamp();
						default_cookie->expiredate = mktime(localtime(&default_cookie->expiredate));
						default_cookie->maxage = default_cookie->expiredate - current_time;
						default_cookie->session = false;
						/*
						if( default_cookie->expiredate < current_time)
						{
							 printf("expiration time has already passed\n");
							 //default_cookie->discard = true;
						}
						*/
					}
					else if( strcasecmp(known_attributes[kac],"domain") == 0 ) {
						printf("\t\t\t\tdomain: %s\n",attvalue(attributes[i]));
						if( default_cookie->domain != NULL)
						{
							memset(default_cookie->domain,0,strlen(default_cookie->domain)+1);
							delete default_cookie->domain;
							default_cookie->domain = NULL;
						}
						char *d = attvalue(attributes[i]);
						int32 len = strlen(d);
						default_cookie->domain = new char[len+1];
						memset(default_cookie->domain,0,len+1);
						strncpy(default_cookie->domain,d,len);
					}
					else if( strcasecmp(known_attributes[kac],"secure") == 0) {
						default_cookie->secure = true;
					}
					else if( strcasecmp(known_attributes[kac],"httponly") == 0 ) {
						default_cookie->httponly = true;
					}
					else if( strcasecmp(known_attributes[kac],"comment") == 0 ) {
						if( default_cookie->comment != NULL)
						{
							memset(default_cookie->comment,0,strlen(default_cookie->comment)+1);
							delete default_cookie->comment;
							default_cookie->comment = NULL;
						}
						char *comment = attvalue(attributes[i]);
						int32 len = strlen(comment);
						default_cookie->comment = new char[len+1];
						memset(default_cookie->comment,0,len+1);
						strncpy(default_cookie->comment,comment,len);
					}
					else if( strcasecmp(known_attributes[kac],"commenturl") == 0 ) {
						if( default_cookie->commenturl != NULL)
						{
							memset(default_cookie->commenturl,0,strlen(default_cookie->commenturl)+1);
							delete default_cookie->commenturl;
							default_cookie->commenturl = NULL;
						}
						char *commenturl = attvalue(attributes[i]);
						int32 len = strlen(commenturl);
						default_cookie->commenturl = new char[len+1];
						memset(default_cookie->commenturl,0,len+1);
						strncpy(default_cookie->commenturl,commenturl,len);
					}
					else if( strcasecmp(known_attributes[kac],"max-age") == 0) {
						default_cookie->session = false;
						default_cookie->maxage = atol(attvalue(attributes[i]));
						default_cookie->expiredate = current_time+default_cookie->maxage;
						if( default_cookie->maxage == 0)
						{
							 abort = true;
							 default_cookie->discard = true;
						}
					}
					else if( strcasecmp(known_attributes[kac],"discard") == 0) {
						default_cookie->discard = true;
						default_cookie->session = true;
					}
					else if( strcasecmp(known_attributes[kac],"ports") == 0 ) {
					}
					else if( strcasecmp(known_attributes[kac],"version") == 0) {
						default_cookie->version = atoi(attvalue(attributes[i]));
					}
					break;
				}
			}
		}
		delete []attributes;
	} // process the remaining cookie header info
	cookie_st *new_cookie = NULL,*last_cookie = cookie_head, *original = NULL;
	char **nvarr = new char*[2];
	if( last_cookie != NULL)
	{
		while( last_cookie->next != NULL)
			last_cookie = last_cookie->next;
	}
	char *buf = NULL,*eq = NULL;
	for( int32 i=0; i< nvpair_counter; i++)
	{
		tlen = strlen(cookie_array[i]);
		buf = new char[tlen+1];
		memset(buf,0,tlen+1);
		strncpy(buf,cookie_array[i],tlen);
		eq = strchr(buf,'=');
		tlen = eq-buf;
		nvarr[0] = strtok(buf,"=");
		nvarr[1] = strtok(NULL,"=");
		//printf("name: %s\tvalue: %s\n",nvarr[0],nvarr[1]);
		new_cookie = new cookie_st(default_cookie);
		if( nvarr[0] != NULL ) {
		tlen = strlen(nvarr[0]);
		new_cookie->name = new char[tlen+1];
		memset(new_cookie->name,0,tlen+1);
		strncpy(new_cookie->name,nvarr[0],tlen);
		}
		else
		{
			new_cookie->name = NULL;
		}
		if(nvarr[1] != NULL) {
		tlen = strlen(nvarr[1]);
		new_cookie->value = new char[tlen+1];
		memset(new_cookie->value,0,tlen+1);
		strncpy(new_cookie->value,nvarr[1],tlen);
		}
		else
		{
			new_cookie->value = NULL;
			
		}
		delete buf;
//		delete nvarr[0];
//		delete nvarr[1];
		if( DuplicatedCookie(new_cookie) )
		{
			original = FindCookie(new_cookie->name,new_cookie->path, new_cookie->domain);
			printf("found cookie %s, updating value from \"%s\" to \"%s\"\n",new_cookie->name,original->value,new_cookie->value);
			if( original->secure == new_cookie->secure )
			{
				original->discard = new_cookie->discard;
				original->session = new_cookie->session;
				original->httponly = new_cookie->httponly;
				original->expiredate = new_cookie->expiredate;
				original->maxage = new_cookie->maxage;
				original->datereceived = new_cookie->datereceived;
				original->version = new_cookie->version;
				if( original->value != NULL) {
					tlen = strlen(original->value);
					memset(original->value,0,tlen+1);
					delete original->value;
					original->value = NULL;
				}
				if( new_cookie->value != NULL ) {
					tlen = strlen(new_cookie->value);
					original->value = new char[tlen+1];
					memset(original->value,0,tlen+1);
					strncpy(original->value,new_cookie->value,tlen);
				}
				if( original->comment != NULL)
				{
					tlen = strlen(original->comment);
					memset(original->comment,0,tlen+1);
					delete original->comment;
					if(new_cookie->comment != NULL)
					{
						tlen = strlen(new_cookie->comment);
						original->comment = new char[tlen+1];
						memset(original->comment,0,tlen+1);
						strncpy(original->comment,new_cookie->comment,tlen);
					}
				}
				if( original->commenturl != NULL)
				{
					tlen = strlen(original->commenturl);
					memset(original->commenturl,0,tlen+1);
					delete original->commenturl;
					if(new_cookie->commenturl != NULL)
					{
						tlen = strlen(new_cookie->commenturl);
						original->commenturl = new char[tlen+1];
						memset(original->commenturl,0,tlen+1);
						strncpy(original->commenturl,new_cookie->commenturl,tlen);
					}
				}
				SaveCookie(original);
				delete new_cookie;
				continue;
			}
		}
		if( Validate(new_cookie,request_host,request_uri) )
		{
			//printf("Validation succeeded\n discard? %s\n", (new_cookie->discard ? "yes":"no"));
			if( new_cookie->discard != true) SaveCookie(new_cookie);
			if(cookie_head == NULL )
			{
				cookie_head = new_cookie;
				last_cookie = cookie_head;
			}
			else
			{
				last_cookie->next = new_cookie;
				last_cookie = last_cookie->next;
			}
		}
		else
		{
			 //printf("Validation failed\n");
			 delete new_cookie;
		}
		//PrintCookie(new_cookie);
		//delete new_cookie;
	}
	delete default_cookie;
	delete []nvarr;
	delete []cookie_array;
	cookie_array=NULL;
	memset(header_copy,0,header_length+1);
	delete header_copy;
	header_copy=NULL;
}
	//ClearExpiredCookies();
	return B_OK;
}
void CookieManager::ProcessAttributes(char **attributes, cookie_st *cookie, const char *request_host, const char *request_uri)
{
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
				strcasecmp(cookie->domain,current->domain)==0 && cookie->secure == current->secure
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
								for (uint32 j=0; j<strlen(ports_copy); j++)
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
			if (cur->session == false && cur->discard == false && cur->expiredate<=currenttime) {
//				printf("The following cookie has expired:\n");
//				PrintCookie(cur);
				if (cur==cookie_head) {
					printf("ClearExpiredCookies line 1764: cur %p\n",cur);
					cookie_head=cookie_head->next;
					BEntry ent(&cur->ref);
					if (ent.InitCheck() == B_OK && ent.Exists())
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
		printf("\tSession Cookie:\t%s\n",(cookie->session?"yes":"no"));
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
	//PrintCookie(cookie);
	BAutolock alock(lock);
	bool valid=false;
	if (alock.IsLocked()) {
		valid=true;
		time_t currenttime=time(NULL);
		currenttime=mktime(localtime(&currenttime));
//		printf("expiredate: %ld\ncurrent time: %ld\n",cookie->expiredate, currenttime);
		if( cookie->discard == false && cookie->session == false )
		{
		if (cookie->expiredate<=currenttime)
			return ValidateFail("Cookie has already expired.");
		if ((cookie->maxage+cookie->datereceived)<=currenttime)
			return ValidateFail("Cookie has already expired.");
		}
		else if( cookie->discard ) printf("Cookie is marked for discarding.\n");
		else if( cookie->session ) printf("Cookie is a session cookie.\n");
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
					return ValidateFail("Their domain contained no embedded dots. (4.3.2:2)");
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
						if(strcasecmp(attname,"Themis:httponly") == 0 )
						node.ReadAttr("Themis:httponly",B_BOOL_TYPE,0,&cookie->httponly,ai.size);
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
	//PrintCookie(cookie);	
	return B_OK;
}

status_t CookieManager::SaveCookie(cookie_st *cookie) {
	if (cookie==NULL)
		return B_ERROR;
	if (cookie->discard || cookie->session)
		return B_NO_ERROR;
	BEntry ent(&cookie->ref,true);
	if (ent.InitCheck()==B_OK) {
		SaveCookiePoint1:
		BPath testPath;
		ent.GetPath(&testPath);
		//printf("Saving cookie to: %s\n",testPath.Path());
		BFile file(&ent,B_CREATE_FILE|B_ERASE_FILE|B_WRITE_ONLY);
		BNodeInfo nodeInfo(&file);
		nodeInfo.SetType(ThemisCookieFile);
		nodeInfo.SetTo(NULL);
		file.Unset();
		BNode node(&cookie->ref);
		node.Lock();
		//char *type=new char[strlen(ThemisCookieFile)+1];
		//memset(type,0,strlen(ThemisCookieFile)+1);
		//strcpy(type,ThemisCookieFile);
		//node.WriteAttr("BEOS:TYPE",B_STRING_TYPE,0,type,strlen(ThemisCookieFile)+1);
		//delete type;
		node.WriteAttr("Themis:cookiesecure",B_BOOL_TYPE,0,&cookie->secure,sizeof(cookie->secure));
		node.WriteAttr("Themis:httponly",B_BOOL_TYPE,0,&cookie->httponly,sizeof(cookie->httponly));
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
		else 
		{
			status_t res = node.RemoveAttr("Themis:cookievalue");
			printf("removal of cookievalue attribute (null cookie value) successful? %s\n", (res == B_OK ? "yes":"no"));
		}
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
		printf("Cookie will be saved to: %s\n",path.Path());
		goto SaveCookiePoint1;
		
	}
	
	return B_OK;
}

void CookieManager::LoadAllCookies() {
	uint32 count = 0;
	BAutolock alock(lock);
	if (alock.IsLocked()) {
		entry_ref cookiedirref,current_ref;
		
		if (CookieSettings->FindRef("cookie_ref",&cookiedirref)==B_OK) {
			BDirectory *dir = new BDirectory(&cookiedirref);
			BPath path;
			BNode node;
			BNodeInfo nodeinfo;
			char mime[B_MIME_TYPE_LENGTH+1];
			cookie_st *curcookie=cookie_head;
			cookie_st *nucookie=NULL;
			if( curcookie != NULL)
			{
				//This function should only get called at initialization; if there are already cookies loaded, remove them...
				while( cookie_head != NULL)
				{
					curcookie = cookie_head->next;
					delete cookie_head;
					cookie_head = curcookie;
				}
			}
			while( dir->GetNextRef(&current_ref) == B_OK) {
				path.SetTo(&current_ref);
				//printf("Looking at: %s\n",path.Path());
				node.SetTo(&current_ref);
				nodeinfo.SetTo(&node);
				memset(mime,0,B_MIME_TYPE_LENGTH+1);
				nodeinfo.GetType(mime);
				if( strcasecmp(mime,ThemisCookieFile) == 0 )
				{
					//printf("We have a cookie!\n");
					nucookie = new cookie_st;
					nucookie->ref = current_ref;
					LoadCookie(nucookie);
					count++;
					if(cookie_head == NULL)
					{
						cookie_head = nucookie;
						curcookie = cookie_head;
					}
					else
					{
						curcookie->next = nucookie;
						curcookie = curcookie->next;
					}
				}
				node.Unset();
				nodeinfo.SetTo(NULL);
			}
		}
		/*
	BPath trashpath;
	BDirectory *trashdir;
	if (find_directory(B_TRASH_DIRECTORY,&trashpath)==B_OK) {
		trashdir=new BDirectory(trashpath.Path());
	} else {
		trashdir=new BDirectory("/boot/home/Desktop/Trash");
	}
		BEntry ent(&cookiedirref,true);
		BPath tempP(&ent);
		printf("Looking for cookies at %s\n",tempP.Path());
		struct stat devstat;
		ent.GetStat(&devstat);
		BVolume vol(devstat.st_dev);
		BQuery query;
		query.SetVolume(&vol);
		query.PushAttr("BEOS:TYPE");
		query.PushString(ThemisCookieFile);
		query.PushOp(B_EQ);
		status_t status = query.Fetch();
		switch(status)
		{
			case B_OK: {
				printf("Query is fine, still may not have a result though...\n");
			}break;
			case B_NO_INIT: {
				printf("Query fetch returned B_NO_INIT\n");
			}break;
			case B_BAD_VALUE: {
				printf("Query fetch returned B_BAD_VALUE\n");
			}break;
			case B_NOT_ALLOWED: {
				printf("Query fetch returned B_NOT_ALLOWED, you already fetched?\n");
			}break;
		}
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
//	printf("%ld cookie(s) loaded.\n",count);
		delete trashdir;
		*/
	}
	//PrintCookies();
	printf("%u cookie(s) loaded\n",count);
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

