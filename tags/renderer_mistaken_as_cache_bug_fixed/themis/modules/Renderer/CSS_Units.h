#include <string.h>

#include <SupportDefs.h>

#ifndef CSS_UNITS_H_
#define CSS_UNITS_H_

/***********************************************************************
	Most of what is built here is to handle CSS Types as defined at:
	http://www.w3.org/TR/REC-CSS2/syndata.html
***********************************************************************/

//Used by GetFloatValue()
inline const char *GetValue(const char *value, int32 effectiveLength)
{
	char *val = new char[effectiveLength+1];
	val[effectiveLength] = '\0';
	return strncpy(val,value,effectiveLength);
}	

float GetFloatValue(const char *value, int32 dpi, float containingLength = 0);

#endif



