/*
Copyright (c) 2001 Z3R0 One. All Rights Reserved.

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



