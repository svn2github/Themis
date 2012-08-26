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

#define DEBUG_INFO_MSG MessageSystem::DEBUG_INFO_MSG

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
#define CSS_CHANGED				'cssc'
#define CSS_CHANGED_PARSER		'cssp'
#define PREFSWIN_CLOSE			'pwcl'
#define PREFSWIN_SHOW			'pwsh'
#define RE_INIT_TABHISTORY		'rith'
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
#define SET_NAV_URL				'snvu'

// UrlHandler message constants
//#define UH_DOC_CLOSED			'udcl'	// sent by tab to UH (removes UrlEntry)
//#define UH_LOAD_NEW_PAGE		'ulnp'	// sent by win to UH/all interested
//#define UH_LOADING_FINISHED		'ulfi'	// sent by network to UH
//#define UH_LOADING_PROGRESS		'ulpg'	// sent by network to UH
//#define UH_PARSE_DOC_FINISHED	'updf'	// sent by HTML parser to UH
//#define UH_PARSE_DOC_START		'upds'	// sent by UH to HTML parser
//#define UH_RELOAD_PAGE			'urel'	// sent by win to UH/all interested
//#define UH_RENDER_START			'urst'	// sent by UH to renderer
//#define UH_RENDER_FINISHED		'urfi'	// snet by renderer to UH
//#define UH_WIN_LOADING_PROGRESS	'uwlp'	// sent by UH to windows

// SiteHandler message constants
#define SH_DOC_CLOSED			'sdcl'	// sent by win to SH
#define SH_LOAD_NEW_PAGE		'slnp'	// sent by win to SH
#define SH_LOADING_PROGRESS		'slpg'	// sent by network to SH
#define SH_PARSE_DOC_FINISHED	'spdf'	// sent by parser(s) to SH
#define SH_PARSE_DOC_START		'spds'	// sent by SH to parser(s)
#define SH_RELOAD_PAGE			'srel'	// sent by win to SH
#define SH_RENDER_FINISHED		'srfi'	// sent by renderer to SH
#define SH_RENDER_START			'srst'	// sent by SH to renderer
#define SH_RETRIEVE_START		'srst'	// sent by SH to protocol(s)
#define SH_RETRIEVE_STOP		'srsp'	// sent by SH to protocol(s)
#define SH_WIN_LOADING_PROGRESS	'swlp'	// sent by SH to win

// additional interface stuff
const float kPrefsWinWidth	= 600.0;
const float kPrefsWinHeight	= 440.0;
const float kItemSpacing	= 10.0;
const float kBBoxExtraInset	= 5.0;
const float kDividerSpacing = 5.0;
const char* const kAboutBlankPage = "about:blank";
const char* const kAboutThemisPage = "about:themis";
const char* const kNoDTDFoundString = "none";
const char* const kNoCSSFoundString = "none";
// colors which can't be grepped from the system
#include <GraphicsDefs.h>
const rgb_color kColorInactiveTab = { 184, 184, 184, 255 };
const rgb_color kColorDarkBorder = { 51, 51, 51, 255 };
const rgb_color kColorLightBorder = { 187, 187, 187, 255 };
const rgb_color kColorShadow = { 240, 240, 240, 255 };
const rgb_color kColorTheme = { 255, 200, 0, 255 };

//ImageHandler message constants
#define IH_LOAD_IMAGE			'ihli'
#define IH_IMAGE_LOADED			'ihil'

//HTTPv4 Specific message contants
//You should only send these to MS_TARGET_PROTOCOL or specifically to the HTTP plugin
//as what values.
#define HTTP_POST_REQUEST		'post'
#define HTTP_PUT_REQUEST		'_put'
#define HTTP_GET_REQUEST		'_get'
#define HTTP_DELETE_REQUEST		'dele'
#define HTTP_HEAD_REQUEST		'head'

//commands that can be sent to the plug-ins
/*
	Exact behavior of a command depends on the type of plug-in the message is sent to.
	Notes follow each command.
	Commands are sent in the included message (the sub-BMessage) in a int32 field
	called command. 
*/

