/* css-test.cpp - tests the portable CSS function set
**
** Copyright 2001, pinc Software. All Rights Reserved.
*/


#include <stdio.h>

#include "StyleSheet.h"
#include "InputStream.h"


int main(int argc,char **argv)
{
	if (argc != 2)
	{
		fprintf(stderr,"usage: %s css-file\n",argv[0]);
		return -1;
	}
	FileInputStream input(argv[1]);
	
	if (input.InitCheck() >= RC_OK)
	{
		StyleSheet css(CSS_USER_TYPE,&input);
	}
}
