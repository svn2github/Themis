#ifndef _http_main
#define _http_main
#include <SupportKit.h>
#include <AppKit.h>
#include "plugin.h"
//namespace Themis_Networking
// {
  extern "C" __declspec(dllexport)status_t Initialize(bool go=false);
  extern "C" __declspec(dllexport)status_t Shutdown(bool now=false);
  extern "C" __declspec(dllexport)protocol_plugin* GetObject(void);
  extern "C" __declspec(dllexport)class http_protocol:public protocol_plugin
   {
    private:
     size_t buflen,rawbuflen;
     bool chunked;
     int32 chunksize;
     bool headerreceived;
    public:
     http_protocol();
     ~http_protocol();
     BMessage headers;
     BMallocIO *buffer,*RawBuffer; //parsed and unparsed buffers respectively
  /*extern "C" __declspec(dllexport)*/char *GetPluginName(void);
  /*extern "C" __declspec(dllexport)*/int32 GetPluginID(void);
  /*extern "C" __declspec(dllexport)*/float GetPluginVers(void);
  /*extern "C" __declspec(dllexport)*/status_t Go(void);
                                      bool IsPersistant(void);
                                      PlugType PluginType(void);
                                      BMessage *SupportedTypes(void);
  /*extern "C" __declspec(dllexport)*/void FindURI(const char *url,BString &host,int &port,BString &uri);
  /*extern "C" __declspec(dllexport)*/void ParseResponse(unsigned char *resp,size_t size);
  /*extern "C" __declspec(dllexport)*/unsigned char *GetDoc(BString &host,int &port,BString &uri);
  /*extern "C" __declspec(dllexport)*/unsigned char *GetDoc(const char* url);
  /*extern "C" __declspec(dllexport)*/unsigned char *GetURL(const char* url);
   };
// };
#endif
