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
\brief Contains important definitions and a class for creating and utilizing plug-ins.
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
#include <Locker.h>
#include <Entry.h>
#include <OS.h>
#include <MenuBar.h>
#include "msgsystem.h"


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
#define TARGET_APPLICATION '_app'
#define TARGET_PLUGMAN '_plm'
#define TARGET_RENDERER 'rndr'
#define CONTENT_IMAGE	0x4
#define CONTENT_TEXT	0x6
#define CONTENT_SCRIPT	0x8
#define CONTENT_AUDIO	0xa
#define CONTENT_VIDEO	0xc
#define CONTENT_COOKIE	0xe
#define CONTENT_DATA	0x10
#define TYPE_ALL 0x404
#define TYPE_DISK	0x406
#define TYPE_RAM	0x408
#define TYPE_DISK_FILE	0x40a
#define PROTO_HTTP	0x804
#define PROTO_FTP	0x806
#define PROTO_SMTP	0x808
#define PROTO_POP	0x80a
#define PROTO_NNTP	0x80c
#define SCRIPT_JAVASCRIPT 0x1002
#define SCRIPT_VBSCRIPT 0x1004
#define SCRIPT_ACTIVEX 0x1006
#define TEXT_HTML 0x2002
#define TEXT_CSS 0x2004
#define TEXT_PLAIN 0x2006

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
#define JAVASCRIPT_HANDLER TARGET_HANDLER|CONTENT_SCRIPT|SCRIPT_JAVASCRIPT
//handler for javascript scripts embedded in html files
#define HTML_PARSER TARGET_PARSER|CONTENT_TEXT|TEXT_HTML
//handler that displays HTML text files
#define TEXT_HANDLER TARGET_HANDLER|CONTENT_TEXT|TEXT_PLAIN
//Handler that displays text files that are not html
#define CSS_PARSER TARGET_PARSER|CONTENT_TEXT|TEXT_CSS
#define RENDER_ENGINE TARGET_HANDLER|TARGET_RENDERER

#define PLUG_DOESNT_HANDLE 'pdnh'
//Plug-in doesn't handle the type of message sent to it, or the type of data specified
//in the message.
#define PLUG_HANDLE_GOOD 'phgd'
//The plug-in can handle the message/data sent.
#define PLUG_REPLY 'plrp'
#define PLUG_REPLY_RECEIVED 'prrc'
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
int32 strtoval(char *proto); //plug-in identifier converter 4 char string to int32
class plugman;
//!The base class for all plug-ins.
/*!
PlugClass is the base class for all of the plug-ins that Themis can and will utilize.
Some plug-ins will use this class directly, others will probably use a derived verison.
*/
class PlugClass: public MessageSystem {
	private:
	public:
	//!PlugClass constructor.
	/*!
	This constructor takes a BMessage object that contains various tidbits that are
	useful to all plug-ins, including pointers to the settings BMessage.
	*/
		PlugClass(BMessage *info=NULL,char *msg_sys_name="Plug-in");
		//!Initialization information container.
		BMessage *InitInfo;
		//!The destructor.
		virtual ~PlugClass();
		
		volatile int32 Cancel;
		//!The plug-in's unique 32-bit idenitfier.
		/*!
		The PlugID() function should return a unique 32-bit identifier that identifies
		the plug-in. The identifier, or value, can be either a pure integer value or
		it could be done as a character constant such as 'abcd' or '_hlo'.
		*/
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
		//!Returns the plug-in's descriptive name.
		/*!
		This function should return the name of the plug-in as the author specified.
		Although it is not it is only meant for descriptive purposes, it's a good idea
		to keep this name to 20-30 characters or less.
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
		//!This is a reference to the Plug-in Manager. 
		plugman *PlugMan;
		
		virtual int32 TypePrimary();
		virtual int32 TypeSecondary();
		
		//!Is the plug-in a BHandler derived plug-in?
		/*!
		\retval true Your plug-in is also derived from BHandler.
		\retval false Your plug-in does not derive from BHandler.
		
		Plug-ins by default do not derive from BHandler, and so the default value
		is false. If you derive from BHandler, be sure to override this function
		and return true; otherwise your function will not be added as a handler
		to the Plug-in Manager.
		
		This is also applicable if your plug-in simply creates a BHandler object
		that needs to be added to the Plug-in Manager.
		*/
		virtual bool IsHandler();//if the plugin has a BHandler object instead
		//!Returns a pointer to this plug-in's BHandler object.
		/*!
		If your plug-in has a BHandler object that needs to be added to the Plug-in
		Manager, or if its PlugClass derived class also inherits from BHandler, you
		will need to return a pointer to it here.
		\retval NULL The default return value.
		\retval "BHandler *" A pointer to a BHandler object. ("this" is valid if the class is derived from BHandler.)
		*/
		virtual BHandler *Handler();
		
		//!Should the plug-in stay in memory when not needed or be unloaded?
		/*!
		\retval true The plug-in is persistant and should stay in RAM even when not needed.
		\retval false The plug-in should be unloaded when not needed. (default)
		*/
		virtual bool IsPersistent();//does the plugin load and unload based on page?
		
		//!Is the plug-in derived from BLooper?
		/*!
		\retval true The plug-in is derived from or utilizes a BLooper that is accessible via the PlugClass.
		\retval false The plug-in is not derived from BLooper or does not have an accessible BLooper.
		*/
		virtual bool IsLooper();
		//!Returns a pointer to the BLooper object if applicable.
		virtual BLooper *Looper();
		//!Starts a BLooper if applicable.
		virtual void Run();
		
		virtual bool IsView();
		virtual BView *View();
		virtual BView *Parent();
		//!Sets the entry_ref for the plug-in.
		entry_ref SetRef(entry_ref nuref);
		//!Returns the entry_ref for the plug-in.
		entry_ref Ref();
		//!Internal entry_ref structure that points to the plug-in.
		entry_ref plug_ref;//an entry for the plugin
		
		virtual void AddMenuItems(BMenu *menu);//this is a signal for plug-ins to add any menus or items
		virtual void RemoveMenuItems();
		
		bool uses_heartbeat;
		virtual void Heartbeat();
		virtual bool RequiresHeartbeat();

		virtual status_t ReceiveBroadcast(BMessage *msg)=0;
		virtual status_t BroadcastReply(BMessage *msg)=0;
		virtual uint32 BroadcastTarget()=0;
	
		virtual int32 Type();
		
		//The following functions return NULL unless overridden. See each description
		//for more details on the functionality.
		
		virtual char *AboutViewLabel();//name as it should appear in the About window list
		virtual BView *AboutView();
			/*
				The top level view to add to the about window. If no view is returned,
				basic information (PlugName(), PlugID(), and PlugVersion()) will
				be used to generate a simple about view.
			*/
		virtual char *SettingsViewLabel();//name as it should appear in the settings list
		virtual BView *SettingsView();
			/*
				The top level view to add to the settings window. Be sure to SetTarget()
				any controls to this view in either the constructor, AllAttached(), or
				AttachedToWindow(). Otherwise your view will not receive messages for
				it's controls and widgets! AttachedToWindow() or AllAttached() is the
				best place to do this!
			*/
		BLocker *Lock;
	
};

#endif 
