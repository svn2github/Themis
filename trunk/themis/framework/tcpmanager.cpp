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
#include "tcpmanager.h"

#include <stdio.h>
#include <string.h>
#include <Autolock.h>
#include <signal.h>
#include "connection.h"
#include "networkableobject.h"
using namespace _Themis_Networking_;
TCPManager *TCPManager::_manager_=NULL;
sem_id TCPManager::process_sem_1=0;
sem_id TCPManager::process_sem_2=0;

TCPManager::TCPManager() {
	Init();
	cryptInit();
//	lock=new BLocker(true);
	Connection::TCPMan=this;
	Connection::ListLock=new BLocker(true);
	process_sem_1=create_sem(0,"tcp_manager_sem1");
	process_sem_2=create_sem(1,"tcp_manager_sem2");
	signal(SIGALRM,&AlarmHandler);
	
}
TCPManager::~TCPManager() {
	//printf("destroying connections...");
	fflush(stdout);
	
	if (Connection::CountConnections()>0) {
		Connection *connection;
		while (Connection::CountConnections()>0) {
			connection=Connection::ConnectionAt(0);
			if ((Connection::HasConnection(connection)) || (connection!=NULL))
			{
				Connection::ConnectionList->RemoveItem(connection);
				delete connection;
				connection=NULL;
			}
		}
	}
	//printf("done.\n");
//	delete lock;
	delete Connection::ListLock;
	delete_sem(process_sem_1);
	delete_sem(process_sem_2);
	cryptEnd();
	printf("============\nTCP Manager:\n============\n");
	printf("Bytes Sent:\t%Ld\nBytes Received:\t%Ld\n",total_bytes_sent,total_bytes_received);
	printf("Total Connections:\t%Lu\nConnections Reused:\t%Lu (%1.2f%%)\n",sessions_created,sessions_reused, (sessions_reused/((sessions_created*1.0)+((sessions_created==0)?1:0))*100.0));
	printf("============\n");
	
}
TCPManager *TCPManager::ManagerPointer() {
	return _manager_;
}

void TCPManager::Init() {
	_quitter_=0;
	Connection::TCPMan=NULL;
	total_bytes_received=0L;
	total_bytes_sent=0L;
	sessions_reused=0L;
	sessions_created=0L;
	process_sem_1=0;
	process_sem_2=0;
	_manager_=this;
	
}

void TCPManager::Quit() {
	atomic_add(&_quitter_,1);
	
}
void TCPManager::Start() {
	thread=spawn_thread(_Thread_Starter,"TCP Manager",1,this);
	resume_thread(thread);
	atomic_add(&Connection::SystemReady,1);
}
int32 TCPManager::_Thread_Starter(void *arg) {
	TCPManager *obj=(TCPManager *)arg;
	return obj->_Manager_Thread();
	
}
void TCPManager::AlarmHandler(int signum) {
//	printf("alarm\n");
	
	if (acquire_sem_etc(process_sem_2,1,B_ABSOLUTE_TIMEOUT,7500)==B_OK) {
//		printf("about to trigger processing...\n");
		release_sem(process_sem_1);
	}
	
}

