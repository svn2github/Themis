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
using namespace _Themis_Networking_;
TCPManager *TCPManager::_manager_=NULL;
sem_id TCPManager::process_sem_1=0;
sem_id TCPManager::process_sem_2=0;

TCPManager::TCPManager() {
	Init();
//	lock=new BLocker(true);
	Connection::TCPMan=this;
	Connection::ListLock=new BLocker(true);
	process_sem_1=create_sem(0,"tcp_manager_sem1");
	process_sem_2=create_sem(1,"tcp_manager_sem2");
	signal(SIGALRM,&AlarmHandler);
	
#ifdef USEOPENSSL
	printf("Initializing OpenSSL...");
	
	SSL_library_init();
	SSL_load_error_strings();
		SSLeay_add_ssl_algorithms();
	sslmeth = SSLv23_client_method();
	sslctx=SSL_CTX_new (sslmeth);
	SSL_CTX_set_options(sslctx, SSL_OP_ALL);
	SSL_CTX_set_default_verify_paths(sslctx);
	unsigned char buf[17];
	memset(buf,0,17);
	RAND_egd(tmpnam(NULL));
	 do{
		for (int i=0;i<16;i++) {
			srand(real_time_clock());
			buf[i]=(real_time_clock_usecs()/system_time())%128+(rand()%128);
		}
		RAND_seed(buf,16);
	}while(RAND_status()!=1);
	printf("done.\n");
	
#endif
}
TCPManager::~TCPManager() {
	printf("destroying connections...");
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
	printf("done.\n");
#ifdef USEOPENSSL
	printf("Destroying OpenSSL objects...");
	fflush(stdout);
	if (sslctx!=NULL)
		SSL_CTX_free (sslctx);
	printf("done.\n");
#else
#endif	
//	delete lock;
	delete Connection::ListLock;
	delete_sem(process_sem_1);
	delete_sem(process_sem_2);
	
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
	
#ifdef OPENSSL
	sslmeth=NULL;
	sslctx=NULL;
#endif
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
	int32 connect_count;
	int32 current_time;
	int32 last_used;
	
	int32 time_out=DEFAULT_TIMEOUT;
	
	while(!_quitter_) {
//		if (acquire_sem(process_sem_1)==B_OK) {
//			printf("processing\n");
			connect_count=Connection::CountConnections();
			
			if (connect_count>0) {
				current_time=real_time_clock();
				for (int32 i=0; i<connect_count; i++) {
					lock.Lock();
					
					connection=Connection::ConnectionAt(i);
					if (!Connection::HasConnection(connection))
					{
						lock.Unlock();
						continue;
					}
					if (connection->IsConnected()) {
						if (!connection->NotifiedConnect()) {
							connection->ConnectionEstablished();
							connection->NotifyConnect();
						}
//						printf("[TCP Manager] Checking if data is waiting.\n");
						if (connection->IsDataWaiting()) {
							connection->RetrieveData();
						}
						
					} else {
						if (!connection->NotifiedDisconnect()) {
							connection->NotifyDisconnect();
							Connection::ConnectionList->RemoveItem(connection);
							delete connection;
							connection=NULL;
							lock.Unlock();
							continue;
						}
						
					}
					if (!Connection::HasConnection(connection))
					{
						lock.Unlock();
						continue;
					}
					last_used=connection->LastUsed();
					if ((last_used!=0) && ((current_time-last_used)>=time_out)) {
						connection->TimeOut();
						Disconnect(connection);
						lock.Unlock();
						continue;
					}
					
					lock.Unlock();
					
				}
				
			}
			snooze(7500);
//			release_sem(process_sem_2);
//		}
			
	}
//	set_alarm(B_INFINITE_TIMEOUT,B_PERIODIC_ALARM);
	atomic_add(&Connection::SystemReady,-1);
	
	return 0;
}

Connection *TCPManager::CreateConnection(NetworkableObject *net_obj,const char *host,uint16 port,bool secure,bool asynch) {
	printf("CreateConnection begun\n");
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
						existing_available;
						break;
					}
				}
			}
		}
		if (existing_available) {
			connection->StartUsing();
			connection->AssignNetObject(net_obj);
			connection->Connect();
			sessions_reused++;
		} else {
			connection=new Connection(net_obj,host,port,secure,asynch);
			connection->StartUsing();
			connection->Connect();
			sessions_created++;
		}
		
//	}
	printf("CreateConnection is done\n");
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
#ifdef OPENSSL
	return true;
#else
	return false;
#endif	
}

#ifdef USEOPENSSL
SSL_METHOD *TCPManager::SSLMethod() {
	SSL_METHOD *method=NULL;
	BAutolock alock(lock);
	if (alock.IsLocked())
		method=sslmeth;
	return method;
	
}

SSL_CTX *TCPManager::SSLContext() {
	SSL_CTX *context=NULL;
	BAutolock alock(lock);
	if (alock.IsLocked()) 
		context=sslctx;
	return context;
}
#endif

