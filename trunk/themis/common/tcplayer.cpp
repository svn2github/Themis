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

#include "tcplayer.h"
#include <signal.h>
tcplayer *meTCP;
int32 TL;
int32 TU;
bool tcplayer::SSLSupported() {
	bool supported=false;
#ifdef USEOPENSSL
	supported=true;
#endif
	return supported;
}
char *tcplayer::SSLAboutString() {
	char *str=NULL;
#ifdef USEOPENSSL
	str="This product includes software developed by the OpenSSL Project for use in the OpenSSL Toolkit (http://www.openssl.org/)\nCopyright (c) 1998-2000 The OpenSSL Project.  All rights reserved.\nThis product includes cryptographic software written by Eric Young (eay@cryptsoft.com)\nCopyright (C) 1995-1998 Eric Young (eay@cryptsoft.com)\nAll rights reserved.\n";
#else
	str="Support for SSL is not included in this version, as it was selectively compiled out; possibly due to support or compatibility issues.\nHowever, when compiled in, Themis utilizes SSL software by OpenSSL (http://www.openssl.org ). It's copyright information is as follows:\nThis product includes software developed by the OpenSSL Project for use in the OpenSSL Toolkit (http://www.openssl.org/)\nCopyright (c) 1998-2000 The OpenSSL Project.  All rights reserved.\nThis product includes cryptographic software written by Eric Young (eay@cryptsoft.com)\nCopyright (C) 1995-1998 Eric Young (eay@cryptsoft.com)\nAll rights reserved.\n";
#endif
	return str;
}
tcplayer::tcplayer() {
	TL=TU=0;
	
	meTCP=this;
	lock=new BLocker("tcp lock",false);
//	mtx=new mutex;
	quit=0;
	tcp_mgr_sem=create_sem(1,"tcp_layer_manager_sem");
//	conn_sem=create_sem(1,"connection_handling_sem");
//	cb_sem=create_sem(1,"callback_sem");
	tcplayer_sem=create_sem(1,"tcplayer_sem");
//	acquire_sem(cb_sem);
	callback_head=NULL;	
	firstcb=0;
	atomic_add(&firstcb,1);
//	release_sem(cb_sem);
	conn_head=NULL;
	prev_conn=NULL;
#ifdef USEOPENSSL
	//required copyright notices and ego boosters
	printf("This product includes software developed by the OpenSSL Project for use in the OpenSSL Toolkit (http://www.openssl.org/)\nCopyright (c) 1998-2000 The OpenSSL Project.  All rights reserved.\nThis product includes cryptographic software written by Eric Young (eay@cryptsoft.com)\nCopyright (C) 1995-1998 Eric Young (eay@cryptsoft.com)\nAll rights reserved.\n");
	printf("tcplayer constructor\n");
	static int32 initcount = 0;
	static bool initialized = false;
	if (atomic_or(&initcount, 1) == 0) {
		SSL_library_init();
		SSL_load_error_strings();
		initialized = true;
	} else {
		while (!initialized) snooze(50000);
	}

	SSLeay_add_ssl_algorithms();
	sslmeth = SSLv23_client_method();
	sslctx=SSL_CTX_new (sslmeth);
	SSL_CTX_set_options(sslctx, SSL_OP_ALL);
	SSL_CTX_set_default_verify_paths(sslctx);
	if (sslctx==NULL)
		printf("SSL Context creation error\n");
	unsigned char buf[17];
	memset(buf,0,17);
	while(RAND_status()!=1) {
		for (int i=0;i<16;i++) {
			srand(real_time_clock());
			buf[i]=(real_time_clock_usecs()/system_time())%128+(rand()%128);
		}
		RAND_seed(buf,16);
	}
	printf("RAND status: %d\n",RAND_status());
#endif
}

