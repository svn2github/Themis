#ifndef _stripwhitefunction
#define _stripwhitefunction

#include <ctype.h>
#include <SupportKit.h>
#include <string.h>

void stripwhite(const char *orig,char *result)
 {
  int32 len=strlen(orig), pos=0;
//  memset(result,0,strlen(orig));
  for (int32 i=0;i<len;i++)
   {
    if (isspace(orig[i]))
     continue;
    result[pos]=orig[i];
    pos++;
   }
 }
void stripfrontwhite(const char *orig,char *result)
 {
  int32 len=strlen(orig),pos=0,pos2=0;
  while(isspace(orig[pos]))
   pos++;
  for (int32 i=pos;i<len;i++)
   {
    result[pos2]=orig[i];
    pos2++;
   }
 }
void stripendwhite(const char *orig,char *result)
 {
  int32 len=strlen(orig),pos=0;
  pos=len;
  while(isspace(orig[pos]))
   pos--;
  strncpy(result,orig,pos);
 }
#endif
