/*
Copyright (c) 2003 Z3R0 One. All Rights Reserved.

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

Original Author & Project Manager: Z3R0 One (z3r0_one@bbnk.dhs.org)
Project Start Date: October 18, 2000
*/
/*!
\file
\brief This file contains the declaration of the main new Message System class.

The new message broadcast system is based around a single class &#151 MessageSystem &#151 
and a single administrative class &#151 MessageDaemon.
*/
#ifndef _new_message_system_
#define _new_message_system_

#include <Locker.h>
#include <SupportKit.h>
#include <Message.h>
#include <kernel/OS.h>
#include <MessageQueue.h>

#define ProtocolPlugin 'pplg'
#define ContentPlugin 'cplg'
#define HTMLPlugin 'hplg'
#define ImagePlugin 'iplg'
#define MediaPlugin 'mplg'
#define JavaScriptPlugin 'jspl'
#define BroadcastMessage 'bcmg'
#define BroadcastHandledBy 'bchb'

/*!
\brief The MsgSysTargets enumeration values determine which subsystem(s) receives the
message being broadcast.

These are a number of values that help determine exactly where messages can and will be sent.
There is no guarantee that any particular target or combination of targets is a part of the
broadcast system.
*/
enum MsgSysTargets 
{
	MS_TARGET_ALL=0,//!< Send message to all members of the broadcast system; including the sender.
	MS_TARGET_SELF,//!< Send message back to the sender.
	MS_TARGET_OTHER,//!< Send message to all members of the broadcast system except the sender. (Not yet implemented.)
	MS_TARGET_UNKNOWN='____',//!< This value should never be sent, unless you are testing whether the message system is working; the message will not be sent to any receivers.
	MS_TARGET_CONTENT_IMAGE=0x4,//!< The content of the message or referenced in the message is an image file.
	MS_TARGET_CONTENT_TEXT=0x6,//!< The content of the message or referenced in the message is a text file.
	MS_TARGET_CONTENT_SCRIPT=0x8,//!< The content of the message or referenced in the message is a script of some sort.
	MS_TARGET_CONTENT_AUDIO=0xa,//!< The content of the message or referenced in the message is an audio file.
	MS_TARGET_CONTENT_VIDEO=0xc,//!< The content of the message or referenced in the message is a video file.
	MS_TARGET_CONTENT_COOKIE=0xe,//!< The content of the message or referenced in the message is a cookie of some sort. (Probably HTTP Cookie.)
	MS_TASRGET_CONTENT_DATA=0x10,//!< The content of the message or referenced in it is data of unknown or unspecified type.
	MS_TARGET_CACHE=0x20,//!< Send message to cache members of the broadcast system.
	MS_TARGET_PROTOCOL=0x40,//!< Send message to protocol members of the broadcast system.
	MS_TARGET_HANDLER=0x80,//!< Send message to content handler members of the broadcast system.
	MS_TARGET_PARSER=0x100,//!< Send message to text/data parser members of the broadcast system.
	MS_TARGET_APPLICATION=0x202,//!< Send message to the BApplication object.
	MS_TARGET_RENDERER=0x204,//!< Send message to the renderer object.
	MS_TARGET_TYPE_ALL=0x404,//!< Send message to members that process data on disk or RAM. Useful really only with cache target.
	MS_TARGET_TYPE_DISK=0x406,//!< Send message to members that process data on disk only. Useful really only with cache target.
	MS_TARGET_TYPE_RAM=0x408,//!< Send message to members that process data in RAM only. Useful really only with cache target.
	MS_TARGET_PROTO_HTTP=0x804,//!< Send message to the HTTP Protocol object.
	MS_TARGET_PROTO_FILE=0x806,//!< Send message to the FILE Protocol object.
	MS_TARGET_PROTO_FTP=0x808,//!< Send message to the FTP Protocol object.
	MS_TARGET_HTTP_PROTOCOL=(MS_TARGET_PROTO_HTTP|MS_TARGET_PROTOCOL),//!< More correct way to target the HTTP protocol in a message.
	