tcplayer::~tcplayer() {
	printf("tcp_layer destructor, %ld connections\n",Connections());
	if (conn_head!=NULL) {
		connection *cur=conn_head;
		printf("tcplayer: deleting connections:\n");
		while (conn_head!=NULL) {
			printf("tcplayer: deleting connection %p\n",cur);
			cur=conn_head->next;
			delete conn_head;
			conn_head=cur;
		}
	}
//	acquire_sem(cb_sem);
printf("clearing out callbacks.\n");
	if (callback_head!=NULL) {
		DRCallback_st *cur=callback_head;
		while (callback_head!=NULL) {
			cur=callback_head->next;
			delete callback_head;
			callback_head=cur;
		}
		
	}
printf("done clearing callbacks.\n");
#ifdef USEOPENSSL
	if (sslctx!=NULL)
		SSL_CTX_free (sslctx);
#endif	
//	delete_sem(conn_sem);
	delete_sem(tcp_mgr_sem);
//	delete mtx;
//	release_sem(cb_sem);
//	delete_sem(cb_sem);
//	release_sem(tcplayer_sem);
	delete_sem(tcplayer_sem);
	lock->Unlock();
	delete lock;
	printf("~tcplayer end\n");
}
int32 tcplayer::Lock(int32 timeout) 
{
	
/*
	sem_id targetsem=0;
	sem_info info;
	int32 cookie=0;
	while (get_next_sem_info(0,&cookie,&info)==B_OK) {
		if (strcasecmp(info.name,"tcplayer_sem")==0) {
			targetsem=info.sem;
		}
		
	}
	if (targetsem==0)
		return B_ERROR;
	int32 result=B_ERROR;
*/
//	printf("tcp::Lock %ld\n",TL);
	thread_id callingthread=find_thread(NULL),lockingthread=lock->LockingThread();
	if (lockingthread!=B_ERROR) {
		if (callingthread==lockingthread)
			return B_OK;
	}
	
	if (timeout==-1) {
		if (lock->Lock()){
			
//	TL++;
			return B_OK;
		}
		
		return B_ERROR;
//		result=acquire_sem(targetsem);
	} else {
		return lock->LockWithTimeout(timeout);
		
		//result=acquire_sem_etc(targetsem,1,B_ABSOLUTE_TIMEOUT,timeout);
	}
//	return result;
}
void tcplayer::Unlock() 
{
//printf("tcp::Unlock %ld\n",TU);
//	TU++;
lock->Unlock();
	
/*
	sem_id targetsem=0;
	sem_info info;
	int32 cookie=0;
	while (get_next_sem_info(0,&cookie,&info)==B_OK) {
		if (strcasecmp(info.name,"tcplayer_sem")==0) {
			targetsem=info.sem;
		}
		
	}
	if (targetsem==0)
		return;
	release_sem(targetsem);
*/
}

void tcplayer::Start() {
	if (acquire_sem_etc(tcp_mgr_sem,1,B_ABSOLUTE_TIMEOUT,100000)==B_OK) {
		thread=spawn_thread(StartManager,"tcp_layer_manager",B_LOW_PRIORITY,this);
		resume_thread(thread);
	} else {
		fprintf(stderr,"tcp_layer_manager already running.\n");
	}
}
int32 tcplayer::StartManager(void *arg) {
	printf("tcp_layer starting\n");
	tcplayer* obj=(tcplayer*)arg;
	return obj->Manager();
}

