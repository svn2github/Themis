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
#include "connection.h"
#include <string.h>
#include <Autolock.h>
#include <stdio.h>
#include <OS.h>
#include <errno.h>
#include "tcpmanager.h"
using namespace _Themis_Networking_;

BList *Connection::ConnectionList=NULL;
BLocker *Connection::ListLock=NULL;
TCPManager *Connection::TCPMan=NULL;
int32 Connection::SystemReady=0;

void Connection::AddToList(Connection *connection) {
	if (SystemReady) {
		
		BAutolock alock(ListLock);
		if (alock.IsLocked()) {
			if (ConnectionList==NULL) 	
				ConnectionList=new BList(1);
			ConnectionList->AddItem(connection);
		}
	}
	
}
void Connection::RemoveFromList(Connection *connection) {
	if (SystemReady) {
		
		BAutolock alock(ListLock);
		if (alock.IsLocked()) {
			ConnectionList->RemoveItem(connection);
			if (ConnectionList->IsEmpty()) {
				delete ConnectionList;
				ConnectionList=NULL;
			}
		}
	}
	
}
bool Connection::HasConnection(Connection *connection) {
	bool found=false;
	BAutolock alock(ListLock);
	if (alock.IsLocked()) 
		if (ConnectionList!=NULL)
			found=ConnectionList->HasItem(connection);
	return found;
}
Connection *Connection::ConnectionAt(int32 which) {
	Connection *connection=NULL;
	BAutolock alock(ListLock);
	if (alock.IsLocked())
		if (ConnectionList!=NULL)
			connection=(Connection*)ConnectionList->ItemAt(which);
	return connection;
}
int32 Connection::CountConnections() {
	int32 count=0;
	BAutolock alock(ListLock);
	if (alock.IsLocked())
		if (ConnectionList!=NULL)
			count=ConnectionList->CountItems();
	return count;
	
}
int32 Connection::ConnectionResult() {
	printf("ConnectionResult\n");
	int32 result=0;
	BAutolock alock(lock);
	if (alock.IsLocked())
		result=connection_result;
	return result;
}
bool Connection::NotifiedConnect() {
	bool notified=false;
	BAutolock alock(lock);
	if (alock.IsLocked())
		notified=notified_connect;
	return notified;
}
void Connection::NotifyConnect(){
	BAutolock alock(lock);
	if (alock.IsLocked()) {
		if (owner!=NULL) {
			owner->ConnectionEstablished(this);
			if (notified_connect==0)
				atomic_add(&notified_connect,1);
		}
		
	}
	
}
bool Connection::NotifiedDisconnect(){
	bool notified=false;
	BAutolock alock(lock);
	if (alock.IsLocked())
		notified=notified_disconnect;
	return notified;
}
void Connection::NotifyDisconnect() {
	BAutolock alock(lock);
	if (alock.IsLocked()) {
		if (owner!=NULL) {
			owner->ConnectionTerminated(this);
			if (notified_disconnect==0)
				atomic_add(&notified_disconnect,1);
		}
	}
	
}

