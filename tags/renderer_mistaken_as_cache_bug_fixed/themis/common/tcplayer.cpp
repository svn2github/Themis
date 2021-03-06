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
#include <Autolock.h>
#include "protocol_plugin.h"
tcplayer *meTCP;
int32 TL;
int32 TU;
bool tcplayer::SSLSupported() {
	//BAutolock alock(lock);
	bool supported=false;
#ifdef USEOPENSSL
	supported=true;
#endif
	return supported;
}
char *tcplayer::SSLAboutString() {
	//BAutolock alock(lock);
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
	lock=new BLocker("tcp lock",true);
	quit=0;
	tcp_mgr_sem=create_sem(1,"tcp_layer_manager_sem");
	tcplayer_sem=create_sem(1,"tcplayer_sem");
	callback_head=NULL;	
	firstcb=0;
	atomic_add(&firstcb,1);
	conn_head=NULL;
	conn_queue=NULL;
	prev_conn=NULL;
#ifdef USEOPENSSL
	//required copyright notices and ego boosters
	printf("This product includes software developed by the OpenSSL Project for use in the OpenSSL Toolkit (http://www.openssl.org/)\nCopyright (c) 1998-2000 The OpenSSL Project.  All rights reserved.\nThis product includes cryptographic software written by Eric Young (eay@cryptsoft.com)\nCopyright (C) 1995-1998 Eric Young (eay@cryptsoft.com)\nAll rights reserved.\n");
	printf("tcplayer constructor\n");
	static int32 initcount = 0;
	static bool initialized = false;
	
	if (atomic_or(&initcount, 1) == 0) {
#ifdef DEBUG
	printf("Initializing SSL Library\n");
#endif
		
		SSL_library_init();
#ifdef DEBUG
		printf("loading error strings.\n");
#endif
		
		SSL_load_error_strings();
		initialized = true;
	} else {
		while (!initialized) snooze(50000);
	}
#ifdef DEBUG
printf("loading ssl algorithms\n");
#endif
	
	SSLeay_add_ssl_algorithms();
#ifdef DEBUG
printf("creating ssl client method\n");
#endif
	
		sslmeth = SSLv23_client_method();
#ifdef DEBUG
printf("creating ssl context\n");
#endif
	
		sslctx=SSL_CTX_new (sslmeth);
#ifdef DEBUG
		printf("setting ssl context options\n");
#endif
	
	SSL_CTX_set_options(sslctx, SSL_OP_ALL);
#ifdef DEBUG
	printf("setting default verify paths\n");
#endif
	
	SSL_CTX_set_default_verify_paths(sslctx);
#ifdef DEBUG
	if (sslctx==NULL)
		printf("SSL Context creation error\n");
#endif
	unsigned char buf[17];
	memset(buf,0,17);
#ifdef DEBUG
	printf("doing rand stuff...\n");
#endif
	RAND_egd("/tmp/test.rnd");
	
	 do{
#ifdef DEBUG
		printf("rand while loop\n");
#endif
		
		for (int i=0;i<16;i++) {
#ifdef DEBUG
			printf("rand for loop\n");
#endif
			
			srand(real_time_clock());
			buf[i]=(real_time_clock_usecs()/system_time())%128+(rand()%128);
		}
		RAND_seed(buf,16);
	}while(RAND_status()!=1);
	
#ifdef DEBUG
	printf("RAND status: %d\n",RAND_status());
#endif
#endif
}