int32 tcplayer::Manager() {
	connection *current;
	volatile uint32 connections=0;
	volatile uint32 curtime=0;
	volatile uint32 timeout=0;
	DRCallback_st *cur;
	status_t stat;
	while(!quit) {
		stat=Lock(20000);//lock->LockWithTimeout(20000);
		if (stat!=B_OK) {
			snooze(10000);
			continue;
		}
		
		connections=Connections();
		if (connections>0) {
			current=NextConnection();
			curtime=real_time_clock();
			if (current==NULL) {
				lock->Unlock();
				continue;
			}
//			if (acquire_sem_etc(conn_sem,1,B_ABSOLUTE_TIMEOUT,100000)!=B_OK)
//				continue;
			timeout=current->last_trans_time+current->time_to_live;
			if (Connected(current)) {
				//delete these lines if this doesn't work
//				if (current->usessl) {
//					char *stateinfo=(char*)SSL_state_string_long(current->ssl);
//					printf("SSL State: %s\n",stateinfo);
					//free(stateinfo);
//					stateinfo=NULL;
					
//				}
				
				if (DataWaiting(current)) {
					if (!current->callbackdone) {
//						acquire_sem(cb_sem);
//									printf("[TCP Manager] DataWaiting on connection %p\n",current);
						
						cur=callback_head;
						while (cur!=NULL) {
							if (cur->protocol==current->proto_id) {
								if (cur->Lock(100000)==B_OK) {
									atomic_add(&current->callbackdone,1);
									cur->callback(current);
									cur->Unlock();
								}
								
								break;
							}
							cur=cur->next;
						}
//						release_sem(cb_sem);
						
					}
					Unlock();
					snooze(80000);
					continue;
				}
				//delete above lines			
				if ((curtime>=timeout) && (curtime<(timeout+current->time_to_live))) {
					CloseConnection(current);
					lock->Unlock();
					snooze(10000);
					continue;
				} else {
					if ((curtime>=timeout) && (curtime>=(timeout+current->time_to_live))) {
						KillConnection(current);
						lock->Unlock();
						snooze(10000);
						continue;
					}
				}
			} else {
				if ((curtime>=timeout) && (curtime>=(timeout+current->time_to_live))) {
					KillConnection(current);
					lock->Unlock();
					snooze(10000);
					continue;
				}
			}
//			release_sem(conn_sem);
//cut back on processor time a bit... has little impact on performance						
		lock->Unlock();
		snooze(30000);
		} else	{
			lock->Unlock();
			snooze(30000);
			
		}

	}
	exit_thread(B_OK);
	return 0;
}
uint32 tcplayer::Connections() {
//	if (acquire_sem_etc(conn_sem,1,B_ABSOLUTE_TIMEOUT,100000)!=B_OK)
//		return 1;
	if (conn_head!=NULL) {
		uint32 count=0;
		connection *current=conn_head;
		while (current!=NULL) {
			count++;
			current=current->next;
		}
//		release_sem(conn_sem);
		return count;
	}
//	release_sem(conn_sem);
	return 0;
}
/*
Danger! Danger! Ugly monstrosity that seems to work! Needs to be rewritten!!!
*/
connection* tcplayer::ConnectTo(int32 protoid,char *host,int16 port, bool ssl, bool forcenew) {
	if (host==NULL)
		return NULL;
	if (strlen(host)<4)
		return NULL;
	bool ilocked=false;
		if (!lock->IsLocked()) {
			ilocked=true;
			lock->Lock();
			
		}
		

	printf("Requestor wants to connect to %s:%d\n",host,port);
	connection *conn=NULL;
	int32 sockproto=0;
#if USEBONE
		sockproto=IPPROTO_TCP;
#endif
	if (forcenew) {
		printf("[forcenew]");
		
		NewConnectStruct:
		printf("\tNew Connection\n");
		
		connection *cur=conn_head;
			conn=new connection;
		if (cur==NULL) {
			conn_head=conn;
			cur=conn;
		} else {
			while (cur->next!=NULL)
				cur=cur->next;
			cur->next=conn;
			cur=cur->next;
		}
		//mtx->lock();
		conn->socket=socket(AF_INET,SOCK_STREAM,sockproto);
		sockaddr_in servaddr;
//		current->address->GetAddr(servaddr);
		conn->hptr=gethostbyname(host);
		if (conn->hptr==NULL) {
			//the name can't be found...
			KillConnection(conn);
			return NULL;
		}
		
		conn->pptr=(struct in_addr**)conn->hptr->h_addr_list;
		memcpy(&servaddr.sin_addr,*conn->pptr,sizeof(struct in_addr));
		servaddr.sin_port=htons(port);
		conn->result=connect(conn->socket,(sockaddr *)&servaddr,sizeof(servaddr));
		//mtx->unlock();
		if (conn->result==0) 
			conn->open=true;
		else 
			conn->open=false;
			printf("Connected? %s\n",conn->open?"yes":"no");
	} else {
		connection *cur=conn_head;
		while (cur!=NULL) {
			printf("desired host: %s\ncur->host: %s\n",host,cur->addrstr.String());
			
			if ((strcasecmp(host,cur->addrstr.String())==0) && (cur->port==port))
				break;
			cur=cur->next;
		}
		printf("cur: %p\n",cur);
		
		if (cur==NULL)
			goto NewConnectStruct;
		if (cur->requests!=0) {
			printf("found existing connection, but it's busy.\n");
			goto NewConnectStruct;
		}
		printf("found existing connection, it's available.\n");
		
		if (!Connected(cur,true)) {
			cur->socket=socket(AF_INET,SOCK_STREAM,sockproto);
			sockaddr_in servaddr;
			memcpy(&servaddr.sin_addr,*cur->pptr,sizeof(struct in_addr));
			servaddr.sin_port=htons(port);
			cur->result=connect(cur->socket,(sockaddr *)&servaddr,sizeof(servaddr));
			if (cur->result==0)
				cur->open=true;
			else
				cur->open=false;
			printf("Connected? %s\n",cur->open?"yes":"no");
		} 
		conn=cur;
		
	}
	if (conn!=NULL) {
	conn->proto_id=protoid;
	conn->addrstr=host;
		conn->port=port;
#ifdef USEOPENSSL
		conn->usessl=ssl;
#endif		
		if (!conn->open) {
			if (ilocked)
				lock->Unlock();
			
			return conn;
		}
		
		if (ssl) {
#ifdef USEOPENSSL
				int flags=fcntl(conn->socket,F_GETFL,0);
				flags|=O_NONBLOCK;
				fcntl(conn->socket,F_SETFL,flags);
				printf("Uses SSL...\n");
				conn->usessl=true;	
				printf("Creating new context...");
				fflush(stdout);
				printf("done.\n");
				printf("Creating new ssl object...");
				fflush(stdout);
				conn->ssl = SSL_new (sslctx);
				printf("done.\n");
				SSL_set_cipher_list(conn->ssl, SSL_TXT_ALL);
				conn->sslbio=BIO_new_socket(conn->socket,BIO_NOCLOSE);
				SSL_set_bio(conn->ssl,conn->sslbio,conn->sslbio);
			SSL_set_connect_state(conn->ssl);
				printf("SSL connecting...");
				fflush(stdout);
				conn->result = SSL_connect (conn->ssl);
				TryConnectAgain2:
				int err = SSL_connect (conn->ssl);
				if (SSL_get_error(conn->ssl,err)==SSL_ERROR_WANT_READ) {
					
					snooze(10000);
					goto TryConnectAgain2;
					
				}
				flags &= ~O_NONBLOCK;
				fcntl(conn->socket,F_SETFL,flags);
				printf("done.\n");
				if (conn->result==1) {
				SSL_CIPHER *cipher=SSL_get_current_cipher (conn->ssl);
					
				printf ("SSL connection using %s\n", SSL_CIPHER_get_name(cipher));
				int usedbits=0,cipherbits=0;
				usedbits=SSL_CIPHER_get_bits(cipher,&cipherbits);
				printf("SSL Cipher is %d bits, %d bits used.\n",cipherbits,usedbits);
				printf("Getting SSL certificate...");
						fflush(stdout);
						
				conn->server_cert = SSL_get_peer_certificate (conn->ssl);
					printf("done.\n");
	
		const char *s;
		SSL_CIPHER *c;
	
		
		c = SSL_get_current_cipher(conn->ssl);
		
		s = (char *) SSL_get_version(conn->ssl);
		printf("SSL version: %s\n",s);
		s = (char *) SSL_CIPHER_get_name(c);
		
		printf("SSL Cipher name: %s\n",s);
		int ssl_keylength=0;
		SSL_CIPHER_get_bits(c, &ssl_keylength);
		
		/*
		 * set to the secret key size for the export ciphers...
		 * SSLeay returns the total key size
		 */
		if(strncmp(s, "EXP-", 4) == 0)
			ssl_keylength = 40;
			printf("key length: %d\n",ssl_keylength);
				}
				else
				 {
				 	conn->result=SSL_get_error(conn->ssl, conn->result);
				 	printf("SSL Error: %ld\n",conn->result);
				 }
					if (conn->server_cert!=NULL) {
						
				char * str;
				printf("certificate: %p\n",conn->server_cert);
				printf("Getting SSL subject...");
				fflush(stdout);
				  str = X509_NAME_oneline (X509_get_subject_name (conn->server_cert),0,0);
				printf("done\n");
		  printf ("\t subject: %s\n", str);
	  free (str);
	  str = X509_NAME_oneline (X509_get_issuer_name  (conn->server_cert),0,0);
	  printf ("\t issuer: %s\n", str);
	  free (str);
					}
#else
			printf("tcplayer.cpp: SSL support has not been included in this binary.\n");
#endif
		}
	}
	if (ilocked)
		lock->Unlock();
	
	return conn;
}

