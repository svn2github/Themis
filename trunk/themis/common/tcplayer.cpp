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
		connection *cur=conn_head, *tmp;
		while (cur!=NULL) {
			tmp=cur->next;
			delete cur;
			cur=tmp;
		}
	}
//	acquire_sem(cb_sem);
	if (callback_head!=NULL) {
		DRCallback_st *cur=callback_head;
		while (callback_head!=NULL) {
			cur=callback_head->next;
			delete callback_head;
			callback_head=cur;
		}
		
	}
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
								atomic_add(&current->callbackdone,1);
								if (cur->Lock(100000)==B_OK) {
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
//	if (acquire_sem(conn_sem)!=B_OK)
//		return NULL;
	if (forcenew) {
		connection *nu=new connection;
		nu->proto_id=protoid;
		nu->addrstr=host;
		nu->address->SetTo(host,port);
		nu->socket=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
		sockaddr_in servaddr;
		//mtx->lock();
		nu->address->GetAddr(servaddr);
		nu->result=connect(nu->socket,(sockaddr *)&servaddr,sizeof(servaddr));
#ifdef USEOPENSSL
		if (ssl){
			int flags=fcntl(nu->socket,F_GETFL,0);
			flags|=O_NONBLOCK;
			fcntl(nu->socket,F_SETFL,flags);
			nu->usessl=true;	
			nu->ssl = SSL_new (sslctx);
			if (nu->ssl==NULL)
				printf("SSL creation error (nu)\n");
			SSL_set_cipher_list(nu->ssl, SSL_TXT_ALL);
			nu->sslbio=BIO_new_socket(nu->socket,BIO_NOCLOSE);
			SSL_set_bio(nu->ssl,nu->sslbio,nu->sslbio);
			SSL_set_connect_state(nu->ssl);
			
//			SSL_set_fd (nu->ssl, nu->socket);
			TryConnectAgain1:
			nu->result=SSL_connect (nu->ssl);
			int err = nu->result;
			if (SSL_get_error(nu->ssl,err)==SSL_ERROR_WANT_READ) {
				
				snooze(10000);
				goto TryConnectAgain1;
				
			}
			flags &= ~O_NONBLOCK;
			fcntl(nu->socket,F_SETFL,flags);
//			CHK_SSL(err);
			SSL_CIPHER *cipher=SSL_get_current_cipher (nu->ssl);
			printf ("SSL connection using %s\n",SSL_CIPHER_get_name(cipher) );
			int usedbits=0,cipherbits=0;
			usedbits=SSL_CIPHER_get_bits(cipher,&cipherbits);
			printf("SSL Cipher is %d bits, %d bits used.\n",cipherbits,usedbits);
			printf("Getting SSL certificate...");
					fflush(stdout);
					
			nu->server_cert = SSL_get_peer_certificate (nu->ssl);
				printf("done.\n");
//			CHK_NULL(nu->server_cert);
			char * str=NULL;
			printf("certificate: %p\n",nu->server_cert);
			if (nu->server_cert!=NULL) {
				
			printf("Getting SSL subject...");
			fflush(stdout);
			
			  str = X509_NAME_oneline (X509_get_subject_name (nu->server_cert),0,0);
			printf("done\n");
//  CHK_NULL(str);
  printf ("\t subject: %s\n", str);
  free (str);
  str = X509_NAME_oneline (X509_get_issuer_name  (nu->server_cert),0,0);
//  CHK_NULL(str);
  printf ("\t issuer: %s\n", str);
  free (str);
			}
		//mtx->unlock();
		}
#endif			
		if (nu->result<0) {
			nu->result=errno;
		}
		else {
			 nu->open=true;
			 printf("[Forced] New connection is on socket: %ld\n",nu->socket);
			
		}
		if (conn_head==NULL) {
			conn_head=nu;
			
		}
		else
		{
			connection *cur=conn_head;
			while(cur->next!=NULL)
				cur=cur->next;
			cur->next=nu;
			
		}
		nu->last_trans_time=real_time_clock();
		
//		release_sem(conn_sem);
		return nu;
	}
	
	connection *current=conn_head;
	char curhost[250];
	uint16 curport=80;
	char host2[250];
	{
		BNetAddress addr(host,port);
		uint16 tport;
		
		addr.GetAddr(host2,&tport);
		
	}
	
	while (current!=NULL){
		current->address->GetAddr(curhost,&curport);
		if (/*(strcasecmp(curhost,host)==0)*/(strcasecmp(curhost,host2)==0) && (curport==port))
			break;
		current=current->next;		
	}
	if (current==NULL) {	
		current=new connection;
		current->proto_id=protoid;
		current->addrstr=host;
		current->address->SetTo(host,port);
		connection *cur=conn_head;
		if (cur==NULL){
			conn_head=current;
			cur=conn_head;
			
		}
		else {
			
			while (cur->next!=NULL)
				cur=cur->next;
			cur->next=current;
		}
			printf("Adding new connection\n");
		int32 sockproto=IPPROTO_TCP;
#if USENETSERVER
		sockproto=0;
#endif
		//mtx->lock();
		current->socket=socket(AF_INET,SOCK_STREAM,sockproto);
		sockaddr_in servaddr;
		current->address->GetAddr(servaddr);
		current->result=connect(current->socket,(sockaddr *)&servaddr,sizeof(servaddr));
		//mtx->unlock();
		if (current->result==0) {
			current->open=true;
		} else {
			current->open=false;
			printf("error: %d\n",errno);
		}
		printf("Connected? %s\n",current->open?"yes":"no");
/*		if (!Connected(current,true)) {
			current->open=false;
				
			return current;
		}
*/		
#ifdef USEOPENSSL
		if (ssl){
//mtx->lock();
			int flags=fcntl(current->socket,F_GETFL,0);
			flags|=O_NONBLOCK;
			fcntl(current->socket,F_SETFL,flags);
			printf("Uses SSL...\n");
			current->usessl=true;	
			printf("Creating new context...");
			fflush(stdout);
//			SSL_CTX_set_options(current->sslctx,SSL_OP_ALL);
//			SSL_CTX_set_default_verify_paths(current->sslctx);
			printf("done.\n");
			printf("Creating new ssl object...");
			fflush(stdout);
			current->ssl = SSL_new (sslctx);
			printf("done.\n");
			SSL_set_cipher_list(current->ssl, SSL_TXT_ALL);
			current->sslbio=BIO_new_socket(current->socket,BIO_NOCLOSE);
			SSL_set_bio(current->ssl,current->sslbio,current->sslbio);
/*
		#ifndef BONE_VERSION
		current->sslbio=BIO_new_socket(current->socket,BIO_NOCLOSE);
		if (current->sslbio==0)
			printf("bio creation error\n");
		SSL_set_bio(current->ssl,current->sslbio,current->sslbio);
		#else
			printf("Setting ssl fd...");
			fflush(stdout);
			SSL_set_fd (current->ssl, current->socket);
			printf("done.\n");
		#endif

*/
		SSL_set_connect_state(current->ssl);
			printf("SSL connecting...");
			fflush(stdout);
			current->result = SSL_connect (current->ssl);
			TryConnectAgain2:
			int err = SSL_connect (current->ssl);
			if (SSL_get_error(current->ssl,err)==SSL_ERROR_WANT_READ) {
				
				snooze(10000);
				goto TryConnectAgain2;
				
			}
			flags &= ~O_NONBLOCK;
			fcntl(current->socket,F_SETFL,flags);
			printf("done.\n");
//			CHK_SSL(err);
			if (current->result==1) {
			SSL_CIPHER *cipher=SSL_get_current_cipher (current->ssl);
				
			printf ("SSL connection using %s\n", SSL_CIPHER_get_name(cipher));
			int usedbits=0,cipherbits=0;
			usedbits=SSL_CIPHER_get_bits(cipher,&cipherbits);
			printf("SSL Cipher is %d bits, %d bits used.\n",cipherbits,usedbits);
			printf("Getting SSL certificate...");
					fflush(stdout);
					
			current->server_cert = SSL_get_peer_certificate (current->ssl);
				printf("done.\n");

	const char *s;
	SSL_CIPHER *c;

	
	c = SSL_get_current_cipher(current->ssl);
	
	s = (char *) SSL_get_version(current->ssl);
	printf("SSL version: %s\n",s);
//	free(s);
	s = (char *) SSL_CIPHER_get_name(c);
	
	printf("SSL Cipher name: %s\n",s);
//	free(s);	
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
			 	current->result=SSL_get_error(current->ssl, current->result);
			 	printf("SSL Error: %ld\n",current->result);
			 }
				if (current->server_cert!=NULL) {
					
//			CHK_NULL(current->server_cert);
			char * str;
			printf("certificate: %p\n",current->server_cert);
			printf("Getting SSL subject...");
			fflush(stdout);
			  str = X509_NAME_oneline (X509_get_subject_name (current->server_cert),0,0);
			printf("done\n");
//  CHK_NULL(str);
  printf ("\t subject: %s\n", str);
  free (str);
  str = X509_NAME_oneline (X509_get_issuer_name  (current->server_cert),0,0);
//  CHK_NULL(str);
  printf ("\t issuer: %s\n", str);
  free (str);
				}
//mtx->unlock();				
		}
#endif
		
		if (current->result<0) {
			current->result=errno;
		}
		else {
			 current->open=true;
			 printf("New connection is on socket: %ld\n",current->socket);
			
		}
		
		current->last_trans_time=real_time_clock();
//		release_sem(conn_sem);
		
		return current;
	}
	else {
		printf("Reusing existing connection\n");
		if (!current->open) {
//			if (!Connected(current)) {
				sockaddr_in servaddr;
				current->address->GetAddr(servaddr);
				current->socket=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
//mtx->lock();
				current->result=connect(current->socket,(sockaddr *)&servaddr,sizeof(servaddr));
//mtx->unlock();
/*			if (!Connected(current,true)) {
				current->open=false;
					
				return current;
			}
*/			
#ifdef USEOPENSSL
				if (ssl){
				//mtx->lock();
			int flags=fcntl(current->socket,F_GETFL,0);
			flags|=O_NONBLOCK;
			fcntl(current->socket,F_SETFL,flags);
			current->usessl=true;	
			current->ssl = SSL_new (sslctx);
			SSL_set_cipher_list(current->ssl, SSL_TXT_ALL);
			current->sslbio=BIO_new_socket(current->socket,BIO_NOCLOSE);
			SSL_set_bio(current->ssl,current->sslbio,current->sslbio);
			SSL_set_connect_state(current->ssl);
			TryConnectAgain3:
					current->result=SSL_connect (current->ssl);
			int err = current->result;
			if (SSL_get_error(current->ssl,err)==SSL_ERROR_WANT_READ) {
				
				snooze(10000);
				goto TryConnectAgain3;
				
			}
				
			flags &= ~O_NONBLOCK;
			fcntl(current->socket,F_SETFL,flags);

//			CHK_ERR(err,current->ssl);
			SSL_CIPHER *cipher=SSL_get_current_cipher (current->ssl);
					
			printf ("SSL connection using %s\n", SSL_CIPHER_get_name(cipher));
			int usedbits=0,cipherbits=0;
			usedbits=SSL_CIPHER_get_bits(cipher,&cipherbits);
			printf("SSL Cipher is %d bits, %d bits used.\n",cipherbits,usedbits);
			printf("Getting SSL certificate...");
					fflush(stdout);
					
			current->server_cert = SSL_get_peer_certificate (current->ssl);
				printf("done.\n");
					if (current->server_cert!=NULL) {
						
//			CHK_NULL(current->server_cert);
			char * str;
			printf("certificate: %p\n",current->server_cert);
			printf("Getting SSL subject...");
			fflush(stdout);
					
			  str = X509_NAME_oneline (X509_get_subject_name (current->server_cert),0,0);
			printf("done\n");
//  CHK_NULL(str);
  printf ("\t subject: %s\n", str);
  free (str);
  str = X509_NAME_oneline (X509_get_issuer_name  (current->server_cert),0,0);
//  CHK_NULL(str);
  printf ("\t issuer: %s\n", str);
  free (str);

		}
//mtx->unlock();
				}
#endif
				
				
//			}
			
			if (current->result<0) {
				current->result=errno;
			}
			else {
				 current->open=true;
				 
			}
		}
		
		current->last_trans_time=real_time_clock();
//		release_sem(conn_sem);
		return current;
	}
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
	closesocket(target->socket);
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
//	release_sem(conn_sem);
}

