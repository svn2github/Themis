/*
Copyright (c) 2002 Raymond "Z3R0 One" Rodgers. All Rights Reserved.

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

Original Author & Project Manager: Raymond "Z3R0 One" Rodgers (z3r0_one@yahoo.com)
Project Start Date: October 18, 2000
*/
#include "imageman.h"
#define PlugNameInfo "Image Handler"
#define PlugVersionInfo 0.05
#define PlugIDInfo 'imgh'
#include <stdio.h>
#include <Message.h>
#include <string.h>
#include "commondefs.h"

#include <String.h>
#include "plugman.h"
ImageMan *ImageManager;
BMessage *AppSettings;
BMessage **AppSettings_p;
status_t Initialize(void *info) {
  AppSettings=NULL;
  ImageManager=NULL;
  if (info!=NULL) {
	  
	BMessage *imsg=(BMessage *)info;
	if (imsg->HasPointer("settings_message_ptr")) {
		
		imsg->FindPointer("settings_message_ptr",(void**)&AppSettings_p);
		AppSettings=*AppSettings_p;
		
	}
	ImageManager=new ImageMan(imsg);
  } else {
  	ImageManager=new ImageMan;
  }
  
	return B_OK;
	
}
status_t Shutdown(bool now) {
//	if (now)
	delete ImageManager;
	return B_OK;
	
}
PlugClass *GetObject(void) {
	return ImageManager;
	
}
ImageMan::ImageMan(BMessage *info)
	:PlugClass(info) {
	printf("image handler loaded!\n");
	imagelist=NULL;
	CacheSys=NULL;
	CacheSys=(CachePlug*)PlugMan->FindPlugin(CachePlugin);
	if (CacheSys!=NULL)
		cache_user_token=CacheSys->Register(Type(),"Image Handler");
	mimes=NULL;
	TRoster=new BTranslatorRoster();
	TRoster->AddTranslators(NULL);
	translators=NULL;
	transcount=0;
	int32 transvers;
	const char *transname,*transinfo;
	TRoster->GetAllTranslators(&translators,&transcount);
	imagetranscount=0;
	imagetrans=NULL;
	const translation_format *tinfo;
	int32 outformats;
//build a list of all the translators, and count the image translators
	for (int32 i=0; i<transcount; i++) {
		TRoster->GetTranslatorInfo(translators[i],&transname, &transinfo, &transvers);
		TRoster->GetInputFormats(translators[i],&tinfo,&outformats);
		for (int32 j=0; j<outformats; j++) {
			if (strncasecmp("image/",tinfo[j].MIME,6)==0) {
				imagetranscount++;
			} else {
				continue;
			}
		}
	}
	imagetrans=new translator_info[imagetranscount];
	int32 k=0;
//cycle through the translators getting the mime types of the supported image formats
	for (int32 i=0; i<transcount; i++) {
		TRoster->GetInputFormats(translators[i],&tinfo,&outformats);
		for (int32 j=0; j<outformats; j++) {
			if (strncasecmp("image/",tinfo[j].MIME,6)==0) {
				imagetrans[k].type=tinfo[j].type;
				imagetrans[k].translator=translators[i];
				imagetrans[k].group=tinfo[j].group;
				imagetrans[k].quality=tinfo[j].quality;
				imagetrans[k].capability=tinfo[j].capability;
				strcpy(imagetrans[k].name,tinfo[j].name);
				strcpy(imagetrans[k].MIME,tinfo[j].MIME);
				AddMIMEType((char*)tinfo[j].MIME);
				k++;
			}
		}
	}
	printf("The following image types are supported by ImageHandler:\n");
	smt_st *cur=mimes;
	while (cur!=NULL) {
		printf("\t%s\n",cur->type);
		cur=cur->next;
	}
	image_list=NULL;
		
	
}

ImageMan::~ImageMan() {
	printf("~ImageMan\n");
	delete TRoster;
	delete []imagetrans;
	delete []translators;
	smt_st *cur;
	while (mimes!=NULL) {
		cur=mimes->next;
		delete mimes;
		mimes=cur;
	}
	Image *curimage=NULL;
	while (imagelist!=NULL) {
		curimage=imagelist->Next();
		delete imagelist;
		imagelist=curimage;
	}
	printf("~ImageMan end\n");
	
}

bool ImageMan::IsPersistent() {
	return true;
}

uint32 ImageMan::PlugID() {
	return PlugIDInfo;
}

char *ImageMan::PlugName() {
	return PlugNameInfo;
}

float ImageMan::PlugVersion() {
	return PlugVersionInfo;
}

