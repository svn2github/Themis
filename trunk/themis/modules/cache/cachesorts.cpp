#include "cachesorts.h"
#include "cacheobject.h"
#include <kernel/OS.h>
#include <stdio.h>

int sort_by_size(const void *a, const void *b)
{
	CacheObject *one=*(CacheObject**)a, *two=*(CacheObject**)b;
//	printf("one:\t%s\ntwo:\t%s\n",one->URL(),two->URL());
	return (one->Size()-two->Size());
	
}

int sort_by_age(const void *a, const void *b)
{
//	CacheObject *one=*(CacheObject**)a, *two=*(CacheObject**)b;
	
	return 0;
}

