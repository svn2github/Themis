#include "protocol_plugin.h"
#include <Autolock.h>
#include <String.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
BLocker ProtocolPlugClass::UOU_lock(true);
ProtocolPlugClass::ProtocolPlugClass(BMessage *info,const char *msg_sys_name)
	:PlugClass(info,msg_sys_name), _Themis_Networking_::NetworkableObject()
{
}

const char *ProtocolPlugClass::SetURL(const char* url)
{
	URL=url;
	return URL.String();
}
void ProtocolPlugClass::Config(BMessage *msg)
{
}

BMessage *ProtocolPlugClass::SupportedTypes(void)
{
	return NULL;
}

void ProtocolPlugClass::FindURI(const char *url,BString &host,int &port,BString &uri)
{
}

void ProtocolPlugClass::ParseResponse(unsigned char *resp,size_t size)
{
}
unsigned char *ProtocolPlugClass::GetDoc(BString &host,int &port,BString &uri)
{
	return NULL;
}
unsigned char *ProtocolPlugClass::GetDoc(const char* url)
{
	return NULL;
}
int32 ProtocolPlugClass::GetURL(BMessage *info)
{
	return 0;
}
int32 ProtocolPlugClass::TypePrimary()
{
	return ProtocolPlugin;
}

int32 ProtocolPlugClass::UnObfuscateURL(const char *obfuscated,char *unobfuscated, int32 max_size,bool host_only)
{
	status_t stat=B_ERROR;
	BAutolock alock(UOU_lock);
	if (alock.IsLocked()) {
		//BString str(obfuscated);
		char *temp_str=new char[max_size+1];
		char *host_str=new char[max_size+1];
		char *uri_str=new char[max_size+1];
		memset(unobfuscated,0,max_size);
		memset(temp_str,0,max_size+1);
		memset(host_str,0,max_size+1);
		memset(uri_str,0,max_size+1);
		//find host location
			char *start=(char*)obfuscated;
			char *protocol_str=strstr(start,"://");
			char *at_symbol=strchr(start,'@');
			char *first_slash=NULL;
			if (protocol_str!=NULL) {
				first_slash=strchr(protocol_str+3,'/');
				if (at_symbol==NULL) {
					start=protocol_str+3;
				} else {
					start=at_symbol+1;
				}
			} else {
				first_slash=strchr(start,'/');
				if (at_symbol!=NULL) {
					start=at_symbol+1;
				} 
			}
		//start should now point to the start of the host info
		{
			//now, resolve any 32 bit numbers used as the server location...
			if (first_slash>start) {
				
				strncpy(temp_str,start,first_slash-start);
			}
			UnObfuscateHost(temp_str,host_str,max_size);
		}
		//point start at the first slash of the URI.
		if (first_slash>start) {
			start=first_slash;
			printf("start: %s\n",start);
			memset(temp_str,0,max_size+1);
		} else {
			//there is no uri; just the protocol (possibly) and host... we're done!
			if (protocol_str!=NULL) {
				strncpy(unobfuscated,obfuscated,(protocol_str+3)-obfuscated);
				strncat(unobfuscated,host_str,min_c(strlen(host_str),max_size-(strlen(unobfuscated)+strlen(host_str))));
			} else {
				strncpy(unobfuscated,host_str,min_c((int32)strlen(host_str),max_size));
			}
			memset(host_str,0,max_size);
			memset(temp_str,0,max_size);
			memset(uri_str,0,max_size);
			delete host_str;
			delete temp_str;
			delete uri_str;
			protocol_str=NULL;
			at_symbol=NULL;
			start=NULL;
			first_slash=NULL;
			return B_OK;
		}
		//resolve hex in URI
		char *percent=strchr(start,'%');
		if (percent!=NULL) {
			int32 counter=percent-start;
			strncpy(temp_str,start,counter);
			char numstr[3];
			char *endp=NULL;
			char c;
			char *prev_percent=NULL,*prev_percent2;
			do {
				memset(numstr,0,3);
				strncpy(numstr,percent+1,2);
				c=(char)strtoul(numstr,&endp,16);
				prev_percent2=prev_percent;
				prev_percent=percent;
				percent=strchr(percent+1,'%');
				*(temp_str+(strlen(temp_str)))=c;
				if (percent==NULL) {
					strncat(temp_str,prev_percent+3,/*(max_size-counter)*/(prev_percent)-(prev_percent2));
					counter+=prev_percent-prev_percent2;
				} else {
					strncat(temp_str,prev_percent+3,percent-(prev_percent+3));
					counter+=percent-(prev_percent+3);
				}
			}while(percent!=NULL);
		//stage 1 of unobfuscating is complete. copy the results to the target output string.
		strncpy(uri_str,temp_str,strlen(temp_str));
		memset(temp_str,0,max_size+1);
		} else {
			strncpy(uri_str,start,max_size-strlen(temp_str));
		}
		if (protocol_str!=NULL) {
			strncpy(unobfuscated,obfuscated,protocol_str+3-obfuscated);
			strncat(unobfuscated,host_str,min_c(strlen(host_str),max_size-(strlen(host_str)+strlen(unobfuscated))));
			if (uri_str!=NULL) {
				strncat(unobfuscated,uri_str,min_c(strlen(uri_str),max_size-(strlen(uri_str)+strlen(unobfuscated))));
			}
		} else {
			strncpy(unobfuscated,host_str,min_c(strlen(host_str),max_size-(strlen(host_str)+strlen(unobfuscated))));
			if (uri_str!=NULL) {
				strncat(unobfuscated,uri_str,min_c(strlen(uri_str),max_size-(strlen(uri_str)+strlen(unobfuscated))));
			}
		}
			memset(host_str,0,max_size);
			memset(temp_str,0,max_size);
			memset(uri_str,0,max_size);
			delete host_str;
			delete temp_str;
			delete uri_str;
			protocol_str=NULL;
			at_symbol=NULL;
			start=NULL;
			first_slash=NULL;
			stat=B_OK;
	}
	return stat;
}
int32 ProtocolPlugClass::UnObfuscateHost(const char *obfuscated, char *unobfuscated, int32 max_size)
{
	status_t stat=B_ERROR;
	BAutolock alock(UOU_lock);
	if (alock.IsLocked()) {
			int32 length=strlen(obfuscated);
			char *addr=new char[length+1];
			memset(addr,0,length+1);
			memset(unobfuscated,0,max_size);
			strncpy(addr,obfuscated,length);
			bool all_digits=true;
			for (int32 i=0; i<length; i++) {
				if (!isdigit(addr[i])) {
					all_digits=false;
					break;
				}
			}
			
			if (all_digits) {//we have a 32-bit number!
//				char *endp;
				uint64 largenum=0L;
				char con[2];
				for (int i=0; i<length; i++) {
					strncpy(con,addr+i,1);
					largenum+=atoi(con)*(uint64)pow(10,(length-i)-1);
				}
				if (largenum>0xffffffff) {
					uint32 ip_addr_32=(uint32)largenum^0xffffffff;
					uint8 ip_addr[4];
//					uint32 total=0;
					uint8 num=0;
					for (int8 i=3; i>=0; i--) {
						num=ip_addr_32%256;
						ip_addr[i]=num;
						ip_addr_32-=num;
						ip_addr_32/=256;
					}
					sprintf(unobfuscated,"%d.%d.%d.%d",255-ip_addr[0],255-ip_addr[1],255-ip_addr[2],255-ip_addr[3]);
				} else {
					uint32 ip_addr_32=(uint32)largenum;
					uint8 ip_addr[4];
//					uint32 total=0;
					uint8 num=0;
					for (int8 i=3; i>=0; i--) {
						num=ip_addr_32%256;
						ip_addr[i]=num;
						ip_addr_32-=num;
						ip_addr_32/=256;
					}
					sprintf(unobfuscated,"%d.%d.%d.%d",ip_addr[0],ip_addr[1],ip_addr[2],ip_addr[3]);
				}
			}
			delete addr;
	}
	return stat;
}
