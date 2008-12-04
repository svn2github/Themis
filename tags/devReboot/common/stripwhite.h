#ifndef _stripwhitefunction
#define _stripwhitefunction

#include <ctype.h>
#include <SupportKit.h>
#include <string.h>

/*
These functions strip white space from the entirety, beginning, or end of a string, 
respectively.
*/
void stripwhite(const char *orig,char *result);
void stripfrontwhite(const char *orig,char *result);
void stripendwhite(const char *orig,char *result);

/*!
This function trims the white space off the beginning and end of a passed in character string
and returns a brand new string (allocated with the "new" function) as the result.
*/
char *trim(char *target);

#endif