void tcplayer::KillConnection(connection *target) {
//	if (acquire_sem_etc(conn_sem,1,B_ABSOLUTE_TIMEOUT,100000)!=B_OK)
//		return;
	if (target->open) {
//mtx->lock();
		closesocket(target->socket);
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
			(*conn)->address->GetAddr(host,(unsigned short*)&port);
#ifdef USEOPENSSL
			*conn=ConnectTo((*conn)->proto_id,host,port,(*conn)->usessl,true);
#else
			*conn=ConnectTo((*conn)->proto_id,host,port,false,true);
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
			(*conn)->address->GetAddr(host,(unsigned short*)&port);
#ifdef USEOPENSSL
			*conn=ConnectTo((*conn)->proto_id,host,port,(*conn)->usessl);
#else
			*conn=ConnectTo((*conn)->proto_id,host,port,false);
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
			int16 port;
			(*conn)->address->GetAddr(host,(unsigned short*)&port);
#ifdef USEOPENSSL
			*conn=ConnectTo((*conn)->proto_id,host,port,(*conn)->usessl,true);
#else
			*conn=ConnectTo((*conn)->proto_id,host,port,false,true);
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
		(*conn)->open=false;
	}
#endif
	return got;
}

void tcplayer::RequestDone(connection *conn) {
	if (conn!=NULL) {
		atomic_add(&conn->requests,-1);
		if (conn->requests>0)
			conn->requests=0;
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
	if (skipvalid) {
		bool oldstatus=conn->open;
		
//	printf("is valid\n");
/*		if ((FD_ISSET(conn->socket,&fds)) && (FD_ISSET(conn->socket,&fdr))) {//(send(conn->socket,&test,1,0)>=0) {
			conn->open=true;
		}
		else
			conn->open=false;
*/
//		printf("connected: %d\n",conn->open);
	int iret=0;
	bool bOK=true;
	int option=1;
	//mtx->lock();
	setsockopt(conn->socket,SOL_SOCKET,SO_NONBLOCK,&option,sizeof(option));
	//mtx->unlock();
//		printf("checking connection status...");
//		fflush(stdout);
		
//		if (err<0)
//			printf("err %d\n",err);
		
		struct timeval timeout={0,10000};
		fd_set readsocketset;
		//mtx->lock();
		FD_ZERO(&readsocketset);
		FD_SET(conn->socket,&readsocketset);
		iret=select(32,&readsocketset,NULL,NULL,&timeout);
		//mtx->unlock();
		bOK=(iret>0);
		
		if (bOK){
		//mtx->lock();
		bOK = FD_ISSET(conn->socket, &readsocketset );
		//mtx->unlock();
		}
		
        if( bOK ) 
        { 
                char szBuffer[1] = ""; 
#ifdef BONE_VERSION
                //mtx->lock();
                iret = recv( conn->socket, szBuffer, 1, MSG_PEEK ); 
                //mtx->unlock();
//				printf("iret: %d\n",iret);
//				if (iret==0)
//					printf("error: %d\n",errno);
			
                bOK = ( iret > 0 ); 
                if( !bOK ) 
                { 
                        int iError = errno;
//						printf("socket %ld error\n",conn->socket);
						if  ((iError==ENETUNREACH )||(iError==ECONNREFUSED)||(iError==ECONNRESET)||(iError==EBADF)||(iret==0))
						{
							
                        conn->open = false; //Graceful disconnect from other side. 
						CloseConnection(conn);
							
						}
						
                }
				else
					conn->open=true; 
#else
/*
	char nul='\0';
	iret=send(conn->socket,&nul,1,0);
	if (iret==-1) {
		conn->open=false;
		}else {
			conn->open=true;
		}
*/
#endif
        }
		option=0;
//		printf("%s\n",conn->open ? "connected":"disconnected");
		//mtx->lock();
		setsockopt(conn->socket,SOL_SOCKET,SO_NONBLOCK,&option,sizeof(option));
		//mtx->unlock();
		if (oldstatus)
			if (!conn->open)
				printf("Connection %p on socket %ld closed\n",conn,conn->socket);
		
	return conn->open;
		
	}
	else{
		if ( IsValid(conn)) {
		bool oldstatus=conn->open;
		
//	printf("is valid\n");
/*		if ((FD_ISSET(conn->socket,&fds)) && (FD_ISSET(conn->socket,&fdr))) {//(send(conn->socket,&test,1,0)>=0) {
			conn->open=true;
		}
		else
			conn->open=false;
*/
//		printf("connected: %d\n",conn->open);
	int iret=0;
	bool bOK=true;
	int option=1;
//		printf("checking connection status...");
//		fflush(stdout);
	//mtx->lock();	
	/*int err=*/setsockopt(conn->socket,SOL_SOCKET,SO_NONBLOCK,&option,sizeof(option));
	//mtx->unlock();
//		if (err<0)
//			printf("err %d\n",err);
		
		struct timeval timeout={0,10000};
		//mtx->lock();
		fd_set readsocketset;
		FD_ZERO(&readsocketset);
		FD_SET(conn->socket,&readsocketset);
		iret=select(32,&readsocketset,NULL,NULL,&timeout);
		//mtx->unlock();
		bOK=(iret>0);
		
		if (bOK){
		//mtx->lock();
		bOK = FD_ISSET(conn->socket, &readsocketset );
		//mtx->unlock();
		}
		
        if( bOK ) 
        { 
                char szBuffer[1] = ""; 
#ifdef BONE_VERSION
        //mtx->lock();
                iret = recv( conn->socket, szBuffer, 1, MSG_PEEK ); 
        //mtx->unlock();
//				printf("iret: %d\n",iret);
//				if (iret==0)
//					printf("error: %d\n",errno);
			
                bOK = ( iret > 0 ); 
                if( !bOK ) 
                { 
                        int iError = errno;
//						printf("socket %ld error\n",conn->socket);
						if  ((iError==ENETUNREACH )||(iError==ECONNREFUSED)||(iError==ECONNRESET)||(iError==EBADF)||(iret==0))
						{
							
                        conn->open = false; //Graceful disconnect from other side. 
						CloseConnection(conn);
							
						}
						
                }
				else
					conn->open=true; 
#else
/*
	char nul='\0';
	iret=send(conn->socket,&nul,1,0);
	if (iret==-1) {
		conn->open=false;
		}else {
			conn->open=true;
		}
*/
#endif
        }
		option=0;
//		printf("%s\n",conn->open ? "connected":"disconnected");
		//mtx->lock();
		setsockopt(conn->socket,SOL_SOCKET,SO_NONBLOCK,&option,sizeof(option));
		//mtx->unlock();
		if (oldstatus)
			if (!conn->open)
				printf("Connection %p on socket %ld closed\n",conn,conn->socket);
		
	return conn->open;
		
	}
	}
	
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
