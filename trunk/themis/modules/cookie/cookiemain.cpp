#include "cookiedefs.h"
#include "cookiemain.h"
#include "cookieman.h"
cookieman *CookieMonster;


status_t Initialize(void *info) {
	CookieMonster=NULL;
	CookieMonster=new cookieman((BMessage*)info);
	return B_OK;
	
}
status_t Shutdown(bool now) {
	if (now)
		if (CookieMonster!=NULL)
			delete CookieMonster;
	return B_OK;
	
}
PlugClass *GetObject(void) {
	return CookieMonster;
	
}
