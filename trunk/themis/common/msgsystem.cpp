#include "msgsystem.h"
#include <Autolock.h>
#include <stdio.h>
volatile int32 MessageSystem::broadcast_target_count=0;
MessageSystem::msgsysclient_st *MessageSystem::MsgSysClients=NULL;
BLocker *MessageSystem::msgsyslock=NULL;
sem_id MessageSystem::process_sem=0;
sem_id MessageSystem::transmit_sem=0;
volatile int32 MessageSystem::_Quit_Thread_=0;
thread_id MessageSystem::_ProcessThread_=0;
volatile uint32 MessageSystem::_messages_sent_=0;
volatile uint32 MessageSystem::_message_targets_=0;
MessageSystem::MessageSystem(char *msg_sys_name)
{
	MS_Name=msg_sys_name;
	if (MS_Name==NULL)
		MS_Name="MessageSystem member";
	broadcast_status_code=B_NO_ERROR;
	_message_queue_=new BMessageQueue();
	_msg_receiver_sem_=create_sem(0,"message_receiver_sem");
	_ms_receiver_quit_=0;
	_msg_receiver_running_=0;
	_msg_receiver_thread_=spawn_thread(MS_Start_Thread,MS_Name,B_LOW_PRIORITY,this);
	resume_thread(_msg_receiver_thread_);
}

MessageSystem::~MessageSystem()
{
	_ms_receiver_quit_=1;
	release_sem(_msg_receiver_sem_);
	delete_sem(_msg_receiver_sem_);
	delete _message_queue_;
	status_t stat;
	wait_for_thread(_msg_receiver_thread_,&stat);
}
int32 MessageSystem::MS_Start_Thread(void *arg) 
{
	MessageSystem *obj=(MessageSystem*)arg;
	return obj->_ProcessMessage_(obj);
}

int32 MessageSystem::_ProcessBroadcasts_(void *data) 
{
	while (!_Quit_Thread_) {
		if (acquire_sem(process_sem)==B_OK) {
//			printf("MessageSystem::_ProcessBroadcasts_\n");
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
//				printf("Broadcasting this message:\n");
				msg->PrintToStream();
				msgsysclient_st *cur=MsgSysClients;
				volatile uint32 current_target=0;
				bool found=false;
				broadcaster->broadcast_status_code=B_OK;
				broadcaster->broadcast_successful_receives=0;
				while (cur!=NULL) {
//					printf("getting current broadcast target id\n");
					current_target=cur->ptr->BroadcastTarget();
//					printf("targets: %lu\tcurrent: %lu\tcomparison: %lu\n",targets,current_target,current_target&targets);
					switch(targets) {
						case MS_TARGET_UNKNOWN: {
							found=true;
//							printf("\t*** Attempt to send broadcast to unknown target. Duh!\n");
						}break;
						case MS_TARGET_ALL: {
							found=true;
//							broadcaster->broadcast_status_code|=cur->ptr->ReceiveBroadcast(msg);
							_message_targets_++;
								cur->ptr->_message_queue_->AddMessage(new BMessage(*msg));
								if (cur->ptr->_msg_receiver_running_==0) {
									release_sem(cur->ptr->_msg_receiver_sem_);
								}
							broadcaster->broadcast_successful_receives++;
						}break;
/*
						case MS_TARGET_PROTOCOL: {
							if (current_target&MS_TARGET_PROTOCOL==MS_TARGET_PROTOCOL) {
								cur->ptr->ReceiveBroadcast(msg);
								_message_targets_++;
							}
						}break;
*/
						case MS_TARGET_SELF: {
							if (current_target==sender_target_id) {
								found=true;
								broadcaster->broadcast_successful_receives++;
								cur->ptr->_message_queue_->AddMessage(new BMessage(*msg));
								if (cur->ptr->_msg_receiver_running_==0) {
									release_sem(cur->ptr->_msg_receiver_sem_);
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
								cur->ptr->_message_queue_->AddMessage(new BMessage(*msg));
								if (cur->ptr->_msg_receiver_running_==0) {
									release_sem(cur->ptr->_msg_receiver_sem_);
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
			}
//			printf("processing pass done.\n");
		}
		
	}
	return 0;
}

void MessageSystem::Broadcast(uint32 targets,BMessage *msg) 
{
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
int32 MessageSystem::_ProcessMessage_(void *arg) 
{
	MessageSystem *me=(MessageSystem*)arg;
	volatile int32 count=0;
	BMessage *msg;
	while (!me->_ms_receiver_quit_) {
		if (acquire_sem(me->_msg_receiver_sem_)==B_OK) {
			if (_ms_receiver_quit_)
				break;
			me->_msg_receiver_running_=1;
			me->_message_queue_->Lock();
			if (!me->_message_queue_->IsEmpty()) {
				count=me->_message_queue_->CountMessages();
			}
			me->_message_queue_->Unlock();
			if (count>0) {
				while (count!=0) {
					if (_ms_receiver_quit_)
						break;
			me->_message_queue_->Lock();
					msg=me->_message_queue_->NextMessage();
					count=me->_message_queue_->CountMessages();
			me->_message_queue_->Unlock();
					me->ReceiveBroadcast(msg);
					delete msg;
					msg=NULL;
					
				}
				
			}
			
			me->_msg_receiver_running_=0;
			
		}
		
	}
	
}

status_t MessageSystem::ReceiveBroadcast(BMessage *msg)
{
		
//		while (!_message_queue_->IsEmpty()) {
//			msg=_message_queue->NextMessage();
			
//			printf("%p has received this message:\n",this);
			msg->PrintToStream();
			delete msg;
			
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
	printf("New receiver registered; Broadcast System Members: %ld\n",broadcast_target_count);
}
uint32 MessageSystem::BroadcastTarget() 
{
	return MS_TARGET_UNKNOWN;
}
		