Connection::Connection(NetworkableObject *NetObject,const char *host, uint16 port,bool secure,bool async) {
	Init();
//	lock=new BLocker(true);
	owner=NetObject;
	if (host!=NULL) {
		
		int32 len=strlen(host);
		host_name=new char[len+1];
		memset((char*)host_name,0,len+1);
		strcpy((char*)host_name,host);
	} else
		host_name=NULL;
	
	host_port=port;
	use_ssl=secure;
	
	nonblocking=async;
}
Connection::~Connection() {
	if (!IsBufferEmpty())
		EmptyBuffer();
	session_id=0;
	in_use=0;
	lastusedtime=0;
	host_port=0;
	if (host_name!=NULL) {
		memset((char*)host_name,0,strlen(host_name));
		delete host_name;
		host_name=NULL;
	}
#ifdef USEOPENSSL
	if (server_cert!=NULL) {
		X509_free(server_cert);
		server_cert=NULL;
	}
	if (ssl!=NULL) {
		SSL_free(ssl);
		ssl=NULL;
	}
	
	if (ssl_version!=NULL) {
		memset((char*)ssl_version,0,strlen(ssl_version)+1);
		free((char*)ssl_version);
		ssl_version=NULL;
	}
	if (cipher_name!=NULL) {
		memset((char*)cipher_name,0,strlen(cipher_name)+1);
		free((char*)cipher_name);
		cipher_name=NULL;
	}
	if (server_cert_subject!=NULL) {
		memset((char*)server_cert_subject,0,strlen(server_cert_subject)+1);
		free((char*)server_cert_subject);
		server_cert_subject=NULL;
	}
	if (server_cert_issuer!=NULL) {
		memset((char*)server_cert_issuer,0,strlen(server_cert_issuer)+1);
		free((char*)server_cert_issuer);
		server_cert_issuer=NULL;
	}
	
#endif	
//	delete lock;
//	RemoveFromList(this);
	if (owner!=NULL) {
		owner->DestroyingConnectionObject(this);
		owner=NULL;
	}
	
}
void Connection::OwnerRelease() {
	BAutolock alock(lock);
	if (alock.IsLocked()) {
		printf("Connection::OwnerRelease\n");
		owner=NULL;
		NewSession();
	}
}
void Connection::AssignNetObject(NetworkableObject *net_obj) 
{
	BAutolock alock(lock);
	if (alock.IsLocked())
		owner=net_obj;
	
}
void Connection::Connect() {
	BAutolock alock(lock);
	if (alock.IsLocked()) {
		if (IsConnected()) {
			if (owner!=NULL)
				owner->ConnectionAlreadyExists(this);
			if (notified_connect==0)
				atomic_add(&notified_connect,1);
		} else {
			//go through the process to connect...
			connection_result=0;
			socket_id=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
			if (nonblocking) {
				int option=1;
				setsockopt(socket_id,SOL_SOCKET,SO_NONBLOCK,&option,sizeof(option));
			}
			
			struct hostent *hptr=gethostbyname(host_name);
			if (hptr!=NULL) {
				memcpy(&sa.sin_addr,(in_addr *)hptr->h_addr_list[0],sizeof(struct in_addr));
				sa.sin_family=AF_INET;
				sa.sin_port=htons(host_port);
				connection_result=connect(socket_id,(sockaddr*)&sa,sizeof(sa));
				if ((errno!=EINPROGRESS) && (errno!=0)) {
					printf("connection error: %d:%s\n",errno,strerror(errno));
					connection_result=-1;
				}
			} else {
				//signal error...
				connection_result=h_errno;
			}
			
			
		}
		
		
	}
	
}
void Connection::ConnectionEstablished(){
	BAutolock alock(lock);
	if (alock.IsLocked()) {
		if (use_ssl) {
#ifdef USEOPENSSL
			ssl=SSL_new(TCPMan->SSLContext());
			SSL_set_cipher_list(ssl,SSL_TXT_ALL);
			bio=BIO_new_socket(socket_id,BIO_NOCLOSE);
			SSL_set_bio(ssl,bio,bio);
			SSL_set_connect_state(ssl);
			int32 error=0;
			bool canexitloop=false;
			
			do {
				connection_result=SSL_connect(ssl);
				error=SSL_get_error(ssl,connection_result);
				switch(error) {
					case SSL_ERROR_NONE:
						canexitloop=true;
						break;
					case SSL_ERROR_SSL:
						canexitloop=true;
						//something ugly must have happened.
						break;
					case SSL_ERROR_WANT_CONNECT:
					case SSL_ERROR_WANT_WRITE:
					case SSL_ERROR_WANT_READ:
					default:
						snooze(10000);
						break;
				}
			}while (canexitloop==false);
			if (connection_result==1) {
				cipher=SSL_get_current_cipher(ssl);
				cipher_used_bits=SSL_CIPHER_get_bits(cipher,(int*)&cipher_bits);
				server_cert=SSL_get_peer_certificate(ssl);
				ssl_version=SSL_get_version(ssl);
				cipher_name=SSL_CIPHER_get_name(cipher);
				server_cert_subject=X509_NAME_oneline(X509_get_subject_name(server_cert),0,0);
				server_cert_issuer=X509_NAME_oneline(X509_get_issuer_name(server_cert),0,0);
				printf("================\nSSL Information:\n================\n");
				printf("SSL Version:\t\t%s\n",ssl_version);
				printf("Certificate Subject:\t\trans_st%s\nIssuer:\t\t%s\n",server_cert_subject,server_cert_issuer);
				printf("Cipher:\t\t%s\nBits:\t\t%ld\nBits Used:\t\t%ld\n",cipher_name,cipher_bits,cipher_used_bits);
				printf("================\n");
			} else {
				use_ssl=false;
			}
			
#else
			printf("SSL is not supported, so I'm trying to do ordinary communications...\n");
#endif
		}
		if (notified_connect==0) {
			first_data_received_time=real_time_clock_usecs();
			if (owner!=NULL)
				owner->ConnectionEstablished(this);
			atomic_add(&notified_connect,1);
			printf("connection %p: notified connect %ld\n",this,notified_connect);
		}
			
	}
}
void Connection::ConnectionTerminated() {
	//call NetworkableObject's ConnectionTerminated...
	Disconnect();
	
}


