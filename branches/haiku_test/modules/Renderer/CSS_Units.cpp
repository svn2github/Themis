/*
	Copyright (c) 2003 Olivier Milla. All Rights Reserved.
	
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
	
	Original Author: 	Olivier Milla (methedras@online.fr)
	Project Start Date: October 18, 2000
*/
#include <stdio.h>
#include <stdlib.h>

#include "CSS_Units.h"

float GetFloatValue(const char *value, int32 dpi, float containingLength = 0)
{
	float res 					= 0.0;
	const char *effectiveValue 	= NULL;
	uint32 length 				= strlen(value);
	
	if (strcmp(value+(length-2),"em") == 0){
		effectiveValue = GetValue(value,length-2);
		//Need the font-size		
	}
	else if (strcmp(value+(length-2),"ex") == 0){
		effectiveValue = GetValue(value,length-2);
		//calculate the x-height		
	}
	else if (strcmp(value+(length-2),"px") == 0){
		effectiveValue = GetValue(value,length-2);	
		res = atof(effectiveValue);		
	}
	else if (strcmp(value+(length-2),"in") == 0){
		effectiveValue = GetValue(value,length-2);	
		res = atof(effectiveValue)*dpi;		
	}
	else if (strcmp(value+(length-2),"cm") == 0){
		effectiveValue = GetValue(value,length-2);	
		res = atof(effectiveValue)/2.54*dpi;
	}
	else if (strcmp(value+(length-2),"mm") == 0){
		effectiveValue = GetValue(value,length-2);
		res = atof(effectiveValue)/25.4*dpi;		
	}
	else if (strcmp(value+(length-2),"pt") == 0){
		effectiveValue = GetValue(value,length-2);	
		res = atof(effectiveValue)/72*dpi;		
	}
	else if (strcmp(value+(length-2),"pc") == 0){
		effectiveValue = GetValue(value,length-2);
		res = atof(effectiveValue)*12/72*dpi;			
	}
	else if (strcmp(value+(length-1),"%") == 0){
		effectiveValue = GetValue(value,length-1);	
		res = atof(effectiveValue)/containingLength*100;
	}
	else
		printf("GetFloatValue::Argh !!!!!!!!!!!!!!!!!!!!!! Unknown Unit!\n");
	
	delete[] effectiveValue;	 

	return res;
}