/*
tcplayer::NextConnection cycles through all connection structures that are under the
tcplayer's command; primarily for processing purposes.
*/
connection *tcplayer::NextConnection() {
//	if (acquire_sem_etc(conn_sem,1,B_ABSOLUTE_TIMEOUT,100000)!=B_OK)
//		return NULL;
	connection *current=prev_conn;
	if (current==NULL) {
		current=conn_head;
		prev_conn=current;
//		release_sem(conn_sem);
		return current;
	} else {
		bool truth=false;
		connection *cur=conn_head;
		while (cur!=NULL) {
			if (cur==current) {
				truth=true;
				break;
			}
			cur=cur->next;
		}
		if (truth) {
			cur=conn_head;
			while (cur!=NULL) {
				if (cur==current->next) {
					truth=true;
					break;
				}
				cur=cur->next;
			}
			if (truth) {
				current=current->next;
				prev_conn=current;
			} else
				current=conn_head;
		} else
			current=conn_head;
//		release_sem(conn_sem);
		return current;		 
	}
}
void tcplayer::CloseConnection(connection *target) {
//	if (acquire_sem(conn_sem)!=B_OK)
//		return;
//mtx->lock();
printf("tcplayer closeconnection\n");
if (target->open) {
	
	closesocket(target->socket);
	target->socket=-1;
	target->open=false;
	
}
if (target->socket!=-1)
	target->socket=-1;
	
//mtx->unlock();
	target->open=false;
#ifdef USEOPENSSL
	if (target->usessl){
		if (target->server_cert!=NULL) {
			X509_free (target->server_cert);
			target->server_cert=NULL;
		}
		if (target->ssl!=NULL) {
			SSL_free (target->ssl);
			target->ssl=NULL;
		}
	}
#endif
	DRCallback_st *cur=callback_head;
	while (cur!=NULL) {
		if (cur->protocol==target->proto_id) {
			if ((target->closedcbdone==0) && (cur->connclosedcb!=NULL)){
				atomic_add(&target->closedcbdone,1);
printf("about to call connection closed callback %ld\n",target->closedcbdone);
				cur->connclosedcb(target);
			}
			break;
		}
		cur=cur->next;
	}
	
//	release_sem(conn_sem);
}