void Connection::Disconnect() {
	BAutolock alock(lock);
	if (alock.IsLocked()) {
		connection_result=0;
		if (socket_id>=0) {
			lastusedtime=real_time_clock();
#ifdef USEOPENSSL
			if (ssl!=NULL)
				SSL_shutdown(ssl);
#endif
			closesocket(socket_id);
			socket_id=-1;
		}
	}
}

bool Connection::IsConnected() {
	bool is_connected=false;
	BAutolock alock(lock);
	if (alock.IsLocked()) {
		if (socket_id>=0) {
			fd_set rset,wset,eset;
			FD_ZERO(&rset);
			FD_SET(socket_id,&rset);
			eset=wset=rset;
			struct timeval tv;
			tv.tv_sec=0;
			tv.tv_usec=100000;
			connection_result=select(socket_id+1,&rset,&wset,&eset,&tv);
			
			if (connection_result==0) {
				//Error!! Call NetworkableObject's error function.
				switch(errno) {
					case EWOULDBLOCK:
						printf("Connection::IsConnected Would Block\n");
						last_error=ERROR_WOULD_BLOCK;
						break;
					case EINTR:
						last_error=ERROR_SOCKET_INTERRUPTED;
						break;
					case ECONNRESET:
						printf("Connection::IsConnected Connection Reset\n");
						last_error=ERROR_CONNECTION_RESET;
						break;
					case ENOTCONN:
						last_error=ERROR_NOT_CONNECTED;
						break;
					case EBADF:
						last_error=ERROR_INVALID_SOCKET;
						break;
					default: {
						last_error=ERROR_UNKNOWN;
					}
//					if (owner!=NULL)
//						owner->ConnectionError(this);
				}
				return false;
			}
			if (FD_ISSET(socket_id,&eset)) {
				//An exception occured...
				printf("Exception on connection to \"%s\":\t%ld - %s\n",host_name,errno,strerror(errno));
			}
			if ((FD_ISSET(socket_id,&rset)) || (FD_ISSET(socket_id,&wset)) ) {
#ifdef BONE
				int len=sizeof(connection_result);
				if (getsockopt(socket_id,SOL_SOCKET,SO_ERROR,&connection_result,&len)<0) {
					connection_result=errno;
					//another error has occurred
					switch(errno) {
						case EWOULDBLOCK:
							last_error=ERROR_WOULD_BLOCK;
							break;
						case EINTR:
							last_error=ERROR_SOCKET_INTERRUPTED;
							break;
						case ECONNRESET:
							last_error=ERROR_CONNECTION_RESET;
							break;
						case ENOTCONN:
							last_error=ERROR_NOT_CONNECTED;
							break;
						case EBADF:
							last_error=ERROR_INVALID_SOCKET;
							break;
						default: {
							last_error=ERROR_UNKNOWN;
						}
					}
					return false;
//					if (owner!=NULL)
//						owner->ConnectionError(this);
				}
				
#else
				connection_result=errno;
#endif
				is_connected=true;
			}
			 
		}
		
	}
	return is_connected;
}
bool Connection::IsDataWaiting() {
	bool waiting_data=false;
	BAutolock alock(lock);
	if (alock.IsLocked()) {
		if (socket_id>=0) {
			struct timeval tv;
			tv.tv_sec=2;
			tv.tv_usec=0;
			struct fd_set fds;
			FD_ZERO(&fds);
			FD_SET(socket_id,&fds);
			int32 err=select(socket_id+1,&fds,NULL,NULL,&tv);
			if (err<=0)
				return false;
			waiting_data=FD_ISSET(socket_id,&fds);
		}
		
	}
	return waiting_data;
}
bool Connection::IsSecure() {
	bool secured=false;
	BAutolock alock(lock);
	if (alock.IsLocked()) {
#ifdef USEOPENSSL
		if ((use_ssl) && (ssl!=NULL))
			secured=true;
		else
			secured=false;
#else
		secured=false;
#endif
	}
	return secured;
}