tcplayer::~tcplayer() {
#ifdef DEBUG
	printf("tcp_layer destructor, %ld connections\n",Connections());
#endif
	if (conn_head!=NULL) {
		connection *cur=conn_head;
#ifdef DEBUG
		printf("tcplayer: deleting connections:\n");
#endif
		while (conn_head!=NULL) {
#ifdef DEBUG
			printf("tcplayer: deleting connection %p\n",cur);
#endif
			cur=conn_head->next;
			delete conn_head;
			conn_head=cur;
		}
	}
#ifdef DEBUG
printf("clearing out callbacks.\n");
#endif
	if (callback_head!=NULL) {
		DRCallback_st *cur=callback_head;
		while (callback_head!=NULL) {
			cur=callback_head->next;
			delete callback_head;
			callback_head=cur;
		}
		
	}
#ifdef DEBUG
printf("done clearing callbacks.\n");
#endif
#ifdef USEOPENSSL
	if (sslctx!=NULL)
		SSL_CTX_free (sslctx);
#endif	
	delete_sem(tcp_mgr_sem);
	delete_sem(tcplayer_sem);
	delete lock;
#ifdef DEBUG
	printf("~tcplayer end\n");
#endif
}
/*
int32 tcplayer::Lock(int32 timeout) 
{
	
	thread_id callingthread=find_thread(NULL),lockingthread=lock->LockingThread();
	if (lockingthread!=B_ERROR) {
		if (callingthread==lockingthread)
			return B_OK;
	}
	
	if (timeout==-1) {
		if (lock->Lock()){
			
			return B_OK;
		}
		
		return B_ERROR;
	} else {
		return lock->LockWithTimeout(timeout);
		
	}
}
void tcplayer::Unlock() 
{
lock->Unlock();
}
*/
void tcplayer::Start() {
	if (acquire_sem_etc(tcp_mgr_sem,1,B_ABSOLUTE_TIMEOUT,100000)==B_OK) {
		thread=spawn_thread(StartManager,"tcp_layer_manager",B_LOW_PRIORITY,this);
		resume_thread(thread);
	} else {
#ifdef DEBUG
		fprintf(stderr,"tcp_layer_manager already running.\n");
#endif
	}
}
int32 tcplayer::StartManager(void *arg) {
#ifdef DEBUG
	printf("tcp_layer starting\n");
#endif
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
	while(!meTCP->quit) {
//		lock->Lock();
		connections=Connections();
		if (connections>0) {
			current=NextConnection();
			curtime=real_time_clock();
			if (current==NULL) {
				continue;
			}
			timeout=current->last_trans_time+current->time_to_live;
			if (Connected(current)) {
				if ((current->made_connection)  && (!current->notified_connect)) {

#ifdef USEOPENSSL
					if (current->usessl) {
						SSLConnect(current);
					}
#endif					
					current->protocol_ptr->ConnectionEstablished(current);
					
					atomic_add(&current->notified_connect,1);
				}
				if (DataWaiting(current)) {
					if (!current->callbackdone) {
						
						cur=callback_head;
						while (cur!=NULL) {
							if (cur->protocol==current->proto_id) {
//								if (cur->Lock(100000)==B_OK) {
									atomic_add(&current->callbackdone,1);
									cur->callback(current);
//									cur->Unlock();
//								}
								
								break;
							}
							cur=cur->next;
						}
						
					}
					continue;
				}
				//delete above lines			
				if ((curtime>=timeout) && (curtime<(timeout+current->time_to_live))) {
					printf("Manager disconnect 1\n");
					CloseConnection(current);
					continue;
				} else {
					if ((curtime>=timeout) && (curtime>=(timeout+current->time_to_live))) {
					printf("Manager disconnect 2\n");
						KillConnection(current);
						continue;
					}
				}
			} else {
				if ((errno!=EINPROGRESS) && (errno!=EALREADY))
					if ((curtime>=timeout) && (curtime>=(timeout+current->time_to_live))) {
					printf("Manager disconnect 3\n");
						KillConnection(current);
						continue;
					}
			}
		}
//		lock->Unlock();
		if (conn_queue!=NULL) {
#ifdef DEBUG
			printf("Processing queued connections...\n");
#endif
			connection *qcur=conn_queue,*next;
			conn_queue=NULL;
			while (qcur!=NULL) {
				if (meTCP->quit)
					break;
				next=qcur->next;
				ConnectTo(qcur);
				qcur=next;
			}
#ifdef DEBUG
			printf("Done with queued connections.\n");
#endif
		}
		snooze(15000);
	}
	exit_thread(B_OK);
	return 0;
}
uint32 tcplayer::Connections() {
	//BAutolock alock(lock);
//	if (alock.IsLocked()) {
	if (conn_head!=NULL) {
		uint32 count=0;
		connection *current=conn_head;
		while (current!=NULL) {
			count++;
			current=current->next;
		}
		return count;
	}
//	}
	return 0;
}
connection* tcplayer::QueueConnect(ProtocolPlugClass *Proto,int32 protoid,char *host,int16 port, bool ssl, bool forcenew) {
	BAutolock alock(lock);
#ifdef DEBUG
	printf("QueueConnect\n");
#endif
//	while (!alock.IsLocked()) {
//		snooze(10000);
//	}
	connection *conn=NULL;
	if (alock.IsLocked()) {
		conn=new connection;
		conn->protocol_ptr=Proto;
		conn->proto_id=protoid;
		conn->addrstr=host;
		conn->port=port;
		conn->open=false;
		conn->result=-2;
#ifdef USEOPENSSL
		conn->usessl=ssl;
#endif
		if (conn_queue==NULL) {
			conn_queue=conn;
		} else {
			connection *cur=conn_queue;
			while (cur->next!=NULL)
				cur=cur->next;
			cur->next=conn;
		}
	}
#ifdef DEBUG
	printf("QueueConnect done.\n");
#endif
	return conn;
}
connection* tcplayer::ConnectTo(connection *target) {
	BAutolock alock(lock);
#ifdef DEBUG
	printf("Attempting to connect to %s:%d from queue.\n",target->addrstr.String(),target->port);
#endif
	connection *conn=target;
	if (alock.IsLocked()) {
	int32 sockproto=0;
#if USEBONE
	sockproto=IPPROTO_TCP;
#endif
		connection *cur=conn_head;
		if (cur==NULL) {
			conn_head=conn;
			cur=conn;
		} else {
			while (cur->next!=NULL)
				cur=cur->next;
			cur->next=conn;
			cur=cur->next;
		}
		cur=conn_head;
		while (cur!=NULL) {
			if ((strcasecmp(conn->addrstr.String(),cur->addrstr.String())==0) && (cur->port==conn->port))
				break;
			cur=cur->next;
		}
		if ((cur!=NULL) && (cur!=conn)) {
#ifdef DEBUG
			printf("found existing connection:\t%s:%d\t%s:%d\n",cur->addrstr.String(),cur->port,conn->addrstr.String(),conn->port);
#endif
			conn->socket=cur->socket;
			conn->open=cur->open;
//			conn->result=cur->result;
			if (conn->socket==-1) {
				conn->socket=socket(AF_INET,SOCK_STREAM,sockproto);
				int option=1;
				setsockopt(conn->socket,SOL_SOCKET,SO_NONBLOCK,&option,sizeof(option));
				conn->open=false;
				conn->result=-2;
			} else {
/*
				if (!Connected(conn,true)) {
					conn->socket=socket(AF_INET,SOCK_STREAM,sockproto);
					conn->hptr=gethostbyname(conn->addrstr.String());
					conn->open=false;
					goto LetsTryThatAgain;
				}
*/
			}
			conn->hptr=cur->hptr;
		} else {
			conn->socket=socket(AF_INET,SOCK_STREAM,sockproto);
			int option=1;
			setsockopt(conn->socket,SOL_SOCKET,SO_NONBLOCK,&option,sizeof(option));
			conn->hptr=gethostbyname(conn->addrstr.String());
		}
		if (conn->hptr==NULL) {
			//the name can't be found...
#ifdef DEBUG
			printf("\t*** THE HOST NAME CAN'T BE FOUND!!! ***\n");
#endif
			KillConnection(conn);
			return NULL;
		}
		if ((cur!=NULL) && (cur!=conn)) {
			conn->pptr=cur->pptr;
	//		conn->result=cur->result;
			if (conn->open!=true) {
				LetsTryThatAgain:
				sockaddr_in servaddr;
				memcpy(&servaddr.sin_addr,*conn->pptr,sizeof(struct in_addr));
				servaddr.sin_port=htons(conn->port);
#ifdef DEBUG
				printf("[QC] reconnecting. (%s:%ld)\n",conn->addrstr.String(),conn->port);
#endif
				conn->result=connect(conn->socket,(sockaddr *)&servaddr,sizeof(servaddr));
#ifdef DEBUG
				printf("[QC] connect result: %ld\n",conn->result);
#endif
				if (conn->result!=0) {
					int32 err=errno;
#ifdef DEBUG
					printf("error: %ld - %s\n",errno,strerror(errno));
#endif
					if (err==-2147459072) {
						conn->socket=socket(AF_INET,SOCK_STREAM,sockproto);
						int option=1;
						setsockopt(conn->socket,SOL_SOCKET,SO_NONBLOCK,&option,sizeof(option));
						goto LetsTryThatAgain;
					}
				}
			}
		} else {
			if (conn->open!=true) {
			sockaddr_in servaddr;
			conn->pptr=(struct in_addr**)conn->hptr->h_addr_list;
			memcpy(&servaddr.sin_addr,*conn->pptr,sizeof(struct in_addr));
			servaddr.sin_port=htons(conn->port);
			conn->result=connect(conn->socket,(sockaddr *)&servaddr,sizeof(servaddr));
			}
		}
		if (conn->result==0) 
			conn->open=true;
		else 
			conn->open=false;
#ifdef DEBUG
		printf("[QC]Connected? %s\n",conn->open?"yes":"no");
#endif
	if (conn!=NULL) {
//		conn->proto_id=protoid;
//		conn->addrstr=host;
//		conn->port=port;
//#ifdef USEOPENSSL
//		conn->usessl=ssl;
//#endif		
		if (!conn->open) {
			return conn;
		}
		
		if (conn->usessl) {
#ifdef USEOPENSSL
			int flags=fcntl(conn->socket,F_GETFL,0);
			flags|=O_NONBLOCK;
			fcntl(conn->socket,F_SETFL,flags);
#ifdef DEBUG
			printf("Uses SSL...\n");
#endif
			conn->usessl=true;	
#ifdef DEBUG
			printf("Creating new context...");
#endif
			fflush(stdout);
#ifdef DEBUG
			printf("done.\n");
			printf("Creating new ssl object...");
#endif
			fflush(stdout);
			conn->ssl = SSL_new (sslctx);
#ifdef DEBUG
			printf("done.\n");
#endif
			SSL_set_cipher_list(conn->ssl, SSL_TXT_ALL);
			conn->sslbio=BIO_new_socket(conn->socket,BIO_NOCLOSE);
			SSL_set_bio(conn->ssl,conn->sslbio,conn->sslbio);
			SSL_set_connect_state(conn->ssl);
#ifdef DEBUG
			printf("SSL connecting...");
#endif
			fflush(stdout);
			TryConnectAgain2TheSequel:
			conn->result = SSL_connect (conn->ssl);
//			int err = SSL_connect (conn->ssl);
			if (SSL_get_error(conn->ssl,conn->result)==SSL_ERROR_WANT_READ) {
				snooze(10000);
				goto TryConnectAgain2TheSequel;
			}
			flags &= ~O_NONBLOCK;
			fcntl(conn->socket,F_SETFL,flags);
#ifdef DEBUG
			printf("done.\n");
#endif
			if (conn->result==1) {
				SSL_CIPHER *cipher=SSL_get_current_cipher (conn->ssl);
				
#ifdef DEBUG
				printf ("SSL connection using %s\n", SSL_CIPHER_get_name(cipher));
#endif
				int usedbits=0,cipherbits=0;
				usedbits=SSL_CIPHER_get_bits(cipher,&cipherbits);
#ifdef DEBUG
				printf("SSL Cipher is %d bits, %d bits used.\n",cipherbits,usedbits);
				printf("Getting SSL certificate...");
#endif
				fflush(stdout);
				
				conn->server_cert = SSL_get_peer_certificate (conn->ssl);
#ifdef DEBUG
				printf("done.\n");
#endif
				
				const char *s;
				SSL_CIPHER *c;
				
				
				c = SSL_get_current_cipher(conn->ssl);
				
				s = (char *) SSL_get_version(conn->ssl);
#ifdef DEBUG
				printf("SSL version: %s\n",s);
#endif
				s = (char *) SSL_CIPHER_get_name(c);
				
#ifdef DEBUG
				printf("SSL Cipher name: %s\n",s);
#endif
				int ssl_keylength=0;
				SSL_CIPHER_get_bits(c, &ssl_keylength);
				
				/*
				* set to the secret key size for the export ciphers...
				* SSLeay returns the total key size
				*/
				if(strncmp(s, "EXP-", 4) == 0)
					ssl_keylength = 40;
#ifdef DEBUG
				printf("key length: %d\n",ssl_keylength);
#endif
			} else {
				conn->result=SSL_get_error(conn->ssl, conn->result);
#ifdef DEBUG
				printf("SSL Error: %ld\n",conn->result);
#endif
				char errormsg[2000];
				memset(errormsg,0,2000);
				ERR_error_string(conn->result,errormsg);
				
#ifdef DEBUG
				printf("Error Message: %s\n",errormsg);
#endif
				SSL_CIPHER *cipher=SSL_get_current_cipher (conn->ssl);
				
#ifdef DEBUG
				printf ("SSL connection using %s\n", SSL_CIPHER_get_name(cipher));
#endif
				int usedbits=0,cipherbits=0;
				usedbits=SSL_CIPHER_get_bits(cipher,&cipherbits);
#ifdef DEBUG
				printf("SSL Cipher is %d bits, %d bits used.\n",cipherbits,usedbits);
				printf("Getting SSL certificate...");
#endif
				fflush(stdout);
				
				conn->server_cert = SSL_get_peer_certificate (conn->ssl);
#ifdef DEBUG
				printf("done.\n");
#endif
				
				const char *s;
				SSL_CIPHER *c;
				
				
				c = SSL_get_current_cipher(conn->ssl);
				
				s = (char *) SSL_get_version(conn->ssl);
#ifdef DEBUG
				printf("SSL version: %s\n",s);
#endif
				s = (char *) SSL_CIPHER_get_name(c);
				
#ifdef DEBUG
				printf("SSL Cipher name: %s\n",s);
#endif
				int ssl_keylength=0;
				SSL_CIPHER_get_bits(c, &ssl_keylength);
				
				/*
				* set to the secret key size for the export ciphers...
				* SSLeay returns the total key size
				*/
				if(strncmp(s, "EXP-", 4) == 0)
					ssl_keylength = 40;
#ifdef DEBUG
				printf("key length: %d\n",ssl_keylength);
#endif
				
			}
			if (conn->server_cert!=NULL) {
				
				char * str;
#ifdef DEBUG
				printf("certificate: %p\n",conn->server_cert);
				printf("Getting SSL subject...");
#endif
				fflush(stdout);
				str = X509_NAME_oneline (X509_get_subject_name (conn->server_cert),0,0);
#ifdef DEBUG
				printf("done\n");
				printf ("\t subject: %s\n", str);
#endif
				free (str);
				str = X509_NAME_oneline (X509_get_issuer_name  (conn->server_cert),0,0);
#ifdef DEBUG
				printf ("\t issuer: %s\n", str);
#endif
				free (str);
			} else {
#ifdef DEBUG
				printf("Unable to get server SSL certificate.\n");
#endif
				
			}
			
#else
#ifdef DEBUG
			printf("tcplayer.cpp: SSL support has not been included in this binary.\n");
#endif
#endif
		}
	}
}	
	return conn;
}
void tcplayer::SSLConnect(connection *conn)
{
#ifdef USEOPENSSL
#ifdef DEBUG
			printf("Uses SSL...\n");
#endif
			conn->usessl=true;	
#ifdef DEBUG
			printf("Creating new context...");
#endif
			fflush(stdout);
#ifdef DEBUG
			printf("done.\n");
			printf("Creating new ssl object...");
#endif
			fflush(stdout);
			conn->ssl = SSL_new (sslctx);
#ifdef DEBUG
			printf("done.\n");
#endif
			SSL_set_cipher_list(conn->ssl, SSL_TXT_ALL);
			conn->sslbio=BIO_new_socket(conn->socket,BIO_NOCLOSE);
			SSL_set_bio(conn->ssl,conn->sslbio,conn->sslbio);
			SSL_set_connect_state(conn->ssl);
#ifdef DEBUG
			printf("SSL connecting...");
#endif
			fflush(stdout);
			TryConnectAgain2TheSequel:
			conn->result = SSL_connect (conn->ssl);
//			int err = SSL_connect (conn->ssl);
			if (SSL_get_error(conn->ssl,conn->result)==SSL_ERROR_WANT_READ) {
				snooze(10000);
				goto TryConnectAgain2TheSequel;
			}
#ifdef DEBUG
			printf("done.\n");
#endif
			if (conn->result==1) {
				SSL_CIPHER *cipher=SSL_get_current_cipher (conn->ssl);
				
#ifdef DEBUG
				printf ("SSL connection using %s\n", SSL_CIPHER_get_name(cipher));
#endif
				int usedbits=0,cipherbits=0;
				usedbits=SSL_CIPHER_get_bits(cipher,&cipherbits);
#ifdef DEBUG
				printf("SSL Cipher is %d bits, %d bits used.\n",cipherbits,usedbits);
				printf("Getting SSL certificate...");
#endif
				fflush(stdout);
				
				conn->server_cert = SSL_get_peer_certificate (conn->ssl);
#ifdef DEBUG
				printf("done.\n");
#endif
				
				const char *s;
				SSL_CIPHER *c;
				
				
				c = SSL_get_current_cipher(conn->ssl);
				
				s = (char *) SSL_get_version(conn->ssl);
#ifdef DEBUG
				printf("SSL version: %s\n",s);
#endif
				s = (char *) SSL_CIPHER_get_name(c);
				
#ifdef DEBUG
				printf("SSL Cipher name: %s\n",s);
#endif
				int ssl_keylength=0;
				SSL_CIPHER_get_bits(c, &ssl_keylength);
				
				/*
				* set to the secret key size for the export ciphers...
				* SSLeay returns the total key size
				*/
				if(strncmp(s, "EXP-", 4) == 0)
					ssl_keylength = 40;
#ifdef DEBUG
				printf("key length: %d\n",ssl_keylength);
#endif
			} else {
				conn->result=SSL_get_error(conn->ssl, conn->result);
#ifdef DEBUG
				printf("SSL Error: %ld\n",conn->result);
#endif
				char errormsg[2000];
				memset(errormsg,0,2000);
				ERR_error_string(conn->result,errormsg);
				
#ifdef DEBUG
				printf("Error Message: %s\n",errormsg);
#endif
				SSL_CIPHER *cipher=SSL_get_current_cipher (conn->ssl);
				
#ifdef DEBUG
				printf ("SSL connection using %s\n", SSL_CIPHER_get_name(cipher));
#endif
				int usedbits=0,cipherbits=0;
				usedbits=SSL_CIPHER_get_bits(cipher,&cipherbits);
#ifdef DEBUG
				printf("SSL Cipher is %d bits, %d bits used.\n",cipherbits,usedbits);
				printf("Getting SSL certificate...");
#endif
				fflush(stdout);
				
				conn->server_cert = SSL_get_peer_certificate (conn->ssl);
#ifdef DEBUG
				printf("done.\n");
#endif
				
				const char *s;
				SSL_CIPHER *c;
				
				
				c = SSL_get_current_cipher(conn->ssl);
				
				s = (char *) SSL_get_version(conn->ssl);
#ifdef DEBUG
				printf("SSL version: %s\n",s);
#endif
				s = (char *) SSL_CIPHER_get_name(c);
				
#ifdef DEBUG
				printf("SSL Cipher name: %s\n",s);
#endif
				int ssl_keylength=0;
				SSL_CIPHER_get_bits(c, &ssl_keylength);
				
				/*
				* set to the secret key size for the export ciphers...
				* SSLeay returns the total key size
				*/
				if(strncmp(s, "EXP-", 4) == 0)
					ssl_keylength = 40;
#ifdef DEBUG
				printf("key length: %d\n",ssl_keylength);
#endif
				
			}
			if (conn->server_cert!=NULL) {
				
				char * str;
#ifdef DEBUG
				printf("certificate: %p\n",conn->server_cert);
				printf("Getting SSL subject...");
#endif
				fflush(stdout);
				str = X509_NAME_oneline (X509_get_subject_name (conn->server_cert),0,0);
#ifdef DEBUG
				printf("done\n");
				printf ("\t subject: %s\n", str);
#endif
				free (str);
				str = X509_NAME_oneline (X509_get_issuer_name  (conn->server_cert),0,0);
#ifdef DEBUG
				printf ("\t issuer: %s\n", str);
#endif
				free (str);
			} else {
#ifdef DEBUG
				printf("Unable to get server SSL certificate.\n");
#endif
				
			}
			
#endif
}

