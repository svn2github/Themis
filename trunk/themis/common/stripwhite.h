#ifndef _stripwhitefunction
#define _stripwhitefunction

#include <ctype.h>
#include <SupportKit.h>
#include <string.h>

/*
These functions strip white space from the entirety, beginning, or end of a string, 
respectively.
*/
void stripwhite(const char *orig,char *result) {
	int32 len=strlen(orig), pos=0;
	//  memset(result,0,strlen(orig));
	for (int32 i=0;i<len;i++) {
		if (isspace(orig[i]))
			continue;
		result[pos]=orig[i];
		pos++;
	}
}
void stripfrontwhite(const char *orig,char *result) {
	int32 len=strlen(orig),pos=0,pos2=0;
	while(isspace(orig[pos]))
		pos++;
	for (int32 i=pos;i<len;i++) {
		result[pos2]=orig[i];
		pos2++;
	}
}
void stripendwhite(const char *orig,char *result) {
	int32 len=strlen(orig),pos=0;
	pos=len;
	while(isspace(orig[pos]))
		pos--;
	strncpy(result,orig,pos);
}
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