void Connection::Init() {
bytes_read_by_owner=0L;
	
	session_bytes_sent=0L;
	session_bytes_received=0L;
	total_bytes_sent=0L;
	total_bytes_received=0L;
	session_bytes_out=0L;
	socket_id=-1;
	memset(&sa,0,sizeof(sa));
	buffer_in=NULL;
	session_id=0;
	owner=NULL;
	in_use=0;
	lastusedtime=0;
	host_name=NULL;
	host_port=0;
	connection_result=0;
	nonblocking=true;
	last_error=0;
	notified_connect=0;
	notified_disconnect=0;
	
#ifdef USEOPENSSL
	ssl=NULL;
	cipher=NULL;
	cipher_used_bits=0;
	cipher_bits=0;
	ssl_version=NULL;
	cipher_name=NULL;
	server_cert=NULL;
	server_cert_subject=NULL;
	server_cert_issuer=NULL;
#endif	
	AddToList(this);
}
bool Connection::IsBufferEmpty() {
	bool emptied=false;
	BAutolock alock(lock);
	if (alock.IsLocked())
		if (buffer_in==NULL)
			emptied=true;
	return emptied;
}

int32 Connection::SessionID() {
	int32 id=0;
	BAutolock alock(lock);
	if (alock.IsLocked())
		id=session_id;
	return id;
}
int32 Connection::NewSession() {
	printf("[NewSession]\n");
	fflush(stdout);
	int32 id=0;
	BAutolock alock(lock);
	if (alock.IsLocked()) {
		if (IsInUse())
			StopUsing();
		atomic_add(&session_id,1);	
		owner=NULL;
		id=session_id;
		lastusedtime=real_time_clock();
		first_data_received_time=0;
		session_bytes_sent=0L;
		session_bytes_received=0L;
		session_bytes_out=0L;
		last_error=NO_ERROR;
		bytes_per_second=0;
		notified_connect=0;
		notified_disconnect=0;
		if (!IsBufferEmpty())
			EmptyBuffer();
	}
	return id;
}
double Connection::BytesPerSecond(){
	double val=0.0;
	BAutolock alock(lock);
	if (alock.IsLocked())
		val=bytes_per_second;
	return val;
}