void tcplayer::KillConnection(connection *target) {
//	if (acquire_sem_etc(conn_sem,1,B_ABSOLUTE_TIMEOUT,100000)!=B_OK)
//		return;
	if (target==NULL)
		return;
	
	printf("Killing connection %p\n",target);
	
		if (target->open) {
//mtx->lock();
		closesocket(target->socket);
		target->socket=-1;
//mtx->unlock();
		target->open=false;
#ifdef USEOPENSSL
		if (target->usessl){
			if (target->server_cert!=NULL) {
				X509_free (target->server_cert);
				target->server_cert=NULL;
			}
			if (target->ssl!=NULL) {
				SSL_free (target->ssl);
				target->ssl=NULL;
			}
		}
#endif
	}
	connection *cur=conn_head;
	if (cur==target) {
		conn_head=target->next;
		delete cur;
	}
	else {
		while(cur!=NULL) {
			if (cur->next==target) {
				cur->next=target->next;
				delete target;
				break;
			}
			cur=cur->next;
		}
	}
//	release_sem(conn_sem);
}
int32 tcplayer::Send(connection **conn,unsigned char *data, int32 size) {
printf("Send\n");

	if ((*conn)->requests>0) {
			char host[250];
			int16 port;
//			(*conn)->address->GetAddr(host,(unsigned short*)&port);
#ifdef USEOPENSSL
			*conn=ConnectTo((*conn)->proto_id,(char*)(*conn)->addrstr.String(),(*conn)->port,(*conn)->usessl,true);
#else
			*conn=ConnectTo((*conn)->proto_id,(char*)(*conn)->addrstr.String(),(*conn)->port,false,true);
#endif
			atomic_add(&(*conn)->requests,1);
		
	}
	else

	 atomic_add(&(*conn)->requests,1);
	
//	printf("Checking connection status.\n");
	if (!Connected((*conn))) {
printf("Not connected (send)\n");
		
			char host[250];
			int16 port;
//			(*conn)->address->GetAddr(host,(unsigned short*)&port);
#ifdef USEOPENSSL
			*conn=ConnectTo((*conn)->proto_id,(char*)(*conn)->addrstr.String(),port,(*conn)->usessl);
#else
			*conn=ConnectTo((*conn)->proto_id,(char*)(*conn)->addrstr.String(),port,false);
#endif
	}
//	printf("post connection status check.\n");
	ssize_t sent=0;
#ifdef USEOPENSSL
	if ((*conn)->usessl) {
		//mtx->lock();
		sent=SSL_write((*conn)->ssl,(const char*)data,size);
		//mtx->unlock();
//		CHK_SSL(sent);
	}
	else {
#endif
		//mtx->lock();
		sent=send((*conn)->socket,data,size,0);
		//mtx->unlock();
#ifdef USEOPENSSL
	}
#endif	 
	if (sent<0) {
		(*conn)->result=errno;
	}
	else {
		(*conn)->last_trans_time=real_time_clock();
	}
//	atomic_add(&(*conn)->requests,-1);
//	if (sent==-1)
//		printf("Error condition: %ld\n",errno);
#ifdef USENETSERVER
	if (sent<=0) {
//		printf("connection terminated.\n");
		(*conn)->open=false;
			char host[250];
			int16 port=(*conn)->port;
//			(*conn)->address->GetAddr(host,(unsigned short*)&port);
#ifdef USEOPENSSL
			*conn=ConnectTo((*conn)->proto_id,(char*)(*conn)->addrstr.String(),port,(*conn)->usessl,true);
#else
			*conn=ConnectTo((*conn)->proto_id,(char*)(*conn)->addrstr.String(),port,false,true);
#endif
			atomic_add(&(*conn)->requests,1);
#ifdef USEOPENSSL
	if ((*conn)->usessl) {
		//mtx->lock();
		sent=SSL_write((*conn)->ssl,(const char*)data,size);
		//mtx->unlock();
//		CHK_SSL(sent);
	}
	else {
#endif
		//mtx->lock();
		sent=send((*conn)->socket,data,size,0);
		//mtx->unlock();
#ifdef USEOPENSSL
	}
#endif	 
	}
#endif
	return sent;
}
int32 tcplayer::Receive(connection **conn, unsigned char *data, int32 size) {
/*	if ((*conn)->requests>5) {
			char host[250];
			int16 port;
			(*conn)->address->GetAddr(host,(unsigned short*)&port);
			*conn=ConnectTo(host,port,(*conn)->usessl,true);
			atomic_add(&(*conn)->requests,1);
		
	}
	else*/
//	 atomic_add(&(*conn)->requests,1);
//	printf("TCP layer Receive\n");
	int option=1;
	//mtx->lock();
	setsockopt((*conn)->socket,SOL_SOCKET,SO_NONBLOCK,&option,sizeof(option));
	//mtx->unlock();
	ssize_t got=0;
	if (Connected((*conn))) {
#ifdef USEOPENSSL
		if ((*conn)->usessl) {
			//mtx->lock();
			got=SSL_read ((*conn)->ssl,(char*)data,size);
			//mtx->unlock();
//			CHK_SSL(got);
		} else {
#endif
			//mtx->lock();
			got=recv((*conn)->socket,data,size,0);
//			printf("%ld bytes received\n",got);
			//mtx->unlock();
#ifdef USEOPENSSL
		}	
#endif
		if (got<0) {
			(*conn)->result=errno;
		} else {
			if (got>0)
				(*conn)->last_trans_time=real_time_clock();
		}
	} else {
#ifdef USEOPENSSL
		if ((*conn)->usessl) {
			//mtx->lock();
			got=SSL_read((*conn)->ssl,(char*)data,size);
			//mtx->unlock();
//			CHK_SSL(got);
		} else {
#endif
			//mtx->lock();
			got=recv((*conn)->socket,data,size,0);
			//mtx->unlock();
#ifdef USEOPENSSL
		}
#endif
		if (got<0) {
			(*conn)->result=errno;
		} else {
			if (got>0)
				(*conn)->last_trans_time=real_time_clock();
		}
//		got=-1;
	}
	
	if ((*conn)->callbackdone) {
		atomic_add(&(*conn)->callbackdone,-1);
	}
//	if ((*conn)->requests==1)
//		atomic_add(&(*conn)->requests,-1);
	option=0;
	//mtx->lock();
	setsockopt((*conn)->socket,SOL_SOCKET,SO_NONBLOCK,&option,sizeof(option));
	//mtx->unlock();
//	if (got==-1)
//		printf("Error condition: %ld\n",errno);
#ifdef USENETSERVER
	if (got<=0) {
//		printf("connection terminated.\n");
		CloseConnection((*conn));
//		 (*conn)->open=false;
	}
#endif
	return got;
}

