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

Original Author & Project Manager: Z3R0 One (z3r0_one@yahoo.com)
Project Start Date: October 18, 2000
*/
#include "msgsystem.h"
#include "plugclass.h"
#include <Autolock.h>
#include <stdio.h>
#include <OS.h>
volatile int32 MessageSystem::broadcast_target_count=0;
MessageSystem::msgsysclient_st *MessageSystem::MsgSysClients=NULL;
BLocker MessageSystem::msgsyslock;
sem_id MessageSystem::process_sem=0;
sem_id MessageSystem::transmit_sem=0;
volatile int32 MessageSystem::_Quit_Thread_=0;
thread_id MessageSystem::_ProcessThread_=0;
volatile uint32 MessageSystem::_messages_sent_=0;
volatile uint32 MessageSystem::_message_targets_=0;
MessageSystem::MessageSystem(const char *msg_sys_name)
{
	MS_Name=msg_sys_name;
	if (MS_Name==NULL)
		MS_Name="MessageSystem member";
	broadcast_status_code=B_NO_ERROR;
//	_message_queue_=new BMessageQueue();
	_msg_receiver_sem_=create_sem(0,"message_receiver_sem");
	_ms_receiver_quit_=0;
	_msg_receiver_running_=0;
	_msg_receiver_thread_=0;
	
//	_msg_receiver_thread_=spawn_thread(MS_Start_Thread,MS_Name,B_LOW_PRIORITY,this);
//	resume_thread(_msg_receiver_thread_);
}