connection* tcplayer::ConnectTo(ProtocolPlugClass *Proto,int32 protoid,char *host,int16 port, bool ssl, bool forcenew) {
	BAutolock alock(lock);
	if (host==NULL)
		return NULL;
	if (strlen(host)<4)
		return NULL;
#ifdef DEBUG
	printf("Requestor wants to connect to %s:%d\n",host,port);
#endif
	connection *conn=NULL;
	if (alock.IsLocked()) {
	int32 sockproto=0;
#if USEBONE
	sockproto=IPPROTO_TCP;
#endif
	if (forcenew) {
#ifdef DEBUG
		printf("[forcenew]");
#endif
		NewConnectStruct:
#ifdef DEBUG
		printf("\tNew Connection\n");
#endif
		connection *cur=conn_head;
		conn=new connection;
		conn->protocol_ptr=Proto;
		if (cur==NULL) {
			conn_head=conn;
			cur=conn;
		} else {
			while (cur->next!=NULL)
				cur=cur->next;
			cur->next=conn;
			cur=cur->next;
		}
		conn->socket=socket(AF_INET,SOCK_STREAM,sockproto);
		int option=1;
		setsockopt(conn->socket,SOL_SOCKET,SO_NONBLOCK,&option,sizeof(option));
		sockaddr_in servaddr;
		conn->hptr=gethostbyname(host);
		if (conn->hptr==NULL) {
			//the name can't be found...
#ifdef DEBUG
			printf("\t*** THE HOST NAME CAN'T BE FOUND!!! ***\n");
#endif
			KillConnection(conn);
			return NULL;
		}
		
		conn->pptr=(struct in_addr**)conn->hptr->h_addr_list;
		memcpy(&servaddr.sin_addr,*conn->pptr,sizeof(struct in_addr));
		servaddr.sin_port=htons(port);
		conn->result=connect(conn->socket,(sockaddr *)&servaddr,sizeof(servaddr));
		if ((errno==EINPROGRESS) || (conn->result==0)) 
			conn->open=true;
		else  {
			printf("Connection error: %ld - %s\n",errno,strerror(errno));
			conn->open=false;
		}
#ifdef DEBUG
		printf("Connected? %s\n",conn->open?"yes":"no");
#endif
	} else {
		connection *cur=conn_head;
		while (cur!=NULL) {
#ifdef DEBUG
			printf("desired host: %s\ncur->host: %s\n",host,cur->addrstr.String());
#endif
			
			if ((strcasecmp(host,cur->addrstr.String())==0) && (cur->port==port))
				break;
			cur=cur->next;
		}
#ifdef DEBUG
		printf("cur: %p\n",cur);
#endif
		
		if (cur==NULL)
			goto NewConnectStruct;
		if (cur->requests!=0) {
#ifdef DEBUG
			printf("found existing connection, but it's busy.\n");
#endif
			goto NewConnectStruct;
		}
#ifdef DEBUG
		printf("found existing connection, it's available.\n");
		printf("Connected? %s\n",cur->open?"yes":"no");
#endif
		
		if (!Connected(cur,true)) {
#ifdef DEBUG
			printf("Connecting...\n");
#endif
			if (cur->protocol_ptr!=Proto)
				cur->protocol_ptr=Proto;
			cur->socket=socket(AF_INET,SOCK_STREAM,sockproto);
			int option=1;
			setsockopt(cur->socket,SOL_SOCKET,SO_NONBLOCK,&option,sizeof(option));
			sockaddr_in servaddr;
			memcpy(&servaddr.sin_addr,*cur->pptr,sizeof(struct in_addr));
			servaddr.sin_port=htons(port);
			cur->result=connect(cur->socket,(sockaddr *)&servaddr,sizeof(servaddr));
			if (cur->result==0)
				cur->open=true;
			else
				cur->open=false;
#ifdef DEBUG
			printf("Connected? %s\n",cur->open?"yes":"no");
#endif
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
			return conn;
		}
/*		
		if (ssl) {
#ifdef USEOPENSSL
//			int flags=fcntl(conn->socket,F_GETFL,0);
//			flags|=O_NONBLOCK;
//			fcntl(conn->socket,F_SETFL,flags);
#ifdef DEBUG
			printf("Uses SSL...\n");
#endif
			conn->usessl=true;	
#ifdef DEBUG
			printf("Creating new context...");
#endif
			fflush(stdout);
#ifdef DEBUG
			printf("done.\n");
			printf("Creating new ssl object...");
#endif
			fflush(stdout);
			conn->ssl = SSL_new (sslctx);
#ifdef DEBUG
			printf("done.\n");
#endif
			SSL_set_cipher_list(conn->ssl, SSL_TXT_ALL);
			conn->sslbio=BIO_new_socket(conn->socket,BIO_NOCLOSE);
			SSL_set_bio(conn->ssl,conn->sslbio,conn->sslbio);
			SSL_set_connect_state(conn->ssl);
#ifdef DEBUG
			printf("SSL connecting...");
#endif
			fflush(stdout);
			TryConnectAgain2:
			conn->result = SSL_connect (conn->ssl);
//			int err = SSL_connect (conn->ssl);
			if (SSL_get_error(conn->ssl,conn->result)==SSL_ERROR_WANT_READ) {
				snooze(10000);
				goto TryConnectAgain2;
			}
	//		flags &= ~O_NONBLOCK;
	//		fcntl(conn->socket,F_SETFL,flags);
#ifdef DEBUG
			printf("done.\n");
#endif
			if (conn->result==1) {
				SSL_CIPHER *cipher=SSL_get_current_cipher (conn->ssl);
				
#ifdef DEBUG
				printf ("SSL connection using %s\n", SSL_CIPHER_get_name(cipher));
#endif
				int usedbits=0,cipherbits=0;
				usedbits=SSL_CIPHER_get_bits(cipher,&cipherbits);
#ifdef DEBUG
				printf("SSL Cipher is %d bits, %d bits used.\n",cipherbits,usedbits);
				printf("Getting SSL certificate...");
#endif
				fflush(stdout);
				
				conn->server_cert = SSL_get_peer_certificate (conn->ssl);
#ifdef DEBUG
				printf("done.\n");
#endif
				
				const char *s;
				SSL_CIPHER *c;
				
				
				c = SSL_get_current_cipher(conn->ssl);
				
				s = (char *) SSL_get_version(conn->ssl);
#ifdef DEBUG
				printf("SSL version: %s\n",s);
#endif
				s = (char *) SSL_CIPHER_get_name(c);
				
#ifdef DEBUG
				printf("SSL Cipher name: %s\n",s);
#endif
				int ssl_keylength=0;
				SSL_CIPHER_get_bits(c, &ssl_keylength);
				
				
//				 set to the secret key size for the export ciphers...
//				 SSLeay returns the total key size
				
				if(strncmp(s, "EXP-", 4) == 0)
					ssl_keylength = 40;
#ifdef DEBUG
				printf("key length: %d\n",ssl_keylength);
#endif
			} else {
				conn->result=SSL_get_error(conn->ssl, conn->result);
#ifdef DEBUG
				printf("SSL Error: %ld\n",conn->result);
#endif
				char errormsg[2000];
				memset(errormsg,0,2000);
				ERR_error_string(conn->result,errormsg);
				
#ifdef DEBUG
				printf("Error Message: %s\n",errormsg);
#endif
				SSL_CIPHER *cipher=SSL_get_current_cipher (conn->ssl);
				
#ifdef DEBUG
				printf ("SSL connection using %s\n", SSL_CIPHER_get_name(cipher));
#endif
				int usedbits=0,cipherbits=0;
				usedbits=SSL_CIPHER_get_bits(cipher,&cipherbits);
#ifdef DEBUG
				printf("SSL Cipher is %d bits, %d bits used.\n",cipherbits,usedbits);
				printf("Getting SSL certificate...");
#endif
				fflush(stdout);
				
				conn->server_cert = SSL_get_peer_certificate (conn->ssl);
#ifdef DEBUG
				printf("done.\n");
#endif
				
				const char *s;
				SSL_CIPHER *c;
				
				
				c = SSL_get_current_cipher(conn->ssl);
				
				s = (char *) SSL_get_version(conn->ssl);
#ifdef DEBUG
				printf("SSL version: %s\n",s);
#endif
				s = (char *) SSL_CIPHER_get_name(c);
				
#ifdef DEBUG
				printf("SSL Cipher name: %s\n",s);
#endif
				int ssl_keylength=0;
				SSL_CIPHER_get_bits(c, &ssl_keylength);
				
				
//				 set to the secret key size for the export ciphers...
//				 SSLeay returns the total key size
				
				if(strncmp(s, "EXP-", 4) == 0)
					ssl_keylength = 40;
#ifdef DEBUG
				printf("key length: %d\n",ssl_keylength);
#endif
				
			}
			if (conn->server_cert!=NULL) {
				
				char * str;
#ifdef DEBUG
				printf("certificate: %p\n",conn->server_cert);
				printf("Getting SSL subject...");
#endif
				fflush(stdout);
				str = X509_NAME_oneline (X509_get_subject_name (conn->server_cert),0,0);
#ifdef DEBUG
				printf("done\n");
				printf ("\t subject: %s\n", str);
#endif
				free (str);
				str = X509_NAME_oneline (X509_get_issuer_name  (conn->server_cert),0,0);
#ifdef DEBUG
				printf ("\t issuer: %s\n", str);
#endif
				free (str);
			} else {
#ifdef DEBUG
				printf("Unable to get server SSL certificate.\n");
#endif
				
			}
			
#else
#ifdef DEBUG
			printf("tcplayer.cpp: SSL support has not been included in this binary.\n");
#endif
#endif
		}
*/
	}
}	
	return conn;
}