int32 TCPManager::_Manager_Thread() {
//	set_alarm(25000,B_PERIODIC_ALARM);
	Connection *connection;
	int32 connect_count = 0;
	int32 current_time;
//	int32 last_used;
	
	int32 time_out=DEFAULT_TIMEOUT;
	
	while(!_quitter_) {
//		if (acquire_sem(process_sem_1)==B_OK) {
//			printf("processing\n");
			if (lock.LockWithTimeout(25000)==B_OK)
			{
				connect_count=Connection::CountConnections();
				
				if (connect_count>0) {
					current_time=real_time_clock();
					for (int32 i=0; i<connect_count; i++) {
					
//						printf("TCP Manager: lock acquired\n");
						connection=Connection::ConnectionAt(i);
						if (connection != NULL && Connection::HasConnection(connection)) {
							if (connection->IsConnected()) {
								if (connection->IsInUse()) {
									if (!connection->NotifiedConnect()) {
										if (!connection->already_connected)
											connection->ConnectionEstablished();
										connection->NotifyConnect();
									} else {
										if (connection->IsDataWaiting()) {
											connection->RetrieveData();
											if (connection->owner!=NULL) {
												connection->owner->DataIsWaiting(connection);
											}
										}
										else if ((connection->LastUsed()!=0) && ((current_time-connection->LastUsed())>=time_out)) {
			//								if (connection->owner!=NULL)
			//									connection->owner->DestroyingConnectionObject(connection);
											connection->TimeOut();
											Disconnect(connection);
										}
									}
								} else {
									if (connection->IsDataWaiting()) {
	//									printf("TCP Manager: data is waiting on unused connection; receiving and flushing data\n");
										int32 lastused=connection->LastUsed();
										connection->RetrieveData();
										connection->lastusedtime=lastused;
									}
									else if ((connection->LastUsed()!=0) && ((current_time-connection->LastUsed())>=time_out)) {
		//								if (connection->owner!=NULL)
		//									connection->owner->DestroyingConnectionObject(connection);
										connection->TimeOut();
										Disconnect(connection);
									}
								}
							} else {
								if (!connection->NotifiedDisconnect()) {
									connection->NotifyDisconnect();
	//								Disconnect(connection);
								}
								else {
									if ((connection->LastUsed()!=0) && ((current_time-connection->LastUsed())>=time_out)) {
	//									if (connection->owner!=NULL)
	//										connection->owner->DestroyingConnectionObject(connection);
										connection->TimeOut();
										Disconnect(connection);
									}
								}
							}
						}
					}
				//	snooze(10000);
				}
				lock.Unlock();
			}
			else {
				connect_count = 0;
			}
			
			if (connect_count == 0) {
				// No connections, so take it easy before we check again for connections.
				// Otherwise we create a busy loop.
				snooze(25000);
			}
//			release_sem(process_sem_2);
//		}
			
	}
//	set_alarm(B_INFINITE_TIMEOUT,B_PERIODIC_ALARM);
	atomic_add(&Connection::SystemReady,-1);
	
	return 0;
}

Connection *TCPManager::CreateConnection(NetworkableObject *net_obj,const char *host,uint16 port,bool secure,bool asynch) {
	//printf("CreateConnection begun\n");
	Connection *connection=NULL;
//	BAutolock alock(lock);
//	if (alock.IsLocked()) {
		int32 connections=0;
		Connection *target_connection=NULL;
		connections=Connection::CountConnections();
		bool existing_available=false;
		if (connections>0) {
			for (int32 i=0; i<connections; i++) {
				target_connection=Connection::ConnectionAt(i);
				if (!target_connection->IsInUse()) {
					if (target_connection->Matches(host,port)) {
						connection=target_connection;
						existing_available=true;
						break;
					}
				}
			}
		}
		if (existing_available) {
			//printf("TCP Manager: found available connection for use. (n: %p\tc: %p)\n",net_obj,connection);
			connection->AssignNetObject(net_obj);
			connection->StartUsing();
//			connection->Connect();
			sessions_reused++;
		} else {
			connection=new Connection(net_obj,host,port,secure,asynch);
			connection->StartUsing();
			connection->Connect();
			sessions_created++;
		}
		
//	}
	//printf("CreateConnection is done: %p\n",connection);
	return connection;
}
void TCPManager::Disconnect(Connection *connection) {
	BAutolock alock(lock);
	if (alock.IsLocked()) {
//		connection->StopUsing();
		if (Connection::HasConnection(connection)) {
			connection->Disconnect();
			Connection::ConnectionList->RemoveItem(connection);
			delete connection;
			connection=NULL;
		}
	}
}
void TCPManager::DoneWithSession(Connection *connection) {
	BAutolock alock(lock);
	if (alock.IsLocked()) {
		if (Connection::HasConnection(connection)) {
			if (!connection->IsConnected()) {
				Disconnect(connection);
				return;
			}
			
			connection->StopUsing();
			connection->NewSession();
		}	
	}
}
void TCPManager::AddBytesSent(off_t bytes) {
	BAutolock alock(lock);
	if (alock.IsLocked())
		total_bytes_sent+=bytes;
}
void TCPManager::AddBytesReceived(off_t bytes) {
	BAutolock alock(lock);
	if (alock.IsLocked())
		total_bytes_received+=bytes;
}

bool TCPManager::SSLEnabled() {
	return true;
}