status_t Connection::AppendData(void *data, off_t size) {
	printf("[AppendData]\n");
	fflush(stdout);
	status_t status=B_BUSY;
	BAutolock alock(lock);
	if (alock.IsLocked()) {
/*
		buffer_list_st *nubuff=new buffer_list_st;
		if (nubuff==NULL) {
			last_error=ERROR_NO_MEMORY;
			if (owner!=NULL)
				owner->ConnectionError(this);
			return B_NO_MEMORY;
		}
		
		nubuff->buffer=new Buffer(data,size);
		if (nubuff->buffer==NULL) {
			delete nubuff;
			last_error=ERROR_NO_MEMORY;
			if (owner!=NULL)
				owner->ConnectionError(this);
			
			return B_NO_MEMORY;
		}
		nubuff->next=NULL;
		if (buffer_in==NULL) {
			buffer_in=nubuff;
		} else {
			buffer_list_st *next=buffer_in;
//			if (next!=NULL)
				while (next->next!=NULL)
					next=next->next;
			next=nubuff;
		}
*/
		buffer_list_st *current=buffer_in;
		if (current==NULL) {
			buffer_in=new buffer_list_st;
			current=buffer_in;
		} else {
			while (current->next!=NULL)
				current=current->next;
			current->next=new buffer_list_st;
			current=current->next;
		}
		current->next=NULL;
		current->buffer=new Buffer(data,size);
		status=B_OK;
		lastusedtime=real_time_clock();
		printf("[Connection::AppendData] Buffer is now %Ld bytes long\n",DataSize());
	}
	return status;
}
off_t Connection::Receive(void *data, off_t max_size) {
	printf("[Receive]\n");
	fflush(stdout);
	off_t size=0L;
	BAutolock alock(lock);
	if (alock.IsLocked()) {
		buffer_list_st *current=buffer_in;
		buffer_list_st *prev;
		memset((unsigned char*)data,0,max_size);
		int32 bytes;
		while (current!=NULL) {
			if (current->buffer==NULL)
				break;
			if ((size+current->buffer->Size())>max_size) {
				break;
			}
			bytes=0;
			printf("[Receive] at most %ld can be read now.\n", max_size-size);
			bytes=current->buffer->GetData((unsigned char*)data+size,max_size-size);
			size+=bytes;
			printf("[Receive] %ld (%ld) bytes were copied into protocol buffer.\n",bytes,size);
			prev=current;
			current=current->next;
			delete prev->buffer;
			delete prev;
			prev=NULL;
		}
		buffer_in=current;
		lastusedtime=real_time_clock();
		session_bytes_out+=size;
		printf("[Connection::Receive] %ld bytes have been transferred to owner. %ld bytes left in buffer.\n\n",session_bytes_received,DataSize());
		fflush(stdout);
	}
	return size;
}
off_t Connection::DataSize() {
	off_t size=0L;
	BAutolock alock(lock);
	if (alock.IsLocked()) {
		buffer_list_st *current=buffer_in;
		while(current!=NULL) {
			if (current->buffer!=NULL)
				size+=current->buffer->Size();
			current=current->next;
		}	
	}
	return size;	
}
void Connection::EmptyBuffer() {
	BAutolock alock(lock);
	if (alock.IsLocked()) {
		if (buffer_in!=NULL) {
			buffer_list_st *next;
			while (buffer_in!=NULL) {
				next=buffer_in->next;
				delete buffer_in->buffer;
				delete buffer_in;
				buffer_in=next;
			}
			
		}
		lastusedtime=real_time_clock();
	}
}
off_t Connection::BytesSent(bool session) {
	off_t bytes=0L;
	BAutolock alock(lock);
	if (alock.IsLocked())
		if (session)
			bytes=session_bytes_sent;
		else
			bytes=total_bytes_sent;
	
	return bytes;
}
off_t Connection::BytesReceived(bool session) {
	off_t bytes=0L;
	BAutolock alock(lock);
	if (alock.IsLocked())
		if (session)
			bytes=session_bytes_received;
		else
			bytes=total_bytes_received;
	
	return bytes;
}
off_t Connection::Send(void *data, off_t size) {
	off_t bytes=0L;
	BAutolock alock(lock);
	if (alock.IsLocked()){
		if (socket_id>=0) {
			if (use_ssl) {
				try {
#ifdef USEOPENSSL
			
					bytes=SSL_write(ssl,(const char*)data,size);
#else
					bytes=send(socket_id,(char *)data, size,0);
#endif
				}
				catch(...) {
				}
		
			} else				
				bytes=send(socket_id,(char *)data, size,0);
		}
		session_bytes_sent+=bytes;
		total_bytes_sent+=bytes;
		TCPMan->AddBytesSent(bytes);
		lastusedtime=real_time_clock();
	}
	return bytes;
}
int32 Connection::IsInUse() {
	int32 isinuse=0;
	BAutolock alock(lock);
	if (alock.IsLocked())
		isinuse=in_use;
	return isinuse;
}
void Connection::StartUsing() 
{
	BAutolock alock(lock);
	if (alock.IsLocked())
		atomic_add(&in_use,1);
}
void Connection::StopUsing()
{
	BAutolock alock(lock);
	if (alock.IsLocked())
		atomic_add(&in_use,-1);
}

