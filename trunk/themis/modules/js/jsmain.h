#ifndef cookie_main
#define cookie_main
#include "plugclass.h"
#include <SupportKit.h>
#include <AppKit.h>
extern "C" __declspec(dllexport)status_t Initialize(void *info=NULL);
extern "C" __declspec(dllexport)status_t Shutdown(bool now=false);
extern "C" __declspec(dllexport)PlugClass *GetObject(void);
#endif
