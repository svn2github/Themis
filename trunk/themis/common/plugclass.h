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

/*
Include *both* plugclass.h *and* plugclass.cpp in your plugin! If necessary,
add a command to makelinks.sh to create this link in your plug-in's directory.
*/

#ifndef _PlugClass
#define _PlugClass

#include <Window.h>
#include <View.h>
#include <Looper.h>
#include <Handler.h>
#include <Message.h>
#include <Entry.h>
#include <OS.h>
#include <MenuBar.h>

#define ProtocolPlugin 'pplg'
#define ContentPlugin 'cplg'
#define HTMLPlugin 'hplg'
#define ImagePlugin 'iplg'
#define MediaPlugin 'mplg'

#define BroadcastMessage 'bcmg'
#define BroadcastHandledBy 'bchb'

/*
	Target values for message broadcasting
*/
#define ALL_TARGETS 0x0
#define TARGET_CACHE	0x20
#define TARGET_PROTOCOL	0x40
#define TARGET_HANDLER	0x80
#define TARGET_PARSER	0x100
#define TARGET_DOM	'_dom'
#define TARGET_WINDOW '_wnd'
#define TARGET_VIEW '_vue'
#define CONTENT_IMAGE	0x4
#define CONTENT_TEXT	0x6
#define CONTENT_SCRIPT	0x8
#define CONTENT_AUDIO	0xa
#define CONTENT_VIDEO	0xc
#define CONTENT_COOKIE	0xe
#define CONTENT_DATA	0x10
#define TYPE_DISK	0x404
#define TYPE_RAM	0x406
#define PROTO_HTTP	0x804
#define PROTO_FTP	0x806
#define PROTO_SMTP	0x808
#define PROTO_POP	0x80a
#define PROTO_NNTP	0x80c

#define MEMORY_CACHE TARGET_CACHE|TYPE_RAM
//memory/ram cache
#define DISK_CACHE TARGET_CACHE|TYPE_DISK
//disk cache
#define HTTP_PROTOCOL TARGET_PROTOCOL|PROTO_HTTP
//http protocol
#define IMAGE_HANDLER TARGET_HANDLER|CONTENT_IMAGE
//handler for images
#define AUDIO_HANDLER TARGET_HANDLER|CONTENT_AUDIO
//handler for audio
#define VIDEO_HANDLER TARGET_HANDLER|CONTENT_VIDEO
//handler for video
#define COOKIE_HANDLER TARGET_HANDLER|CONTENT_COOKIE
//handler for cookie information transmitted in http protocol
#define JAVASCRIPT_HANDLER TARGET_HANDLER|CONTENT_SCRIPT
//handler for javascript scripts embedded in html files
#define HTML_PARSER TARGET_PARSER|CONTENT_TEXT
//handler that displays HTML text files
#define TEXT_HANDLER TARGET_HANDLER|CONTENT_TEXT
//Handler that displays text files that are not html

#define PLUG_DOESNT_HANDLE 'pdnh'
//Plug-in doesn't handle the type of message sent to it, or the type of data specified
//in the message.
#define PLUG_HANDLE_GOOD 'phgd'
//The plug-in can handle the message/data sent.

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
int32 strtoval(char *proto); //plug-in identifier converter 4 char string to int32

class PlugClass {
	private:
	public:
		PlugClass(BMessage *info=NULL);
		BMessage *InitInfo;
		virtual ~PlugClass();
		
		volatile int32 Cancel;
		virtual uint32 PlugID();
		virtual uint32 SecondaryID();
		/*
		plugid is the plugin's individual id, it's a 4 char constant #define'd.
		For example:
		#define JavaPlugin 'java'
		#define OpenSSLPlugin 'ossl'
		Some types of plugins might have a secondary ID number. In these cases, the
		primary ID number (PlugID) indicates the category of the plugin, and the
		secondary indicates a specific item. For example:
		
		HTML Plugins should return 'html' as the PlugID and a unique identifier
		  for each tag; the "<HTML>" tag plugin might return 'html' for both, while
		  the "<TITLE>" tag plugin might return 'html' and 'titl'.
		*/
		virtual char *PlugName();//returns a pointer to a string constant in each plugin
		virtual float PlugVersion();//returns the plugin version, if applicable
		
		virtual bool NeedsThread();
		virtual int32 SpawnThread(BMessage *info);
		virtual int32 StartThread();
		virtual thread_id Thread();
		virtual void Stop();
		thread_id thread;
		
		BWindow *Window;
		BLooper *PlugMan;
		
		virtual int32 TypePrimary();
		virtual int32 TypeSecondary();
		
		virtual bool IsHandler();//if the plugin has a BHandler object instead
		virtual BHandler *Handler();
		
		virtual bool IsPersistant();//does the plugin load and unload based on page?
		
		virtual bool IsLooper();
		virtual BLooper *Looper();
		virtual void Run();
		
		virtual bool IsView();
		virtual BView *View();
		virtual BView *Parent();
		
		entry_ref *SetRef(entry_ref *nuref);
		entry_ref *Ref();
		entry_ref *ref;//an entry for the plugin
		
		virtual void AddMenuItems(BMenu *menu);//this is a signal for plug-ins to add any menus or items
		virtual void RemoveMenuItems();
		
		bool uses_heartbeat;
		virtual void Heartbeat();
		virtual bool RequiresHeartbeat();

		virtual status_t ReceiveBroadcast(BMessage *msg);
		virtual int32 Type();
};

#endif 