	MS_TARGET_CACHE_SYSTEM=0x1004,//!< More correct way to target the cache system object.
	MS_TARGET_HTML_PARSER=(0x1006|MS_TARGET_PARSER),//!< Send a message to the HTML parser object.
	MS_TARGET_CSS_PARSER=(0x1008|MS_TARGET_PARSER),//!< Send a message to the CSS parser object.
	MS_TARGET_DOM=0x100a,//!< Send a message to the Document Object Model system.
	MS_TARGET_DOM_VIEWER=0x100c,//!< Send a message to the DOM viewer add-on.
	MS_TARGET_JAVASCRIPT_HANDLER=0x100e,//!< Send a message to the Javascript handler.
	MS_TARGET_IMAGE_HANDLER=(0x1012|MS_TARGET_HANDLER),//!< Send a message to the image handler.
	MS_TARGET_COOKIE_MANAGER=0x1014, //!< Send a message to the cookie management system.
	MS_TARGET_TAB_SYSTEM_TOP=0x1016, //!< Send a message to the tabbed view system.
	MS_TARGET_TAB_SYSTEM_TAB=0x1018,//!< Send a message to a specific tab in the tabbed view system.
	MS_TARGET_WINDOW=0x101a,//!< Send a message to a specific window.
	MS_TARGET_MEMORY_CACHE=MS_TARGET_CACHE|MS_TARGET_TYPE_RAM, //!< Send a message to the cache system for a RAM cache object.
	MS_TARGET_DISK_CACHE=MS_TARGET_CACHE|MS_TARGET_TYPE_DISK, //!< Send a message to the cache system for a disk cache object.
	MS_TARGET_PLUG_IN_MANAGER='plgm' //!< Send a message to the plug-in manager.
};
class MessageDaemon;
/*!
\brief The main class in the message broadcast system.

In order to make any object a member of the message broadcast system, it is necessary to
make it a derived child of the MessageSystem class, directly or indirectly. All Themis
plug-ins which derive from PlugClass are automatically included in the broadcast system as
PlugClass is now a child of the MessageSystem class.

Please be aware of several facts regarding the broadcast system.
\li Each member of the message broadcast system adds a new thread to the application as a whole. This
does not increase complexity in thread management, but it does lower system resources.
\li Add-ons/plug-ins incorporating the message system (which should be all) need to be linked against _APP_
which is a symbolic link to the Themis framework application. Failing to do so will allow the add-on to function
in general, but it will <b>not</b> be a member of the broadcast system.
\li All objects which that will be participating in the message system also must call MsgSysRegister() on start-up
and MsgSysUnregister() at shutdown. Classes derived from PlugClass don't need to worry about this as it's done in
PlugClass's constructor and destructor.
*/
class MessageSystem {
	private:
/*!
\brief The number of members in the broadcast system.

This variable contains the current number of registered broadcast system members.
*/
		static volatile int32 broadcast_target_count;
/*!
\brief A linked list that records pointers to members of the broadcast system.

This is a linked list structure that records pointers to registered members of the broadcast system.
This structure is a private member of the class to prevent other parts of the application
or add-ons from attempting to add or remove members of the list inappropriately. Lets hear
it for encapsulation!!
*/
		struct msgsysclient_st {
			MessageSystem *ptr;//!< Pointer to the MessageSystem member.
			msgsysclient_st *next;//!< Next member in the linked list.
		};
/*!
\brief Pointer to the first member in the msgsysclient_st linked list.

This pointer points to the first member of the msgsysclient linked list.
*/
		static msgsysclient_st *MsgSysClients;
/*!
\brief System locking object.

*/
		static BLocker msgsyslock;
/*!
\brief Message transmission semaphore.

This semaphore is used to permit only a single message into the broadcast system at a time.
*/
		static sem_id transmit_sem;
/*!
\brief Broadcast processing semaphore.

This semaphore helps prevent multiple messages from entering into the broadcast system
simultaneously. In particular, this semaphore is acquired when the messages are being added
to the target(s)'s message queue.
*/
		static sem_id process_sem;
/*!
\brief Quits the message system main thread.


*/
		static volatile int32 _Quit_Thread_;
/*!
\brief The main thread function of the broadcast system.

This is where messages are officially received and processed by the broadcast system. It is
here that the messages are sent to the message queues of the targets, and the individual
message queue processing threads are awakened to do their work.
*/
		static int32 _ProcessBroadcasts_(void *data);
/*!
\brief The thread identifier for the message system main thread.

*/
		static thread_id _ProcessThread_;
/*!
\brief The number of messages sent since the broadcast system was started.

*/
		static volatile uint32 _messages_sent_;
/*!
\brief The number of targets that received messages through the broadcast system.

*/
		static volatile uint32 _message_targets_;
/*!
\brief Gives full access to an administrative class.

By making the MessageDaemon class a friend, MessageDaemon has full access to all of the
public and private members of the MessageSystem. If necessary (though not) MessageDaemon could
tamper with the linked list or do some other funky things.

However, the only thing that MessageDaemon really does is start and stop the MessageSystem,
and print out the statistics at shutdown.
*/
		friend class MessageDaemon;
/*!
\brief This function starts the message system queue thread for each derived object.

*/
		static int32 MS_Start_Thread(void *arg);
	protected:
/*!
\brief This is the status returned after a message has been broadcast to all targets.

\note While this value is present, it is currently meaningless. The broadcast system was
adapted to utilize a queue for messages, so there is no telling when a message will be
processed by the target; thus no telling whether or not the target actually understands the
message currently.
*/
		volatile status_t broadcast_status_code;
/*!
\brief This number represents the number of targets reached with a broadcast message.

This is the number of targets that received the most recently broadcast message.
*/
		volatile int32 broadcast_successful_receives;
/*!
\brief Yet another semaphore.

*/
		sem_id _msg_receiver_sem_;
/*!
\brief This keeps track of messages received before they've been processed by the receiver.

*/
		BMessageQueue _message_queue_;
/*!
\brief 

*/
		thread_id _msg_receiver_thread_;
/*!
\brief 

*/
		volatile int32 _ms_receiver_quit_;
/*!
\brief 

*/
		volatile int32 _msg_receiver_running_;
/*!
\brief 

*/
		int32 _ProcessMessage_(void *arg);
	public:
/*!
\brief 

*/
		MessageSystem(char *msg_sys_name=NULL);
		char *MS_Name;
/*!
\brief 

*/
		virtual ~MessageSystem();
/*!
\brief Unregister from the message system.

This function removes the target from the message system linked list; the target will no
longer receive messages from the broadcast system. This should generally only be called
by the object requesting to removal from the list (MsgSysUnregister(this);). Classes derived
from PlugClass do not need to call this funciton as it is done as part of the PlugClass definition.
*/
		void MsgSysUnregister(MessageSystem *target);
/*!
\brief Register with the message system.

This function adds the target to the message system linked list; the target will start
to receive messages after this function returns. This should generally only be called by the
object requesting to be added to the list (MsgSysRegister(this);). Classes derived from PlugClass
do not need to call this function as it is done as part of the PlugClass definition.
*/
		void MsgSysRegister(MessageSystem *target);
/*!
\brief Send a message to one or more targets in the message system.

This function allows any member of the message system to send a message to one or more members
of the message system, including itself. Unlike the old message system function of the same name,
each class derived from MessageSystem will have this function, and the message being sent no longer
needs to be in a container message. The message should still contain an uint32 value "command" like
the previous system did however.
*/
		void Broadcast(uint32 targets,BMessage *msg);
/*!
\brief Process and act upon the received broadcast message.

This function is called by the queue processing thread to actually handle the received
broadcast message.
*/
		virtual status_t ReceiveBroadcast(BMessage *msg);
/*!
\brief Secondary message response function.

Sometimes, a broadcast message will trigger a response from another part of the application. If this response
depends on sending data back to the original broadcaster, it should be done by either broadcasting a message
specifically to the originator, or (more appropriately) by referencing the pointer to the originator
and calling this function.
*/
		virtual status_t BroadcastReply(BMessage *msg);
/*!
\brief The member's broadcast target identifier value.

*/
		virtual uint32 BroadcastTarget();
/*!
\brief This notifies the sender of a broadcast that the message system has finished sending the message to the desired targets.


*/
		virtual void BroadcastFinished();
};


#endif