#define COMMAND_RETRIEVE 0x100
/*
	Protocols:	This will trigger the protocol to look for a URL string in the BMessage.
				If there is no specific URL listed, the protocol then looks for host,
				port, and path to try to accomplish its mission.
	Cache:		This command will tell the cache plug-in(s) to look for a file that
				matches the URL (or host, port, and path) contained in the message,
				and create a reference variable to the file/object. All relevant
				information available to the cache should be sent as part of the response.
*/
#define COMMAND_STORE 0x101
/*
	Protocols:	This will trigger the protocol to send data to the enclosed URL.
				If the protocol supports more than one sending method, then
				the message should also include a "STORE_METHOD" string to specify
				the actual method to be used by the protocol. Variables and/or data
				to be sent should have their names stored in the "VARIABLES" string.
				Be sure to include a length variable for any pointers:
				An unsigned char pointer might be stored in the message as follows:
						msg->AddString("VARIABLES","buffer");
						msg->AddPointer("buffer",buff);
						msg->AddInt32("buffer",buff_len);
	Cache:		The cache plug-ins should only receive a store command from a protocol
				plug-in. Depending on whether the protocol needs disk or RAM storage,
				the cache should create a record of the data being sent by the protocol
				for retrieval.All relevant information should be sent as part of the
				message.
*/
#define COMMAND_SEND 0x102
/*
	Protocols:	This should be just a raw send by the protocol. This might be something
				as simple as a notification message, or uploading a file.
*/
#define COMMAND_CLEAR 0x103
/*
	Protocols:	This should close any and all open requests, and delete/free all
				transaction records, unless a specific URL is indicated. If a specific
				URL is indicated, only requests for that URL should be cleared.
	Cache:		This should clear the cache. If a particular URL is specified, then only
				the records for that URL should be cleared.
	Parsers:	The parser's buffers should be emptied and cleared.
	Handlers:	The handler's buffers should be emptied and cleared.
	Scripts:	The script handler should empty its buffers, contexts, etc, and clear them.
*/
#define COMMAND_UPDATE 0x104
/*
	Cache:		This should only be called by a protocol. There is new information to be
				recorded about the specied URL. All relevant information should be sent
				as part of the message.
	Parsers:	Should update their buffers with the information sent.
	Handlers:	Should first clear their buffers, and then use the information sent.
				This is different from parsing as a parser knows specifics about the
				data, where as a handler (more or less) just displays the data.
	Scripts:	Should update their buffers with the information sent.
*/
#define COMMAND_PROCESS 0x105
/*
	Parsers:	Should handle the sent data appropriately.
	Handlers:	Should display the data sent appropriately.
	Scripts:	Should process the data appropriately.
*/
#define COMMAND_INFO 0x106
/*
	This command can be sent by any part of the application and any part that receives
	it should behave according to its own function. This message can be ignored, though
	it probably won't always be.
*/
#define COMMAND_INFO_REQUEST 0x107
/*
	This command is a request for information by some part of the application and should
	not be ignored. Responders should look for a "ReplyTo" Int32 target in the BMessage it
	receives, and Broadcast a reply back to that target.

	This command should always be replied to with a COMMAND_INFO command.
*/
#define COMMAND_ASSUME_RESPONSIBILITY 0x108
/*
	This command means that the sender of the message will assume responsibility for
	the payload of the original message. In other words, it should be sent as a reply
	to a broadcast message, such as when a file is transferred by a protocol, a handler
	will assume control over the request's existance. The originator of this
	sequence of events should receive a "Owner" in the message that assumes responsibility.
	It should then only release the resource at termination of the application/plug-in,
	or upon COMMAND_RELEASE_RESOURCE. "Owner" should be the 32 bit integer PlugID value
	of the plug-in. The DOM, Window, and View should return their Type()/TARGET_* values.
	If the DOM, Window, or View should be set as the owner (unlikely), then the
	receiver of the broadcast should use its stored pointer(s) to these objects.
*/
#define COMMAND_RELEASE_RESOURCE 0x109
/*
	This command tells a resource controller, to delete the resource item specified.
	Typically, there should be a URL specified in the BMessage to refer to the specific
	item. By default, the resource controller should assume this message means delete
	everything in memory regarding the specified resource. If the message contains
	a boolean value "save-resource", all of the resource should be released, except the
	actual pointer to the resource. If the message contains a boolean value "all", it should
	release all resources at its disposal, with the possible exception of the resource pointers.
	(This last part should only be done if "save-resource" is also specified.)

	For example, the HTTP protocol receives a COMMAND_RELEASE_RESOURCE message. It should
	delete the appropriate request structure and data container object for the specified
	URL. If no URL is specified, but a boolean value of "all" is present and true, then
	the HTTP layer should clear all of its requests. If "save-resource" is specified and
	true, then only the structure data regarding the target URL(s) is deleted, the 
	data container objects are not.

			**** NOTE ****

	Be aware that using the "save-resource" function can result in a memory leak if
	the data containers are not later deleted by the object sending the COMMAND_RELEASE_RESOURCE
	message.
*/

//union int32torgb
//{
//	int32 value;
//	struct rgb_color rgb;
//};
#endif