void tcplayer::RequestDone(connection *conn,bool close) {
	if (conn!=NULL) {
		atomic_add(&conn->requests,-1);
		if (conn->closedcbdone==0) {
			
			if (conn->requests>0)
				conn->requests=0;
			if ((close) && (!conn->closedcbdone)) {
				atomic_add(&conn->closedcbdone,1);
				CloseConnection(conn);
			}
			
		}
		
	}
}

bool tcplayer::DataWaiting(connection *conn) {
	if (IsValid(conn)) {
//	bool open=false;
		
//	if ((open=Connected(conn))) {
//	if ((open) && (conn->open==0))
//		printf("[DataWaiting] Connection status reports differ\n");
	
	int option=1;
	//mtx->lock();
	setsockopt(conn->socket,SOL_SOCKET,SO_NONBLOCK,&option,sizeof(option));
		
	struct timeval tv;
	tv.tv_sec=2;
	tv.tv_usec=0;
	struct fd_set fds;
	FD_ZERO(&fds);
	FD_SET(conn->socket,&fds);
	select(32,&fds,NULL,NULL,&tv);
	option=0;
	setsockopt(conn->socket,SOL_SOCKET,SO_NONBLOCK,&option,sizeof(option));
	bool answer=false;
	answer=FD_ISSET(conn->socket,&fds);
	if (answer) {
      char szBuffer[5] = ""; 
 #ifdef BONE_VERSION
		int iret = recv( conn->socket, szBuffer, 4, MSG_PEEK ); 
		if (iret>0)
			answer=true;
		else
			answer=false;
#endif
	}
	//mtx->unlock();
	return answer;
		
	}
	else {
		return false;
	}
	
}
bool tcplayer::IsValid(connection *conn) {
//	if (acquire_sem(conn_sem)!=B_OK)
//		return false;
	bool truth=false;
	connection *cur=conn_head;
	while (cur!=NULL) {
		if (cur==conn) {
			truth=true;
			break;
		}
		cur=cur->next;
	}
//	release_sem(conn_sem);
	return truth;
}
bool tcplayer::Connected(connection *conn,bool skipvalid) {
	if (conn!=NULL) {
		if (skipvalid) {//no need to check validation
			CheckConnectStatus://connection status checking starts here.
			if (conn->socket<0)
				return false;
			bool oldstatus=conn->open;
			int iret=0;
			bool bOK=true;
			int option=1;
			setsockopt(conn->socket,SOL_SOCKET,SO_NONBLOCK,&option,sizeof(option));
			struct timeval timeout={0,10000};
			fd_set readsocketset;
			FD_ZERO(&readsocketset);
			FD_SET(conn->socket,&readsocketset);
			iret=select(32,&readsocketset,NULL,NULL,&timeout);
			bOK=(iret>0);
			if (bOK){
				bOK = FD_ISSET(conn->socket, &readsocketset );
			}
#ifndef USENETSERVER
/*
	Check to see if we're using a network stack other than net_server
	in preparation for OpenBeOS. This means this code should work on both
	BONE and OpenBeOS. In theory.
*/
			if (bOK) {
				char szBuffer[1] = "";
				iret = recv( conn->socket, szBuffer, 1, MSG_PEEK ); 
				bOK = ( iret > 0 ); 
				if( !bOK ) { 
					int iError = errno;
					if  ((iError==ENETUNREACH )||(iError==ECONNREFUSED)||(iError==ECONNRESET)||(iError==EBADF)||(iret==0)) {
						conn->open = false; //Graceful disconnect from other side. 
						CloseConnection(conn);
					}
				}
				else
					conn->open=true; 
			}
#endif
			option=0;
			setsockopt(conn->socket,SOL_SOCKET,SO_NONBLOCK,&option,sizeof(option));
			if (oldstatus)
				if (!conn->open)
					printf("Connection %p on socket %ld closed\n",conn,conn->socket);
		} else {
			if ( IsValid(conn))
				goto CheckConnectStatus; //validation successful
			else
				return false; //validation check failed... don't check
		}
		//return the actual status
		return conn->open;
	}
	//conn is NULL, so this is obviously invalid
	return false;
}
/*
//found this code at http://www.codeguru.com/network/ChatSource.shtml
BOOL CClientSocket::HasConnectionDropped( void ) 
{ 
        BOOL bConnDropped = FALSE; 
        INT iRet = 0; 
        BOOL bOK = TRUE; 
        
        struct timeval timeout = { 0, 0 }; 
        fd_set readSocketSet; 
        
        FD_ZERO( &readSocketSet ); 
        FD_SET( m_hSocket, &readSocketSet ); 
        
        iRet = ::select( 0, &readSocketSet, NULL, NULL, &timeout ); 
        bOK = ( iRet > 0 ); 
        
        if( bOK ) 
        { 
                bOK = FD_ISSET( m_hSocket, &readSocketSet ); 
        } 
        
        if( bOK ) 
        { 
                CHAR szBuffer[1] = ""; 
                iRet = ::recv( m_hSocket, szBuffer, 1, MSG_PEEK ); 
                bOK = ( iRet > 0 ); 
                if( !bOK ) 
                { 
                        INT iError = ::WSAGetLastError(); 
                        bConnDropped = ( ( iError == WSAENETRESET ) || 
                                ( iError == WSAECONNABORTED ) || 
                                ( iError == WSAECONNRESET ) || 
                                ( iError == WSAEINVAL ) || 
                                ( iRet == 0 ) ); //Graceful disconnect from other side. 
                } 
        } 
        
    return( bConnDropped ); 
}
*/
status_t tcplayer::Quit() {
	Lock();
	printf("tcp_layer stopping\n");
	atomic_add(&quit,1);
	status_t status=B_OK;
	wait_for_thread(thread,&status);
	release_sem(tcp_mgr_sem);
	return status;
}
void tcplayer::SetConnectionClosedCB(int32 proto, void (*connclosedcb)(connection *conn)) 
{
	if ((callback_head==NULL) || (firstcb==1)) {
		callback_head=new DRCallback_st;
		callback_head->connclosedcb=connclosedcb;
		callback_head->protocol=proto;
		atomic_add(&firstcb,-1);
	} else {
		DRCallback_st *cur=callback_head;
		DRCallback_st *last=NULL;
		
		while (cur!=NULL) {
			if (cur->protocol==proto)
				break;
			last=cur;
			cur=cur->next;
		}
		if (cur==NULL) {
			last->next=cur=new DRCallback_st;
			cur->connclosedcb=connclosedcb;
			cur->protocol=proto;

		} else {
			cur->connclosedcb=connclosedcb;
		}
		
	}
	
	
}