MessageSystem::~MessageSystem()
{
	BAutolock alock(msgsyslock);
	if (alock.IsLocked()) {
		_ms_receiver_quit_=1;
		release_sem(_msg_receiver_sem_);
		delete_sem(_msg_receiver_sem_);
//		delete _message_queue_;
		status_t stat;
		wait_for_thread(_msg_receiver_thread_,&stat);
	}
}
int32 MessageSystem::MS_Start_Thread(void *arg) 
{
	MessageSystem *obj=(MessageSystem*)arg;
	int32 retval=0;
	obj->_ProcessMessage_(obj);
	wait_for_thread(obj->_msg_receiver_thread_,(status_t *)&retval);
//	printf("thread returned: %ld\n",retval);
	
	return retval;
	
}
const char * MessageSystem::MsgSysObjectName() {
	return MS_Name;
}
void MessageSystem::Debug(const char *message, int32 plugid) {
	BMessage debug(DEBUG_INFO_MSG);
	debug.AddInt32( "command", COMMAND_INFO );
	if (plugid!=-1)
		debug.AddInt32( "pluginID", plugid );
	debug.AddString("message",message);
	Broadcast(MS_TARGET_DEBUG,&debug);
}
int32 MessageSystem::_ProcessBroadcasts_(void *data) 
{
	while (!_Quit_Thread_) {
		if (acquire_sem(process_sem)==B_OK) {
#ifdef DEBUG
			printf("MessageSystem::_ProcessBroadcasts_\n");
#endif
			if (has_data(_ProcessThread_)) {
//				BMessage *msg;
				thread_id sender_thread;
				BMessage *container=NULL;
				uint32 targets=0;
				uint32 sender_target_id=0;
				MessageSystem *broadcaster=NULL;
				receive_data(&sender_thread,&container,sizeof(&container));
			release_sem(transmit_sem);
				container->FindPointer("broadcaster_pointer",(void**)&broadcaster);
				container->FindInt32("broadcast_targets",(int32*)&targets);
				container->FindInt32("broadcaster_target_id",(int32*)&sender_target_id);
//				printf("Sender: %lu\nTarget: %lu\ntest 1: %lu\ntest 2: %lu\n",sender_target_id,targets,MS_TARGET_APPLICATION&targets,MS_TARGET_UNKNOWN&targets);
				BMessage *msg=new BMessage();
				container->FindMessage("parcel",msg);
				msg->AddPointer("_broadcast_origin_pointer_",broadcaster);
//				printf("Broadcasting this message:\n");
//				msg->PrintToStream();
				msgsysclient_st *cur=MsgSysClients;
				volatile uint32 current_target=0;
				bool found=false;
				broadcaster->broadcast_status_code=B_OK;
				broadcaster->broadcast_successful_receives=0;
				while (cur!=NULL) {
//					printf("getting current broadcast target id\n");
					if (cur->ptr==NULL) {
						cur=cur->next;
						continue;
					}
					current_target=cur->ptr->BroadcastTarget();
//					if (targets==MS_TARGET_PROTOCOL)
//						printf("targets: 0x%-2x\tcurrent: 0x%-2x\tcomparison: 0x%-2x\n",targets,current_target,current_target&targets);
					switch(targets) {
						case MS_TARGET_UNKNOWN: {
							found=true;
//							printf("\t*** Attempt to send broadcast to unknown target. Duh!\n");
						}break;
						case MS_TARGET_ALL: {
							found=true;
//							broadcaster->broadcast_status_code|=cur->ptr->ReceiveBroadcast(msg);
							_message_targets_++;
								cur->ptr->_message_queue_.Lock();
								BMessage *copy=new BMessage(*msg);
							
								cur->ptr->_message_queue_.AddMessage(copy);
								printf("message copy %p added to queue for %s\n",cur->ptr->MsgSysObjectName());
							
								cur->ptr->_message_queue_.Unlock();
								if (cur->ptr->_msg_receiver_running_==0) {
							//		status_t status;
							//		wait_for_thread(cur->ptr->_msg_receiver_thread_,&status);
//									release_sem(cur->ptr->_msg_receiver_sem_);
									cur->ptr->_msg_receiver_thread_=spawn_thread(MS_Start_Thread,cur->ptr->MS_Name,B_LOW_PRIORITY,cur->ptr);
									resume_thread(cur->ptr->_msg_receiver_thread_);

								}
							broadcaster->broadcast_successful_receives++;
						}break;


						case MS_TARGET_PROTOCOL: {
							if ((current_target&MS_TARGET_PROTOCOL)!=0) {
	//							printf("A protocol has been found. %s (0x%-2x;0x%-2x)\n",cur->ptr->MsgSysObjectName(),current_target,current_target&0x40);
								
								found=true;
								broadcaster->broadcast_successful_receives++;
								cur->ptr->_message_queue_.Lock();
								cur->ptr->_message_queue_.AddMessage(new BMessage(*msg));
								cur->ptr->_message_queue_.Unlock();
								if (cur->ptr->_msg_receiver_running_==0) {
							//		status_t status;
							//		wait_for_thread(cur->ptr->_msg_receiver_thread_,&status);
//									release_sem(cur->ptr->_msg_receiver_sem_);
									cur->ptr->_msg_receiver_thread_=spawn_thread(MS_Start_Thread,cur->ptr->MS_Name,B_LOW_PRIORITY,cur->ptr);
									resume_thread(cur->ptr->_msg_receiver_thread_);

								}
							}// else {
	//							printf("%s does not seem to be a protocol; bypassing...\n",cur->ptr->MsgSysObjectName());						
	//						}
							
						}break;


						case MS_TARGET_SELF: {
							if (current_target==sender_target_id) {
								found=true;
								broadcaster->broadcast_successful_receives++;
								cur->ptr->_message_queue_.Lock();
								cur->ptr->_message_queue_.AddMessage(new BMessage(*msg));
								cur->ptr->_message_queue_.Unlock();
								if (cur->ptr->_msg_receiver_running_==0) {
							//		status_t status;
							//		wait_for_thread(cur->ptr->_msg_receiver_thread_,&status);
//									release_sem(cur->ptr->_msg_receiver_sem_);
									cur->ptr->_msg_receiver_thread_=spawn_thread(MS_Start_Thread,cur->ptr->MS_Name,B_LOW_PRIORITY,cur->ptr);
									resume_thread(cur->ptr->_msg_receiver_thread_);

								}
								
//								broadcaster->broadcast_status_code=cur->ptr->ReceiveBroadcast(msg);
								_message_targets_++;
							}
							
						}break;
						default: {
//							printf("default\n");
							if (((current_target&targets)>=1) || (current_target==targets) ) {
								found=true;
//								printf("calling ReceiveBroadcast()\n");
								broadcaster->broadcast_successful_receives++;
								cur->ptr->_message_queue_.Lock();
								cur->ptr->_message_queue_.AddMessage(new BMessage(*msg));
								cur->ptr->_message_queue_.Unlock();
								if (cur->ptr->_msg_receiver_running_==0) {
							//		status_t status;
							//		wait_for_thread(cur->ptr->_msg_receiver_thread_,&status);
//									release_sem(cur->ptr->_msg_receiver_sem_);
									cur->ptr->_msg_receiver_thread_=spawn_thread(MS_Start_Thread,cur->ptr->MS_Name,B_LOW_PRIORITY,cur->ptr);
									resume_thread(cur->ptr->_msg_receiver_thread_);

								}
//								broadcaster->broadcast_status_code|=cur->ptr->ReceiveBroadcast(msg);
								_message_targets_++;
							}
							
						}
						
					}
					
					cur=cur->next;
				}
				if (!found) {
					MessageSystem *ptr=NULL;
					container->FindPointer("broadcaster_pointer",(void**)&ptr);
					if (ptr!=NULL)
						ptr->broadcast_status_code=B_ERROR;
					else
						ptr->broadcast_status_code=B_OK;
				}
				delete msg;
				delete container;
				broadcaster->BroadcastFinished();
			}
//			printf("processing pass done.\n");
		}
		
	}
	return 0;
}

