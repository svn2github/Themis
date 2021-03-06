/*
Copyright (c) 2000 Z3R0 One. All Rights Reserved.

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

Original Author & Project Manager: Z3R0 One (z3r0_one@yahoo.com)
Project Start Date: October 18, 2000
*/
#ifndef common_defines
#define common_defines

#define ThemisAppSig "application/x-vnd.becommunity-themis"
#define ThemisCacheMIME "application/x-Themis-cache"
#define ThemisCookieFile "application/x-Themis-cookie"


#define HTTPPlugin 'http'
#define HTTPSPlugin 'htps'
#define CachePlugin 'cash'
#define JavascriptPlugin 'jsct'
//#define HTMLPlugin 'html'
//#define HTMLTagPlugin 'httg'
#define CookiePlugin 'cook'

#define FindCachedObject 'fcob'
#define CreateCacheObject 'ccob'
#define CacheObjectNotFound 'conf'
#define UpdateCachedObject 'ucob'
#define CacheClearFiles 'clca'
#define CachedObject 'cobj'

//There are different methods of caching data according to the http rfc.
//Define them here.

//Cache control identifiers
#define UsesCache 1
#define DoesNotUseCache 0
#define EncryptCache 2
//clear out this item on Themis exit or cache add-on unload
#define CacheClearOnExit 4
//clear out this item on Themis exit, cache add-on unload, or when a new URL is entered
#define CacheClearOnNewPage 8
//clear out this item after a specific time limit
#define CacheClearOnTimeLimit 16

//Cookie Control identifiers
#define FindCookie 'fcok'
#define NoCookieFound 'nocf'
#define FoundCookie 'fndc'
#define SetCookie 'setc'

#define DisableJavascript 'djvs'
#define EnableJavascript 'ejvs'
#define GetJavascriptStatus 'gjss'
#define ProcessJSScript 'pjss'
#define ClearJSScripts 'cjss'


//general notification
#define LoadingNewPage 'ldnp'
#define HeartbeatMessage 'hrtb'

//protocol items
#define FetchItem 'fitm'
#define StayAlive 'live'
#define ReturnedData 'data'
#define SendData 'send'
#define ThreadDone 'done'
#define ProtocolResponse 'prsp'
#define ReloadData 'rldt'
#define ProtocolConnectionClosed '_pcc'
#define UnexpectedDisconnect 'unds'
#define ProtocolConnectionFailed '_pcf'
#define ProtocolTransactionStatus '_pts'

//This is to update the displayed URL; usually done from the protocol.
#define UpdateDisplayedURL 'udur'

//plug in management
#define PlugInLoaded 'pild'
#define PlugInUnLoaded 'piul'

#define AddInitInfo '_aii'
#define RemoveInitInfo '_rii'
//#define ProtocolBroadcast 'prbc'
//#define CacheBroadcast 'chbc' 
#define AppWideBroadcast 'awbc'
#define PluginBroadcast 'plgb'

#define GetSupportedMIMEType 'gsmt'
#define SupportedMIMEType 'spmt'
#endif