bool Connection::Matches(const char *host, uint16 port) {
	bool matches=false;
	BAutolock alock(lock);
	if (alock.IsLocked())
		if ((strcasecmp(host, host_name)==0) && (port==host_port))
			matches=true;
	return matches;
}
void Connection::RetrieveData() {
//	printf("[RetrieveData]\n");
	fflush(stdout);
	BAutolock alock(lock);
	if (alock.IsLocked()) {
		int16 size=10240;
		int16 bytes=0;
		unsigned char *data=new unsigned char[size];
		memset(data,0,size);
		if (socket_id>=0) {
			
			if (use_ssl) {
				try {
#ifdef USEOPENSSL
			
					bytes=SSL_read(ssl,(char*)data,size);
#else
					bytes=recv(socket_id,(char *)data, size,0);
#endif
				}
				catch(...) {
				}
		
			} else				
				bytes=recv(socket_id,(char *)data, size,0);
		}
		if (bytes>0) { //negative byte counts are errors... zero ones are useless
			printf("[RetrieveData] just retrieved %ld bytes\n",bytes);
			fflush(stdout);
			//call the networkable object's data waiting function.
			
			bigtime_t currenttime=real_time_clock_usecs();
			float tdiff=(currenttime-first_data_received_time)/1000000.0;
			if (tdiff<=(1.0/1000000.0))
				tdiff=1.0;
			session_bytes_received+=size;
			bytes_per_second=(session_bytes_received*1.0)/tdiff;
			printf("Estimated Bytes Per Second: %1.4f\n",bytes_per_second);
			total_bytes_received+=size;
			TCPMan->AddBytesReceived(size);
			AppendData(data,bytes);
			lastusedtime=real_time_clock();
			if (owner!=NULL)
				owner->DataIsWaiting(this);
		} else {
			if (bytes<0) {
				//notify of errors...
				bool notify=true;
				switch(errno) {
					case EWOULDBLOCK:
						last_error=ERROR_WOULD_BLOCK;
						notify=false;
						break;
					case EINTR:
						last_error=ERROR_SOCKET_INTERRUPTED;
						break;
					case ECONNRESET:
						last_error=ERROR_CONNECTION_RESET;
						break;
					case ENOTCONN:
						last_error=ERROR_NOT_CONNECTED;
						break;
					case EBADF:
						last_error=ERROR_INVALID_SOCKET;
						break;
					default: {
						last_error=ERROR_UNKNOWN;
					}
				}
				if (notify)
					if (owner!=NULL);
						//owner->ConnectionError(this);
			}
		}
		
		
		memset(data,0,size);
		delete data;
		data=NULL;
		
	}
}
int32 Connection::LastUsed(){
	int32 lasttime=0;
	BAutolock alock(lock);
	if (alock.IsLocked())
		lasttime=lastusedtime;
	return lasttime;
}
void Connection::TimeOut() {
	BAutolock alock(lock);
	if (alock.IsLocked()) {
		last_error=ERROR_TIMED_OUT;
		if (owner!=NULL)
			owner->ConnectionError(this);
	}
	
}

int32 Connection::Error() 
{
	int32 errnum=BASE_ERROR;
	BAutolock alock(lock);
	if (alock.IsLocked())
		errnum=last_error;
	return errnum;
}
void Connection::ClearError() 
{
	BAutolock alock(lock);
	if (alock.IsLocked())
		last_error=NO_ERROR;
}
const char *Connection::ErrorString(int32 which)
{
	const char *string=NULL;
	switch(which) {
		case NO_ERROR:
			string="No error.";
			break;
		case ERROR_SERVER_ADDRESS_NOT_FOUND:
			string="The address of the server could not be found.";
			break;
		case ERROR_CONNECTION_REFUSED:
			string="The server has refused the connection attempt.";
			break;
		case ERROR_TIMED_OUT:
			string="The connection has timed out and been terminated.";
			break;
		case ERROR_CONNECTION_RESET:
			string="The connection was reset by the remote server.";
			break;
		case ERROR_NOT_CONNECTED:
			string="This object is not connected to the remote server.";
			break;
		case ERROR_INVALID_SOCKET:
			string="There has been an attempt to use an invalid socket on the client side.";
			break;
		case ERROR_NO_MEMORY:
			string="Out of memory.";
		
		default:
			//string="Unknown error code.";
			string=strerror(errno);
	}
	return string;
}