void MessageSystem::Broadcast(uint32 targets,BMessage *msg) 
{
	BAutolock alock(&local_msg_sys_lock);
	if (alock.IsLocked()) {
		
	if (acquire_sem(transmit_sem)==B_OK) {
//		printf("MessageSystem::Broadcast\n");
		if (msg!=NULL) {
					
			msg->PrintToStream();
			BMessage *container=new BMessage();
			container->AddInt32("broadcast_targets",targets);
			container->AddPointer("broadcaster_pointer",this);
			container->AddInt32("broadcaster_target_id",BroadcastTarget());
			msgsysclient_st *cur=MsgSysClients;
			while (cur!=NULL) {
				
					if (cur->ptr==this) {
						container->AddBool("system_registered",true);
						break;
					}
					cur=cur->next;
			}
			if (cur==NULL) {
				container->AddBool("system_registered",false);
			}
/*
	if the system_registered variable is true, then a broadcast or broadcast reply
can be sent to the sender of the current message. Useful information.
*/			
			container->AddMessage("parcel",msg);
			_messages_sent_++;
//			printf("sending message to processing thread\n");
			send_data(_ProcessThread_,0,&container,sizeof(&container));
//			printf("message sent to processing thread.\n");
		}
//		printf("releasing process sem\n");
		release_sem(process_sem);
//		printf("sem released, message should be processing now.\n");
	}
	}
	
}
int32 MessageSystem::_ProcessMessage_(void *arg) 
{
	MessageSystem *me=(MessageSystem*)arg;
//	BAutolock alock(&me->_processmessage_lock_);
	
//	if (alock.IsLocked()) {
		volatile int32 count=0;
		BMessage *msg;
		me->_ms_receiver_quit_=0;
	//	me->_processmessage_lock_.Lock();
			if (me->_msg_receiver_running_==0)
				atomic_add(&me->_msg_receiver_running_,1);
	//	me->_processmessage_lock_.Unlock();
		while (!me->_ms_receiver_quit_) {
//				printf("me: %p\t%s\n",me,MsgSysObjectName());
			if (me->_ms_receiver_quit_)
				break;
//			printf("count messages\n");
			me->_message_queue_.Lock();
			if (!me->_message_queue_.IsEmpty()) {
				count=me->_message_queue_.CountMessages();
//				printf("count: %ld\n",count);
			}
			me->_message_queue_.Unlock();
			if (count>0) {
//				printf("processing messages\n");
				while (count>0) {
					if (me->_ms_receiver_quit_)
						break;
	//	me->_processmessage_lock_.Lock();
					me->_message_queue_.Lock();
//									printf("Retrieving message\n");
					msg=me->_message_queue_.NextMessage();
					count=me->_message_queue_.CountMessages();
					me->_message_queue_.Unlock();
					if (msg!=NULL) {		
						me->ReceiveBroadcast(msg);
						delete msg;
						msg=NULL;
					}
	//	me->_processmessage_lock_.Unlock();

				}
			}
			if (me->_message_queue_.IsEmpty()){
//						printf("MessageSystem: No more messages\n");
				break;
			}
		}
//	me->_processmessage_lock_.Lock();
	//	printf("setting to not running\n");
//	me->_processmessage_lock_.Unlock();
//	atomic_add(&me->_msg_receiver_running_,-1);
//	if (me->_msg_receiver_running_!=0)
	//	me->_processmessage_lock_.Lock();

		me->_msg_receiver_running_=0;
	//			me->_processmessage_lock_.Unlock();

	//me->_msg_receiver_thread_=0;
//	}
//	printf("exiting thread\n");
	exit_thread(0L);
	return 0L;
}

