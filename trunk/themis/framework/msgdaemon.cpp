#include "msgdaemon.h"
#include <Autolock.h>
MessageDaemon::MessageDaemon():MessageSystem() {
	msgsyslock=new BLocker(true);
	Start();
}

MessageDaemon::~MessageDaemon() {
	Stop();
	printf("~MessageSystem(): Clients: %ld\t%p\n",broadcast_target_count,MsgSysClients);
	printf("Message System Statistics:\n==========================\nMessages Sent: %lu\nMessage Targets: %lu\n",_messages_sent_,_message_targets_);
	printf("==========================\n");
	fflush(stdout);
	delete msgsyslock;
}
void MessageDaemon::Start() 
{
	BAutolock alock(msgsyslock);
	if (alock.IsLocked()) {
		process_sem=create_sem(0,"broadcast_process_sem");
		transmit_sem=create_sem(1,"broadcast_transmit_sem");
		_ProcessThread_=spawn_thread(_ProcessBroadcasts_,"broadcast_system_thread",B_LOW_PRIORITY,NULL);
		resume_thread(_ProcessThread_);
	}
}

void MessageDaemon::Stop()
{
	BAutolock alock(msgsyslock);
	if (alock.IsLocked()) {
		atomic_add(&_Quit_Thread_,1);
		release_sem(process_sem);
		release_sem(transmit_sem);
		status_t stat;
		wait_for_thread(_ProcessThread_,&stat);
		delete_sem(process_sem);
		delete_sem(transmit_sem);
	}
}



