/*
Copyright (c) 2004 Z3R0 One. All Rights Reserved.

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

Original Author & Project Manager: Z3R0 One (z3r0_one@users.sourceforge.net)
Project Start Date: October 18, 2000
*/
#include "msgsystem.h"
#include "plugclass.h"
#include <Autolock.h>
#include <stdio.h>
#include <OS.h>
#include <String.h>
volatile int32 MessageSystem::broadcast_target_count=0;
MessageSystem::msgsysclient_st *MessageSystem::MsgSysClients=NULL;
BMessageQueue *MessageSystem::MS_MQueue=NULL;
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
//		status_t stat;
//		wait_for_thread(_msg_receiver_thread_,&stat);
		release_sem(_msg_receiver_sem_);
		delete_sem(_msg_receiver_sem_);
//		delete _message_queue_;
	}
}
int32 MessageSystem::MS_Start_Thread(void *arg) 
{
	MessageSystem *obj=(MessageSystem*)arg;
	int32 retval=0;
	obj->_ProcessMessage_(obj);
	wait_for_thread(obj->_msg_receiver_thread_,(status_t *)&retval);
	printf("thread returned: %ld\n",retval);
	
	return retval;
	
}
const char * MessageSystem::MsgSysObjectName() {
	return MS_Name;
}
void MessageSystem::Debug(const char *message, int32 plugid) {
	if (_Quit_Thread_)
		return;
	BMessage debug(DEBUG_INFO_MSG);
	debug.AddInt32( "command", COMMAND_INFO );
	if (plugid!=-1)
		debug.AddInt32( "pluginID", plugid );
	debug.AddString("message",message);
	Broadcast(MS_TARGET_DEBUG,&debug);
}
int32 MessageSystem::_ProcessBroadcasts_(void *data) 
{
	if (MS_MQueue==NULL)
		MS_MQueue=new BMessageQueue();
	while (!_Quit_Thread_) {
//		if (acquire_sem(process_sem)==B_OK) 
			{
			if (_Quit_Thread_)
				break;
			if (MS_MQueue==NULL)
			{
				printf("MS_MQueue is null! Broadcast system exiting.\n");
				break;
			}
			MS_MQueue->Lock();
			if (!MS_MQueue->IsEmpty())//has_data(_ProcessThread_))
			{
			MS_MQueue->Unlock();
//				BMessage *msg;
				thread_id sender_thread;
				BMessage *container=NULL;
				uint32 targets=0;
				uint32 sender_target_id=0;
				MessageSystem *broadcaster=NULL;
				MS_MQueue->Lock();
				container=MS_MQueue->NextMessage();
				MS_MQueue->Unlock();
				//receive_data(&sender_thread,&container,sizeof(&container));
		//	release_sem(transmit_sem);
				container->FindPointer("broadcaster_pointer",(void**)&broadcaster);
				container->FindInt32("broadcast_targets",(int32*)&targets);
				container->FindInt32("broadcaster_target_id",(int32*)&sender_target_id);
				BMessage *msg=new BMessage();
				container->FindMessage("parcel",msg);
				msg->AddPointer("_broadcast_origin_pointer_",broadcaster);
				msg->AddString("_broadcast_origin_name_",broadcaster->MsgSysObjectName());
				msgsysclient_st *cur=MsgSysClients;
				volatile uint32 current_target=0;
				bool found=false;
				broadcaster->broadcast_status_code=B_OK;
				broadcaster->broadcast_successful_receives=0;
				MessageSystem *member;
				msgsysclient_st *verifier;
				volatile int32 member_found;
				BString member_name;
//				printf("Message sender is %s\n",broadcaster->MsgSysObjectName());
				while (cur!=NULL) {
					if (_Quit_Thread_)
						break;
					if (msgsyslock.LockWithTimeout(25000)==B_OK)
						{
						member_found=0;
						member=NULL;
						verifier=MsgSysClients;
						while (verifier!=NULL)
						{
							if (verifier==cur)
							{
								atomic_add(&member_found,1);
								break;
							}
							verifier=verifier->next;
						}
						if (!member_found)
						{
							cur=MsgSysClients;
							msgsyslock.Unlock();
							continue;
						}
						if (cur->ptr==NULL) {
							cur=cur->next;
							msgsyslock.Unlock();
							continue;
						}
						member_name=cur->ptr->MsgSysObjectName();
						member=cur->ptr;
						
						current_target=member->BroadcastTarget();
						switch(targets) {
							case MS_TARGET_UNKNOWN: {
								found=true;
							}break;
							case MS_TARGET_DEBUG: {
								if (current_target==MS_TARGET_DEBUG)
								{
									found=true;
									_message_targets_++;
									member->_message_queue_.Lock();
									member->_message_queue_.AddMessage(new BMessage(*msg));
									member->_message_queue_.Unlock();
									if (member->_msg_receiver_running_==0) {
										member->_msg_receiver_thread_=spawn_thread(MS_Start_Thread,member->MS_Name,B_LOW_PRIORITY,member);
										resume_thread(member->_msg_receiver_thread_);
	
									}
									broadcaster->broadcast_successful_receives++;
									
								}
							}break;
							case MS_TARGET_ALL: {
								found=true;
								_message_targets_++;
									member->_message_queue_.Lock();
									member->_message_queue_.AddMessage(new BMessage(*msg));
									member->_message_queue_.Unlock();
									if (member->_msg_receiver_running_==0) {
										member->_msg_receiver_thread_=spawn_thread(MS_Start_Thread,member->MS_Name,B_LOW_PRIORITY,member);
										resume_thread(member->_msg_receiver_thread_);
	
									}
								broadcaster->broadcast_successful_receives++;
							}break;
	
	
							case MS_TARGET_PROTOCOL: {
								if ((current_target&MS_TARGET_PROTOCOL)!=0) {
									_message_targets_++;
									found=true;
									broadcaster->broadcast_successful_receives++;
									member->_message_queue_.Lock();
									member->_message_queue_.AddMessage(new BMessage(*msg));
									member->_message_queue_.Unlock();
									if (member->_msg_receiver_running_==0) {
										member->_msg_receiver_thread_=spawn_thread(MS_Start_Thread,member->MS_Name,B_LOW_PRIORITY,member);
										resume_thread(member->_msg_receiver_thread_);
	
									}
								}							
							}break;
	
	
							case MS_TARGET_SELF: {
								if (current_target==sender_target_id) {
									found=true;
									broadcaster->broadcast_successful_receives++;
									member->_message_queue_.Lock();
									member->_message_queue_.AddMessage(new BMessage(*msg));
									member->_message_queue_.Unlock();
									if (member->_msg_receiver_running_==0) {
										member->_msg_receiver_thread_=spawn_thread(MS_Start_Thread,member->MS_Name,B_LOW_PRIORITY,member);
										resume_thread(member->_msg_receiver_thread_);
	
									}
									
									_message_targets_++;
								}
								
							}break;
							default: {
								if (((current_target&targets)>=1) || (current_target==targets) ) {
									found=true;
									broadcaster->broadcast_successful_receives++;
									member->_message_queue_.Lock();
									member->_message_queue_.AddMessage(new BMessage(*msg));
									member->_message_queue_.Unlock();
									if (member->_msg_receiver_running_==0) {
										member->_msg_receiver_thread_=spawn_thread(MS_Start_Thread,member->MS_Name,B_LOW_PRIORITY,member);
										resume_thread(member->_msg_receiver_thread_);
	
									}
									_message_targets_++;
								}
								
							}
							
						}
						cur=cur->next;
						msgsyslock.Unlock();
					}
				}
				atomic_add(&broadcaster->_broadcast_complete_,1);
				if (_Quit_Thread_)
				{//make sure we stop delivering messages as well; messages can sometimes generate more messages...
					msgsysclient_st *cur=MsgSysClients;
					while (cur!=NULL)
					{
						atomic_add(&cur->ptr->_ms_receiver_quit_,1);
						cur=cur->next;
					}
					break;
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
			} else
			{
				MS_MQueue->Unlock();
				snooze(10000);
			}
//			printf("processing pass done.\n");
		}
		
	}
	return 0;
}

void MessageSystem::Broadcast(uint32 targets,BMessage *msg) 
{
//	printf("%s is attempting to send a broadcast.\n",MsgSysObjectName());
		if (_Quit_Thread_!=0)
			return;//ignore new broadcasts if we're shutting down.
	BAutolock alock(&local_msg_sys_lock);
	if (alock.IsLocked()) {
//	if (acquire_sem(transmit_sem)==B_OK)
	{
		_broadcast_complete_=0;
		if (msg!=NULL) {
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
			MS_MQueue->Lock();
			MS_MQueue->AddMessage(container);
			MS_MQueue->Unlock();
		//	send_data(_ProcessThread_,0,&container,sizeof(&container));
		}
//		release_sem(process_sem);
	}
	}
	
}
int32 MessageSystem::_ProcessMessage_(void *arg) 
{
	MessageSystem *me=(MessageSystem*)arg;
		volatile int32 count=0;
		BMessage *msg;
		me->_ms_receiver_quit_=0;
	//		if (me->_msg_receiver_running_==0)
				atomic_add(&me->_msg_receiver_running_,1);
		while (!me->_ms_receiver_quit_) {
			if (me->_ms_receiver_quit_)
				break;
			me->_message_queue_.Lock();
			if (!me->_message_queue_.IsEmpty()) {
				count=me->_message_queue_.CountMessages();
			}
			me->_message_queue_.Unlock();
			if (count>0) {
				while (count>0) {
					if (me->_ms_receiver_quit_)
						break;
					me->_message_queue_.Lock();
					msg=me->_message_queue_.NextMessage();
					count=me->_message_queue_.CountMessages();
					me->_message_queue_.Unlock();
					if (msg!=NULL) {		
						me->ReceiveBroadcast(msg);
						delete msg;
						msg=NULL;
					}

				}
			}
			me->_message_queue_.Lock();
			if (me->_message_queue_.IsEmpty()){
				me->_message_queue_.Unlock();
				break;
			}
			me->_message_queue_.Unlock();
		}

		me->_msg_receiver_running_=0;
		exit_thread(0);
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
		BString name=target->MsgSysObjectName();
		printf("%s is unregistering from the message system\n",name.String());
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
				cur=NULL;
				break;
			}
			last=cur;
			cur=cur->next;
		}
		broadcast_target_count--;
		printf("%s is now unregistered\n",name.String());
	}
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