status_t ImageMan::ReceiveBroadcast(BMessage *msg) {
	int32 command=0;
	msg->FindInt32("command",&command);
	printf("ImageHandler ReceiveBroadcast: \n");
	msg->PrintToStream();
	
	switch(command) {
		case COMMAND_INFO: {
			switch(msg->what) {
				case IH_LOAD_IMAGE: {
					const char *url;
					msg->FindString("URL",&url);
					image_info_st *current=image_list,*last;
					while (current!=NULL)
					{
						if (strcmp(current->url,url)==0)
						{
							MessageSystem *origin=NULL;
							msg->FindPointer("_broadcast_origin_pointer_",(void **)&origin);
							BMessage reply(IH_IMAGE_LOADED);
							reply.AddInt32("command",COMMAND_INFO);
							if (current->image_available)
							{
								reply.AddPointer("bitmap_ptr",current->bitmap);
								reply.AddInt32("frame_count",1);
								reply.AddInt32("width",current->bitmap->Bounds().IntegerWidth());
								reply.AddInt32("height",current->bitmap->Bounds().IntegerHeight());
							}
							
							void * element;
							msg->FindPointer("element",&element);
							reply.AddPointer("element",element);
							if (origin!=NULL)
								origin->BroadcastReply(&reply);
							break;
						}
						last=current;
						current=current->next;
						
					}
					if (current==NULL)
					{
						if (last!=NULL)
						{
							last->next=new image_info_st;
							last=last->next;
							
						} else {
							if (image_list==NULL) //which it should
							{
								image_list=new image_info_st;
								last=image_list;
								
							}
							
						}
						msg->FindPointer("element",&last->renderer_bitmap_element);
						last->url=new char[strlen(url)+1];
						memset((char*)last->url,0,strlen(url)+1);
						strcpy((char*)last->url,url);
						//see if we have the image in the cache already...
						if (CacheSys!=NULL)
						{
							last->cache_object_token=CacheSys->FindObject(cache_user_token,url);
							if (last->cache_object_token>=0)
							{//yes, we have it the cache!
								last->bitmap=BTranslatorRoster::GetBitmap(CacheSys->ObjectIOPointer(cache_user_token,last->cache_object_token);
								//if it's an animated image, we'll need to more processing first...
							} else {//request the image from the appropriate protocol
								BMessage request(LoadingNewPage);
								request.AddString("target_url",url);
								request.AddInt32("command",COMMAND_RETRIEVE);
								Broadcast(MS_TARGET_PROTOCOLS,&request);
								
							}
							
							
						}
						
						
					}
					
				}break;
				case PlugInLoaded: {
					PlugClass *pobj=NULL;
					msg->FindPointer("plugin",(void**)&pobj);
					if (pobj!=NULL) {
						if (pobj->PlugID()=='cash') {
							CacheSys=(CachePlug*)pobj;
							cache_user_token=CacheSys->Register(Type(),"Image Handler");
						}
					}
				}break;
				case PlugInUnLoaded: {
					uint32 type=0;
					type=msg->FindInt32("type");
					switch(type) {
						case (TARGET_CACHE|TYPE_RAM|TYPE_DISK): {
							cache_user_token=0;
							CacheSys=NULL;
						}break;
						default: {
							
							//do nothing
						}
						
					}
					
				}break;
				case B_QUIT_REQUESTED: {
				}break;
				case LoadingNewPage: {
/*
since we're going to be looking at a new page, with new images, clear
out our stored images.
*/
					if (imagelist!=NULL) {
						Image *cur;
						while (imagelist!=NULL) {
							cur=imagelist->Next();
							delete imagelist;
							imagelist=cur;
						}
						
					}
					
				}break;
				case ReturnedData: {
					bool done=false;
					BString mime;
					BString url;
					uint64 br=0,cl=0;
					BPositionIO *data=NULL;
					msg->FindString("mimetype",&mime);
					msg->FindBool("request_done",&done);
					printf("Image Handler: checking mime type - ");
					fflush(stdout);
					if (TypeSupported((char*)mime.String())) {
						printf("Supported: %s\n",mime.String());
						if (done) {
							msg->FindString("url",&url);
							BMallocIO *imagedata=NULL;
							int32 objtoken=0;
							if (CacheSys!=NULL) {
								if (msg->HasInt32("cache_object_token"))
									objtoken=msg->FindInt32("cache_object_token");
								else
									objtoken=CacheSys->FindObject(cache_user_token,url.String());
								if (objtoken<0)
									return PLUG_HANDLE_GOOD;
								printf("creating imagedata\n");
								imagedata=new BMallocIO();
								ssize_t size=CacheSys->GetObjectSize(cache_user_token,objtoken);
								printf("cache object size is: %ld\n",size);
								unsigned char *data=new unsigned char[size];
								memset(data,0,size);
								size=CacheSys->Read(cache_user_token,objtoken,data,size);
								printf("read %ld bytes from cache.\n",size);
								imagedata->Write(data,size);
								printf("wrote data to imagedata\n");
								memset(data,0,size);
								delete data;
								data=NULL;
							}
							
//							msg->FindPointer("data_pointer",(void**)&imagedata);
							if (imagedata!=NULL) {
								
								BBitmap *bmp=BTranslationUtils::GetBitmap(imagedata);
								printf("bmp: %p\n",bmp);
								if (imagelist==NULL) {
									imagelist=new Image(bmp,(char*)url.String());
									printf("image size:\n");
									BRect r;
									imagelist->GetSize(&r);
									imagelist->SetCacheToken(objtoken);
									r.PrintToStream();
								} else {
									
									Image *cur=imagelist;
/*
	Check to see if we already have an image for the specified URL. If not, then add it.
	If so, then update the existing image.
*/
									while (cur!=NULL) {
										if (strcasecmp(url.String(),cur->URL())==0) 
											break;
										cur=cur->Next();
									}
									if (cur!=NULL) {
										cur->SetBitmap(bmp);
									} else {
										cur=new Image(bmp,(char*)url.String());
										cur->SetCacheToken(objtoken);
										imagelist->SetNext(cur);
									}
									
								}
								
							}
							
						}
					
					} else {
						printf(" unsupported: %s\n",mime.String());
					}
				
				}break;
			}
			
		}break;
		case COMMAND_INFO_REQUEST: {
/*
	Someone is asking for some information about this plug-in.
*/
			int32 replyto=0;
			msg->FindInt32("ReplyTo",&replyto);
			switch(msg->what) {
				case GetSupportedMIMEType: {
/*
	Ah, they want to know what MIME types this plug-in supports.
*/					
			if (msg->HasBool("supportedmimetypes")) {
				BMessage types(SupportedMIMEType);
				smt_st *cur=mimes;
				while (cur!=NULL) {
					types.AddString("mimetype",cur->type);
					cur=cur->next;
				}
				types.AddInt32("command",COMMAND_INFO);
				PlugClass *plug=NULL;
				if (msg->HasPointer("ReplyToPointer")) {
					msg->FindPointer("ReplyToPointer",(void**)&plug);
					if (plug!=NULL)
						plug->BroadcastReply(&types);
				} else {	
				//	BMessage container;
				//	container.AddMessage("message",&types);
					PlugMan->Broadcast(replyto,&types);
				}
				
			}
				}break;
			}
			
		}break;
		default: {
			printf("TestPlug addon doesn't handle this broadcast.\n");
			msg->PrintToStream();
			return PLUG_DOESNT_HANDLE;
		}
	
	}
	return PLUG_HANDLE_GOOD;
}

status_t ImageMan::BroadcastReply(BMessage *msg) {
	return PLUG_DOESNT_HANDLE;
}

int32 ImageMan::Type() {
	return IMAGE_HANDLER;
}

char *ImageMan::SettingsViewLabel() {
	return "Image Handler";
}
BView *ImageMan::SettingsView() {
	return NULL;
}
const char *ImageMan::TranslatorName(translator_id id) 
{
	const char *name=NULL,*transinfo=NULL;
	int32 transvers=0;
	for (int32 i=0; i<transcount; i++) {
		if (id==translators[i]) {
			TRoster->GetTranslatorInfo(translators[i],&name, &transinfo, &transvers);
		}
	}
	return name;
}
smt_st *ImageMan::AddMIMEType(char *type) 
{
	if (type==NULL)
		return NULL;
	smt_st *cur=mimes;
	while (cur!=NULL) {
		if (strcasecmp(cur->type,type)==0) {
			break;
		}
		cur=cur->next;
	}
	if (cur!=NULL)
		return cur;
	cur=mimes;
	if (cur==NULL) {
		cur=new smt_st;
		mimes=cur;
	} else {
		while (cur->next!=NULL)
			cur=cur->next;
		cur->next=new smt_st;
		cur=cur->next;
	}
	cur->type=new char[strlen(type)+1];
	memset(cur->type,0,strlen(type)+1);
	strcpy(cur->type,type);
	return cur;
}
bool ImageMan::TypeSupported(char *type) 
{
	if (type==NULL)
		return false;
	smt_st *cur=mimes;
	bool found=false;
	while (cur!=NULL) {
		if (strcasecmp(cur->type,type)==0) {
			found=true;
			break;
		}
		cur=cur->next;
	}
	return found;
}
uint32 ImageMan::BroadcastTarget() 
{
	return MS_TARGET_DOM_VIEWER;
}