status_t MessageSystem::ReceiveBroadcast(BMessage *msg)
{
		
//		while (!_message_queue_->IsEmpty()) {
//			msg=_message_queue->NextMessage();
			
//			printf("%p has received this message:\n",this);
			msg->PrintToStream();
//			delete msg;
			
//		}
	
	return B_OK;
}
status_t MessageSystem::BroadcastReply(BMessage *msg)
{
	msg->PrintToStream();
	return B_OK;
}
void MessageSystem::MsgSysUnregister(MessageSystem *target){
	BAutolock alock(msgsyslock);
	if (alock.IsLocked()) {
		msgsysclient_st *cur=MsgSysClients, *last=MsgSysClients;
		while (cur!=NULL) {
			if (cur->ptr==target) {
				if (cur==MsgSysClients) {
					MsgSysClients=MsgSysClients->next;
					delete cur;
				} else {
					last->next=cur->next;
					delete cur;
				}
				break;
			}
			last=cur;
			cur=cur->next;
		}
	}
	broadcast_target_count--;
}
int32 MessageSystem::CountMembers() {
	int32 count=0;
	BAutolock alock(msgsyslock);
	if (alock.IsLocked()) {
		msgsysclient_st *cur=MsgSysClients;
		while (cur!=NULL) {
			count++;
			cur=cur->next;
		}
	}
	return count;
}
MessageSystem *MessageSystem::GetMember(int32 index) {
	MessageSystem *member=NULL;
	BAutolock alock(msgsyslock);
	if (alock.IsLocked()) {
		msgsysclient_st *cur=MsgSysClients;
		if ((index>=CountMembers()) || (index<0) || (cur==NULL))
			return member;
		for (int32 i=0; i<index; i++) {
			cur=cur->next;
		}
		if (cur!=NULL)
			member=cur->ptr;
	}
	return member;
}
void MessageSystem::MsgSysRegister(MessageSystem *target){
	BAutolock alock(msgsyslock);
	if (alock.IsLocked()) {
		msgsysclient_st *cur=MsgSysClients;
//		printf("MsgSysClients: %p\n",MsgSysClients);
		if (cur==NULL) {
//		printf("Check point 1\n");
			MsgSysClients=new msgsysclient_st;
			cur=MsgSysClients;
		} else {
//		printf("Check point 2\n");
			while(cur->next!=NULL)
				cur=cur->next;
			cur->next=new msgsysclient_st;
			cur=cur->next;
		}
//		printf("Check point 3\n");
//		printf("Current MessageSystem Client: %p\n",cur);
		cur->next=NULL;
		cur->ptr=target;
		broadcast_target_count++;
	}
//#ifdef DEBUG
//	char output[200];
//	sprintf(output,"New receiver registered; Broadcast System Members: %ld\n",broadcast_target_count);
	
//	Debug(output);
//#endif
}
uint32 MessageSystem::BroadcastTarget() 
{
	return MS_TARGET_UNKNOWN;
}
void MessageSystem::BroadcastFinished()
{
}