/*
tcplayer::NextConnection cycles through all connection structures that are under the
tcplayer's command; primarily for processing purposes.
*/
connection *tcplayer::NextConnection() {
	connection *current=prev_conn;
	//BAutolock alock(lock);
	//if (alock.IsLocked()) {
	if (current==NULL) {
		current=conn_head;
		prev_conn=current;
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
		}
		return current;		 
//	}
}
void tcplayer::CloseConnection(connection *target) {
//	BAutolock alock(lock);
//	if (alock.IsLocked()) {
#ifdef DEBUG
printf("tcplayer closeconnection\n");
#endif
if (target->open) {
	BAutolock alock(lock);
	if (alock.IsLocked()) {
	closesocket(target->socket);
	target->socket=-1;
	target->open=false;
	}
	
}
if (target->socket!=-1)
	target->socket=-1;
	
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
#ifdef DEBUG
printf("about to call connection closed callback %ld\n",target->closedcbdone);
#endif
				cur->connclosedcb(target);
			}
			break;
		}
		cur=cur->next;
	}
//	}
}

void tcplayer::KillConnection(connection *target) {
	//BAutolock alock(lock);
	//if (alock.IsLocked()) {
	if (target==NULL)
		return;
	
#ifdef DEBUG
	printf("Killing connection %p\n",target);
#endif
	
		if (target->open) {
		closesocket(target->socket);
		target->socket=-1;
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
	//}
}
int32 tcplayer::Send(connection **conn,unsigned char *data, int32 size) {
	BAutolock alock(lock);
	ssize_t sent=0;
	if (alock.IsLocked()) {

	if ((*conn)->requests>0) {
			char host[250];
			int16 port;
#ifdef USEOPENSSL
			*conn=ConnectTo((*conn)->protocol_ptr,(*conn)->proto_id,(char*)(*conn)->addrstr.String(),(*conn)->port,(*conn)->usessl,true);
#else
			*conn=ConnectTo((*conn)->protocol_ptr,(*conn)->proto_id,(char*)(*conn)->addrstr.String(),(*conn)->port,false,true);
#endif
			atomic_add(&(*conn)->requests,1);
		
	}
	else

	 atomic_add(&(*conn)->requests,1);
	
	if (!Connected((*conn))) {
#ifdef DEBUG
		printf("Not connected (send)\n");
#endif
		
			char host[250];
			int16 port;
#ifdef USEOPENSSL
			*conn=ConnectTo((*conn)->protocol_ptr,(*conn)->proto_id,(char*)(*conn)->addrstr.String(),port,(*conn)->usessl);
#else
			*conn=ConnectTo((*conn)->protocol_ptr,(*conn)->proto_id,(char*)(*conn)->addrstr.String(),port,false);
#endif
	}
#ifdef USEOPENSSL
	if ((*conn)->usessl) {
		try {
			
		sent=SSL_write((*conn)->ssl,(const char*)data,size);
		}
		catch(...) {
#ifdef DEBUG
			printf("hmmm... it seems that the send (SSL_WRITE) triggered an exception; perhaps the connection closed before we could send...\n");
#endif
		
		}
		
	}
	else {
#endif
		try {
			
		sent=send((*conn)->socket,data,size,0);
		}
		catch(...) {
#ifdef DEBUG
			printf("hmmm... it seems that the send triggered an exception; perhaps the connection closed before we could send...\n");
#endif
			
		}
		
#ifdef USEOPENSSL
	}
#endif	 
	if (sent<0) {
		(*conn)->result=errno;
	}
	else {
		(*conn)->last_trans_time=real_time_clock();
	}
#ifdef USENETSERVER
	if (sent<=0) {
		(*conn)->open=false;
			char host[250];
			int16 port=(*conn)->port;
#ifdef USEOPENSSL
			*conn=ConnectTo((*conn)->protocol_ptr,(*conn)->proto_id,(char*)(*conn)->addrstr.String(),port,(*conn)->usessl,true);
#else
			*conn=ConnectTo((*conn)->protocol_ptr,(*conn)->proto_id,(char*)(*conn)->addrstr.String(),port,false,true);
#endif
			atomic_add(&(*conn)->requests,1);
#ifdef USEOPENSSL
	if ((*conn)->usessl) {
		sent=SSL_write((*conn)->ssl,(const char*)data,size);
	}
	else {
#endif
		sent=send((*conn)->socket,data,size,0);
#ifdef USEOPENSSL
	}
#endif	 
	}
#endif
	}
	return sent;
}
int32 tcplayer::Receive(connection **conn, unsigned char *data, int32 size) {
	BAutolock alock(lock);
	ssize_t got=0;
	int option=1;
	if (alock.IsLocked()) {
		setsockopt((*conn)->socket,SOL_SOCKET,SO_NONBLOCK,&option,sizeof(option));
		if (Connected((*conn))) {
#ifdef USEOPENSSL
			if ((*conn)->usessl) {
				got=SSL_read ((*conn)->ssl,(char*)data,size);
			} else {
#endif
				got=recv((*conn)->socket,data,size,0);
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
				got=SSL_read((*conn)->ssl,(char*)data,size);
			} else {
#endif
				got=recv((*conn)->socket,data,size,0);
#ifdef USEOPENSSL
			}
#endif
			if (got<0) {
				(*conn)->result=errno;
			} else {
				if (got>0)
					(*conn)->last_trans_time=real_time_clock();
			}
		}
		
		if ((*conn)->callbackdone) {
			atomic_add(&(*conn)->callbackdone,-1);
		}
		option=0;
		setsockopt((*conn)->socket,SOL_SOCKET,SO_NONBLOCK,&option,sizeof(option));
#ifdef USENETSERVER
/*
		if (errno==EWOULDBLOCK) {
			printf("receive would block...\n");
		}
*/		
		if ((got<0) && ((errno==ENOTCONN) || ( errno==ECONNRESET) || (errno==EINTR) /*|| (errno==EWOULDBLOCK) */)) {
			printf("receive-close\n");
			CloseConnection((*conn));
		}
#endif
	}
	return got;
}

void tcplayer::RequestDone(connection *conn,bool close) {
//	BAutolock alock(lock);
//	if (alock.IsLocked()) {
		if (conn!=NULL) {
			atomic_add(&conn->requests,-1);
			if (conn->closedcbdone==0) {
				
				if (conn->requests>0)
					conn->requests=0;
				if ((close) && (!conn->closedcbdone)) {
					atomic_add(&conn->closedcbdone,1);
					printf("request done close\n");
					CloseConnection(conn);
				}
				
			}
			
		}
//	}
}

bool tcplayer::DataWaiting(connection *conn) {
	BAutolock alock(lock);
	if (alock.IsLocked()) {
	if (IsValid(conn)) {
//	printf("Data Waiting check\n");
//	bool open=false;
		
//	if ((open=Connected(conn))) {
//	if ((open) && (conn->open==0))
//		printf("[DataWaiting] Connection status reports differ\n");
	
	//mtx->lock();
	try {
	struct timeval tv;
	tv.tv_sec=2;
	tv.tv_usec=0;
	struct fd_set fds;
	FD_ZERO(&fds);
	FD_SET(conn->socket,&fds);
	select(conn->socket+1,&fds,NULL,NULL,&tv);
	bool answer=false;
	answer=FD_ISSET(conn->socket,&fds);
 #ifdef BONE_VERSION
	if (answer) {
      char szBuffer[5] = ""; 
		int iret = recv( conn->socket, szBuffer, 4, MSG_PEEK ); 
		if (iret>0)
			answer=true;
		else
			answer=false;
	}
#endif
	//mtx->unlock();
	return answer;
	}
	catch(...)
	{
		return false;
	}
		
	}
	else {
		return false;
	}
	}
}
bool tcplayer::IsValid(connection *conn) {
	bool truth=false;
	BAutolock alock(lock);
//	if (acquire_sem(conn_sem)!=B_OK)
//		return false;
	if (alock.IsLocked()) {
			
		connection *cur=conn_head;
		while (cur!=NULL) {
			if (cur==conn) {
				truth=true;
				break;
			}
			cur=cur->next;
		}
		try {
		if (cur->socket==-1)
			truth=false;
		}
		catch(...) {
			truth=false;
		}
//	release_sem(conn_sem);
	}
	
	return truth;
}
bool tcplayer::Connected(connection *conn,bool skipvalid) {
//	printf("Connection status check\n");
	BAutolock alock(lock);
	if (alock.IsLocked()) {
		try {
			
			if (conn!=NULL) {
				if (skipvalid) {//no need to check validation
					CheckConnectStatus://connection status checking starts here.
					if (conn->socket<0)
						return false;
					bool oldstatus=conn->open;
					int iret=0;
					bool bOK=true;
					struct timeval timeout;
					timeout.tv_sec=1;
					timeout.tv_usec=0;
					fd_set readsocketset, exceptsocketset,writesocketset;
					FD_ZERO(&readsocketset);
					FD_ZERO(&exceptsocketset);
					FD_ZERO(&writesocketset);
					FD_SET(conn->socket,&readsocketset);
					FD_SET(conn->socket,&exceptsocketset);
					FD_SET(conn->socket,&writesocketset);
					iret=select(conn->socket+1,&readsocketset,&writesocketset,&exceptsocketset,&timeout);
					bOK=(iret>0);
					conn->open=FD_ISSET(conn->socket,&readsocketset)|FD_ISSET(conn->socket,&writesocketset);
//					printf("[Connected?] Connection open? %s\n",conn->open?"yes":"no");
					if (bOK){
						if (FD_ISSET(conn->socket,&exceptsocketset)) {
							//we've disconnected
		//					Disconnect();
							conn->open=false;
							printf("Socket exception: %d - %s\n",errno,strerror(errno));
						}
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
								printf("connected close\n");
								CloseConnection(conn);
							}
						}
						else
							conn->open=true; 
					}
#endif
#ifdef DEBUG
					if (oldstatus)
						if (!conn->open)
							printf("Connection %p on socket %ld closed\n",conn,conn->socket);
#endif
				} else {
					if ( IsValid(conn))
						goto CheckConnectStatus; //validation successful
					else
						return false; //validation check failed... don't check
				}
				//return the actual status
				if (conn->open) {
					
					conn->made_connection=1;
				}
				else
					conn->made_connection=0;
				return conn->open;
			}
		}
		catch(...) {
			return false;
			
		}
	
	//conn is NULL, so this is obviously invalid
	}
	return false;
}
status_t tcplayer::Quit() {
#ifdef DEBUG
	printf("tcp_layer stopping\n");
#endif
	status_t status=B_OK;
	BAutolock alock(lock);
	if (alock.IsLocked()) {
		
	atomic_add(&quit,1);
#ifdef DEBUG
	printf("tcplayer: releasing sem for shutdown...\n");
#endif
	
	release_sem(tcp_mgr_sem);
#ifdef DEBUG
	printf("tcplayer: waiting for thread...\n");
#endif
	
	wait_for_thread(thread,&status);
	}
	
#ifdef DEBUG
	printf("tcplayer: returning...\n");
#endif
	
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
#ifdef DEBUG
	printf("SetDRCallback: %p\n",callback_head);
#endif
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

}
