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
/*!
\file
\brief Definitions that can and/or should be utilized in multiple locations.

This file contains definitions that can be used in one or more places in the application,
to allow interoperability in some parts of the application.
*/
#ifndef common_defines
#define common_defines
//! The MIME application signature of the framework.
#define ThemisAppSig "application/x-vnd.becommunity-themis"
//! The MIME type of Themis' cache files.
#define ThemisCacheMIME "application/x-Themis-cache"
//! The MIME type of the 
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
//#define FindCookie 'fcok'
//#define NoCookieFound 'nocf'
//#define FoundCookie 'fndc'
//#define SetCookie 'setc'

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
#define ProtocolStopTransaction 'pstn'

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

// Message Constants for the interface
#define ABOUTWIN_CLOSE			'awcl'
#define BUTTON_BACK				'bbac'
#define BUTTON_FORWARD			'bfwd'
#define BUTTON_STOP				'bstp'
#define BUTTON_RELOAD			'brel'
#define BUTTON_HOME				'bhom'
#define CLEAR_TG_HISTORY		'clgh'
#define CLOSE_OTHER_TABS		'clot'
#define CLOSE_URLPOPUP			'clpu'
#define DTD_CHANGED				'dtdc'
#define DTD_CHANGED_PARSER		'dtdp'
#define PREFSWIN_CLOSE			'pwcl'
#define PREFSWIN_SHOW			'pwsh'
#define RE_INIT_INTERFACE		'riif'
#define SAVE_APP_SETTINGS		'savs'
#define TAB_ADD					'tadd'
#define TAB_CLOSE				'tclo'
#define URL_TYPED				'urlt'
#define URL_LOADING				'urll'
#define URL_OPEN				'urlo'
#define URL_SELECT_MOUSE		'ursm'
#define URL_SELECT_NEXT			'ursn'
#define URL_SELECT_PREV			'ursp'
#define WINDOW_CLOSE			'winc'
#define WINDOW_NEW				'winn'

// UrlHandler message constants
#define UH_LOAD_NEW_PAGE		'ulnp'	// sent by win to UH/all interested
#define UH_LOADING_FINISHED		'ulfi'	// sent by network to UH
#define UH_LOADING_PROGRESS		'ulpg'	// sent by network to UH
#define UH_PARSE_HTML_FINISHED	'uphf'	// sent by HTML parser to UH
#define UH_PARSE_HTML_START		'uphs'	// sent by UH to HTML parser
#define UH_RELOAD_PAGE			'urel'	// sent by win to UH/all interested
#define UH_RENDER_START			'urst'	// sent by UH to renderer
#define UH_RENDER_FINISHED		'urfi'	// snet by renderer to UH
#define UH_WIN_LOADING_PROGRESS	'uwlp'	// sent by UH to windows

#include <GraphicsDefs.h>

union int32torgb
{
	int32 value;
	struct rgb_color rgb;
};
#endif
