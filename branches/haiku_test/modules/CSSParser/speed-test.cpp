#include <stdlib.h>
#include <stdio.h>

#include "InputStream.h"

const char *kFileName = "/var/log/syslog";


int
main(int argc,char **argv)
{
	if (argc < 2 || argc > 3)
	{
		fprintf(stderr,"usage: %s testnr [count]\n",argv[0]);
		return -1;
	}
	int test = atol(argv[1]);
	int count = 10,lines = 0;

	if (argc == 3)
		count = atol(argv[2]);

	printf("test: ");
	switch(test)
	{
		case 0: puts("performing empty loop"); break;
		case 1: puts("c-lib's fgets()"); break;
		case 2: puts("InputStream class"); break;
	}

	for(int i = count;i-- > 0;)
	{
		switch(test)
		{
			case 0:
			{
				FILE *file;
				
				if ((file = fopen(kFileName,"r")) != NULL)
				{
					fclose(file);
				}
				else
				{
					fprintf(stderr,"could not open file \"%s\"\n",kFileName);
					return -1;
				}
				break;
			}
			case 1:
			{
				FILE *file;
				
				if ((file = fopen(kFileName,"r")) != NULL)
				{
					char c[1024];
					
					while(fgets(c,1024,file) != NULL)
						lines++;
					fclose(file);
				}
				else
				{
					fprintf(stderr,"could not open file \"%s\"\n",kFileName);
					return -1;
				}
				break;
			}
			case 2:
			{
				FileInputStream in(kFileName);
				
				if (in.InitCheck() == RC_OK)
				{
					char *buffer = NULL;
					int32 line;

					while(in.ReadLine(&buffer,&line))
						lines++;
				}
				else
				{
					fprintf(stderr,"could not open file \"%s\"\n",kFileName);
					return -1;
				}
				break;
			}
		}
	}
	printf("%d lines read.\n",lines);
}