void tcplayer::SetDRCallback(int32 proto,void (*DataReceived)(connection *conn),int32 (*Lock)(int32 timeout),void(*Unlock)(void)) {
//	acquire_sem(cb_sem);
	printf("SetDRCallback: %p\n",callback_head);
	if ((callback_head==NULL) || (firstcb==1)) {
		callback_head=new DRCallback_st;
		callback_head->protocol=proto;
		callback_head->callback=DataReceived;	
		callback_head->Lock=Lock;
		callback_head->Unlock=Unlock;
		atomic_add(&firstcb,-1);	
	}
	else {
		DRCallback_st *cur=callback_head;
		bool found=false;
		while (cur->next!=NULL) {
			if (proto==cur->protocol) {
				found=true;
				break;
			}
			cur=cur->next;
		}
		if (!found) {
			cur->next=new DRCallback_st;
			cur=cur->next;
		} else {
			if (cur->callback!=DataReceived) {
				
				cur->callback=DataReceived;
			}
			if (cur->Lock!=Lock)
				cur->Lock=Lock;
			if (cur->Unlock!=Unlock)
				cur->Unlock=Unlock;
			return;
		}
		
		cur->protocol=proto;
		cur->callback=DataReceived;
	}
//	release_sem(cb_sem);
}
/*
//this was going to be the start of something beautiful:
//a signal based method of detecting waiting data...
//but apparently neither bone nor net_server support
//some of the vitals described in Stevens' TCP/IP Unix Networking book...
void tcplayer::sig_io(int action) {
	printf("data waiting\n");
	
}
*/
