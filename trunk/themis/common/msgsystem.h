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


enum MsgSysTargets 
{
	MS_TARGET_ALL=0,
	MS_TARGET_SELF,
	MS_TARGET_OTHER,
	MS_TARGET_UNKNOWN='____',
	MS_TARGET_CONTENT_IMAGE=0x4,
	MS_TARGET_CONTENT_TEXT=0x6,
	MS_TARGET_CONTENT_SCRIPT=0x8,
	MS_TARGET_CONTENT_AUDIO=0xa,
	MS_TARGET_CONTENT_VIDEO=0xc,
	MS_TARGET_CONTENT_COOKIE=0xe,
	MS_TASRGET_CONTENT_DATA=0x10,
	MS_TARGET_CACHE=0x20,
	MS_TARGET_PROTOCOL=0x40,
	MS_TARGET_HANDLER=0x80,
	MS_TARGET_PARSER=0x100,
	MS_TARGET_APPLICATION=0x202,
	MS_TARGET_RENDERER=0x204,
	MS_TARGET_TYPE_ALL=0x404,
	MS_TARGET_TYPE_DISK=0x406,
	MS_TARGET_TYPE_RAM=0x408,
	MS_TARGET_PROTO_HTTP=0x804,
	MS_TARGET_HTTP_PROTOCOL=(MS_TARGET_PROTO_HTTP|MS_TARGET_PROTOCOL),
	MS_TARGET_CACHE_SYSTEM=0x1004,
	MS_TARGET_HTML_PARSER=(0x1006|MS_TARGET_PARSER),
	MS_TARGET_CSS_PARSER=(0x1008|MS_TARGET_PARSER),
	MS_TARGET_DOM=0x100a,
	MS_TARGET_DOM_VIEWER=0x100c,
	MS_TARGET_JAVASCRIPT_HANDLER=0x100e,
	MS_TARGET_IMAGE_HANDLER=(0x1012|MS_TARGET_HANDLER),
	MS_TARGET_COOKIE_MANAGER=0x1014,
	MS_TARGET_TAB_SYSTEM_TOP=0x1016,
	MS_TARGET_TAB_SYSTEM_TAB=0x1018,
	MS_TARGET_WINDOW=0x101a,
	MS_TARGET_MEMORY_CACHE=MS_TARGET_CACHE|MS_TARGET_TYPE_RAM,
	MS_TARGET_DISK_CACHE=MS_TARGET_CACHE|MS_TARGET_TYPE_DISK,
};
class MessageDaemon;
class MessageSystem {
	private:
		static volatile int32 broadcast_target_count;
		struct msgsysclient_st {
			MessageSystem *ptr;
			msgsysclient_st *next;
		};
		static msgsysclient_st *MsgSysClients;
		static BLocker *msgsyslock;
		static sem_id transmit_sem;
		static sem_id process_sem;
		static volatile int32 _Quit_Thread_;
		static int32 _ProcessBroadcasts_(void *data);
		static thread_id _ProcessThread_;
		static volatile uint32 _messages_sent_;
		static volatile uint32 _message_targets_;
		friend class MessageDaemon;
		static int32 MS_Start_Thread(void *arg);
	protected:
		volatile status_t broadcast_status_code;
		volatile int32 broadcast_successful_receives;
		sem_id _msg_receiver_sem_;
		BMessageQueue *_message_queue_;
		thread_id _msg_receiver_thread_;
		volatile int32 _ms_receiver_quit_;
		volatile int32 _msg_receiver_running_;
		int32 _ProcessMessage_(void *arg);
	public:
		MessageSystem();
		virtual ~MessageSystem();
		void MsgSysUnregister(MessageSystem *target);
		void MsgSysRegister(MessageSystem *target);
		void Broadcast(uint32 targets,BMessage *msg);
		virtual status_t ReceiveBroadcast(BMessage *msg);
		virtual status_t BroadcastReply(BMessage *msg);
		virtual uint32 BroadcastTarget();

};


#endif

