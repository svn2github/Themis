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

