#ifndef _pluginabstractclass
#define _pluginabstractclass

enum PlugType{ProtocolPlugin=0,ContentPlugin=1};
class protocol_plugin
 {
  public:
   protocol_plugin(){}
   virtual ~protocol_plugin(){};
   virtual char *GetPluginName(void)=0;
   virtual int32 GetPluginID(void)=0;
   virtual float GetPluginVers(void)=0;
   virtual PlugType PluginType(void)=0;
   virtual status_t Go(void)=0;
   virtual bool IsPersistant(void)=0;
   virtual BMessage *SupportedTypes(void)=0;
  /*extern "C" __declspec(dllexport)*/virtual void FindURI(const char *url,BString &host,int &port,BString &uri)=0;
  /*extern "C" __declspec(dllexport)*/virtual void ParseResponse(unsigned char *resp,size_t size)=0;
  /*extern "C" __declspec(dllexport)*/virtual unsigned char *GetDoc(BString &host,int &port,BString &uri)=0;
  /*extern "C" __declspec(dllexport)*/virtual unsigned char *GetDoc(const char* url)=0;
  /*extern "C" __declspec(dllexport)*/virtual unsigned char *GetURL(const char* url)=0;
//   virtual status_t Initialize(bool go=false)=0;
//   virtual status_t Shutdown(bool now=false)=0;
 };

#endif