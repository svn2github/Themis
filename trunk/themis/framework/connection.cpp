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

Original Author & Project Manager: Raymond "Z3R0 One" Rodgers (raymond@badlucksoft.com)
Project Start Date: October 18, 2000
*/
#include "connection.h"
#include <string.h>
#include <Autolock.h>
#include <stdio.h>
#include <OS.h>
#include <errno.h>
#include "networkableobject.h"
#include "tcpmanager.h"
#ifdef __HAIKU__
#include <posix/sys/select.h>
#define closesocket close
#endif
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
	//BAutolock alock(ListLock);
	//if (alock.IsLocked())
		if (ConnectionList!=NULL)
			count=ConnectionList->CountItems();
	return count;
	
}
int32 Connection::ConnectionResult() {
	//printf("ConnectionResult\n");
	return connection_result;
}
bool Connection::NotifiedConnect() {
	return notified_connect;
}
void Connection::NotifyConnect(){
	BAutolock alock(lock);
	if (alock.IsLocked()) {
		if (owner!=NULL) {
			if (notified_connect==0)
			{
				uint32 status=0;
				if (already_connected)
					status=owner->ConnectionAlreadyExists(this);
				else
					status=owner->ConnectionEstablished(this);
				if (status==NetworkableObject::STATUS_NOTIFICATION_SUCCESSFUL)
					atomic_add(&notified_connect,1);
			}
		}
		
	}
	
}
bool Connection::NotifiedDisconnect(){
	bool notified=false;
//	BAutolock alock(lock);
	if (lock.LockWithTimeout(10000)==B_OK)
	{
		notified=notified_disconnect;
		lock.Unlock();
	}
	return notified;
}
void Connection::NotifyDisconnect() {
	BAutolock alock(lock);
	if (alock.IsLocked()) {
		if (owner!=NULL) {
			if (notified_disconnect==0)
			{
				uint32 status=0;
				status=owner->ConnectionTerminated(this);
				if (status==NetworkableObject::STATUS_NOTIFICATION_SUCCESSFUL)
					atomic_add(&notified_disconnect,1);
			}
		}
	}
	
}

Connection::Connection(NetworkableObject *NetObject,const char *host, uint16 port,bool secure,bool async) : InitialBufferSize(4096) {
	Init();
//	lock=new BLocker(true);
	BufferSize = InitialBufferSize;
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
	ReadBuffer = NULL;
}
Connection::~Connection() {
	//printf("Connection: Connection to %s is going away.\n",host_name);
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
//	delete lock;
//	RemoveFromList(this);
	if (ssl_server_name!=NULL)
		delete ssl_server_name;
ssl_server_name=NULL;
	if (ssl_org_unit_name!=NULL)
		delete ssl_org_unit_name;
ssl_org_unit_name=NULL;
	if (ssl_org_name!=NULL)
		delete ssl_org_name;
ssl_org_name=NULL;
	if (ssl_country_name!=NULL)
		delete ssl_country_name;
ssl_country_name=NULL;
	if (owner!=NULL) {
		owner->DestroyingConnectionObject(this);
		owner=NULL;
	}
	
	delete[] ReadBuffer;
	
}
void Connection::OwnerRelease() {
	BAutolock alock(lock);
	if (alock.IsLocked()) {
		//printf("Connection::OwnerRelease\n");
		owner=NULL;
		StopUsing();
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
	//printf("Connection::Connect()\n");
	BAutolock alock(lock);
	if (alock.IsLocked()) {
		if (IsConnected()) {
			if (already_connected==0)
				atomic_add(&already_connected,1);
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
	//printf("Connection::Connect() done\n");
	
}
void Connection::ConnectionEstablished(){
	BAutolock alock(lock);
	if (alock.IsLocked()) {
		if (use_ssl) {
			int cryptstatus;
			//printf("CRYPT_OK: %d\n",CRYPT_OK);
			cryptstatus=cryptCreateSession(&cryptSession,CRYPT_UNUSED,CRYPT_SESSION_SSL);
			//printf("session creation: %d\n",cryptstatus);
			cryptstatus=cryptSetAttribute(cryptSession,CRYPT_SESSINFO_NETWORKSOCKET,socket_id);
			//printf("passing network socket: %d\n",cryptstatus);
			cryptstatus=cryptSetAttribute(cryptSession,CRYPT_SESSINFO_ACTIVE,1);
			//printf("activating session: %d\n",cryptstatus);
			if (cryptStatusError(cryptstatus))
			{
			int errorcode,errorstrlen;
			char *errstring;
			cryptGetAttribute(cryptSession,CRYPT_ATTRIBUTE_INT_ERRORCODE,&errorcode);
			cryptGetAttributeString(cryptSession,CRYPT_ATTRIBUTE_INT_ERRORMESSAGE,NULL,&errorstrlen);
			errstring=new char[errorstrlen+1];
			memset(errstring,0,errorstrlen+1);
			cryptGetAttributeString(cryptSession,CRYPT_ATTRIBUTE_INT_ERRORMESSAGE,errstring,&errorstrlen);
			printf("error: %d - %s\n",errorcode,errstring);
			delete errstring;
			//printf("Closing connection.\n");
			last_error=ERROR_CONNECTION_RESET;
			Disconnect();
			NotifyDisconnect();
			return;
			}
			if (cryptstatus==CRYPT_ATTRIBUTE_INT_ERRORMESSAGE)
			{
				cryptSetAttribute(cryptSession,CRYPT_SESSINFO_VERSION,0);
				cryptstatus=cryptSetAttribute(cryptSession,CRYPT_SESSINFO_ACTIVE,1);
			}
			cryptGetAttribute(cryptSession,CRYPT_CTXINFO_ALGO,(int*)&cryptAlgo);
			cryptGetAttribute(cryptSession,CRYPT_CTXINFO_MODE,(int*)&cryptMode);
			cryptGetAttribute(cryptSession,CRYPT_SESSINFO_RESPONSE,&ssl_certificate);
			int len=0;
			printf("SSL Certificate Information:\n============================\n");
			cryptGetAttributeString(ssl_certificate,CRYPT_CERTINFO_COMMONNAME,NULL,&len);
			ssl_server_name=new char[len+1];
			memset((char*)ssl_server_name,0,len+1);
			cryptGetAttributeString(ssl_certificate,CRYPT_CERTINFO_COMMONNAME,(char*)ssl_server_name,&len);
			printf("Host Name: %s\n",ssl_server_name);
			cryptGetAttributeString(ssl_certificate,CRYPT_CERTINFO_ORGANIZATIONALUNITNAME,NULL,&len);
			ssl_org_unit_name=new char[len+1];
			memset((char*)ssl_org_unit_name,0,len+1);
			cryptGetAttributeString(ssl_certificate,CRYPT_CERTINFO_ORGANIZATIONALUNITNAME,(char*)ssl_org_unit_name,&len);
			printf("Org. Unit: %s\n",ssl_org_unit_name);
			cryptGetAttributeString(ssl_certificate,CRYPT_CERTINFO_ORGANIZATIONNAME,NULL,&len);
			ssl_org_name=new char[len+1];
			memset((char*)ssl_org_name,0,len+1);
			cryptGetAttributeString(ssl_certificate,CRYPT_CERTINFO_ORGANIZATIONNAME,(char*)ssl_org_name,&len);
			printf("Organizaiton: %s\n",ssl_org_name);
			cryptGetAttributeString(ssl_certificate,CRYPT_CERTINFO_COUNTRYNAME,NULL,&len);
			ssl_country_name=new char[len+1];
			memset((char*)ssl_country_name,0,len+1);
			cryptGetAttributeString(ssl_certificate,CRYPT_CERTINFO_COUNTRYNAME,(char*)ssl_country_name,&len);
			printf("Country: %s\n",ssl_country_name);
			cryptGetAttribute(cryptSession,CRYPT_CTXINFO_KEYSIZE,(int*)&ssl_encrypt_bits);
			ssl_encrypt_bits*=8;
			printf("Strength: %ld bits\n============================\n",ssl_encrypt_bits);

		}
		if (notified_connect==0) {
			first_data_received_time=real_time_clock_usecs();
//			if (owner!=NULL)
//				owner->ConnectionEstablished(this);
//			atomic_add(&notified_connect,1);
			//printf("connection %p: notified connect %ld\n",this,notified_connect);
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
			cryptDestroySession(cryptSession);
			closesocket(socket_id);
			already_connected=0;
			socket_id=-1;
		}
	}
}

bool Connection::IsConnected() {
	bool is_connected=false;//so that we don't accidentally disconnect prematurely if the lock times out
//	BAutolock alock(lock);
	if (lock.LockWithTimeout(25000)==B_OK) {
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
						//printf("Connection::IsConnected Would Block\n");
						last_error=ERROR_WOULD_BLOCK;
						break;
					case EINTR:
						last_error=ERROR_SOCKET_INTERRUPTED;
						break;
					case ECONNRESET:
						//connection reset is usually a sign that the server closed the connection.
//						printf("Connection::IsConnected Connection Reset\n");
						NotifyDisconnect();
						last_error=ERROR_CONNECTION_RESET;
						is_connected=false;
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
				lock.Unlock();
				return false;
			}
			if (FD_ISSET(socket_id,&eset)) {
				//An exception occured...
				printf("Exception on connection to \"%s\":\t%d - %s\n",host_name,errno,strerror(errno));
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
					lock.Unlock();
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
		lock.Unlock();
	}
	return is_connected;
}
bool Connection::IsDataWaiting() {
	bool waiting_data=false;
//	BAutolock alock(lock);
	if (lock.LockWithTimeout(10000)==B_OK) {
		if (socket_id>=0) {
			struct timeval tv;
			tv.tv_sec=0;//2;
			tv.tv_usec=50000;
			struct fd_set fds;
			FD_ZERO(&fds);
			FD_SET(socket_id,&fds);
			int32 err=select(socket_id+1,&fds,NULL,NULL,&tv);
			if (err<0) {
				//printf("IsDataWaiting socket error\n");
				lock.Unlock();
				return false;
			}
			waiting_data=FD_ISSET(socket_id,&fds);
			//printf("IsDataWaiting: %s\n",(waiting_data ? "yes":"no"));
		}
		lock.Unlock();
	}
	return waiting_data;
}
bool Connection::IsSecure() {
	bool secured=false;
//	BAutolock alock(lock);
//	if (alock.IsLocked()) {
		if ((use_ssl) && (ssl_encrypt_bits>0))
			secured=true;
		else
			secured=false;
//	}
	return secured;
}
uint16 Connection::EncryptionBits()
{
	return ssl_encrypt_bits;
}
const char *Connection::SSLServerName()
{
	return ssl_server_name;
}
const char *Connection::SSLOrgName()
{
	return ssl_org_name;
}
const char *Connection::SSLOrgUnitName()
{
	return ssl_org_unit_name;
}
const char *Connection::SSLCountry()
{
	return ssl_country_name;
}

void Connection::Init() {
	bytes_read_by_owner=0L;
	already_connected=0L;
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
	bytes_per_second=0L;
	first_data_received_time=0L;
	ssl_encrypt_bits=0;
	ssl_server_name=NULL;
	ssl_org_unit_name=NULL;
	ssl_org_name=NULL;
	ssl_country_name=NULL;
	cryptSession=0;
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
	//printf("[NewSession]\n");
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
		first_data_received_time=0L;
		session_bytes_sent=0L;
		session_bytes_received=0L;
		session_bytes_out=0L;
		last_error=NO_ERROR;
		bytes_per_second=0;
		notified_connect=0;
		notified_disconnect=0;
		atomic_add(&already_connected,1);
		if (!IsBufferEmpty())
			EmptyBuffer();
	}
	return id;
}
double Connection::BytesPerSecond(){
	return bytes_per_second;
}

status_t Connection::AppendData(void *data, off_t size) {
	fflush(stdout);
	status_t status=B_BUSY;
//	BAutolock alock(lock);
//	if (alock.IsLocked()) {

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
		//printf("[Connection::AppendData] Buffer is now %Ld bytes long\n",DataSize());
//	}
	return status;
}
off_t Connection::Receive(void *data, off_t max_size) {
	//printf("[Receive]\n");
	fflush(stdout);
	off_t size=0L;
//	BAutolock alock(lock);
	if (lock.LockWithTimeout(10000)==B_OK) {
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
			//printf("[Receive] at most %Ld can be read now.\n", max_size-size);
			bytes=current->buffer->GetData((unsigned char*)data+size,max_size-size);
			size+=bytes;
			//printf("[Receive] %ld (%Ld) bytes were copied into protocol buffer.\n",bytes,size);
			prev=current;
			current=current->next;
			delete prev->buffer;
			delete prev;
			prev=NULL;
		}
		buffer_in=current;
		lastusedtime=real_time_clock();
		session_bytes_out+=size;
		//printf("[Connection::Receive] %Ld bytes have been transferred to owner. %Ld bytes left in buffer.\n\n",session_bytes_received,DataSize());
		fflush(stdout);
		lock.Unlock();
	}
	return size;
}
off_t Connection::DataSize() {
	off_t size=0L;
	if (lock.LockWithTimeout(15000)==B_OK) {
		buffer_list_st *current=buffer_in;
		while(current!=NULL) {
			if (current->buffer!=NULL)
				size+=current->buffer->Size();
			current=current->next;
		}
		lock.Unlock();
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
					cryptPushData(cryptSession,data,size,(int*)&bytes);
					cryptFlushData(cryptSession);
				}
				catch(...) {
				}
		
			} else				
				bytes=send(socket_id,(char *)data, size,0);
		}
		if (bytes==-1)
		{
				switch(errno) {
					case EWOULDBLOCK:
						last_error=ERROR_WOULD_BLOCK;
						break;
					case EINTR:
						last_error=ERROR_SOCKET_INTERRUPTED;
						break;
					case ECONNRESET:
						//connection reset is usually a sign that the server closed the connection.
//						printf("Connection::IsConnected Connection Reset\n");
						NotifyDisconnect();
						last_error=ERROR_CONNECTION_RESET;
						break;
					case ENOTCONN:
						last_error=ERROR_NOT_CONNECTED;
						break;
					case EBADF:
						last_error=ERROR_INVALID_SOCKET;
						break;
					default: {
						printf("%d\t%s\n",errno,strerror(errno));
						last_error=ERROR_UNKNOWN;
					}
				}
		}
		else
		{
			session_bytes_sent+=bytes;
			total_bytes_sent+=bytes;
			TCPMan->AddBytesSent(bytes);
		}
		lastusedtime=real_time_clock();
	}
	return bytes;
}
int32 Connection::IsInUse() {
	int32 isinuse=0;
	//BAutolock alock(lock);
	//if (alock.IsLocked())
		isinuse=in_use;
	return isinuse;
}
void Connection::StartUsing() 
{
	//printf("Connection::StartUsing beginning.\n");
	BAutolock alock(lock);
	if (alock.IsLocked())
		if (in_use==0)
		{
			in_use=1;
//			atomic_add(&in_use,1);
//			if (already_connected==0)
//				atomic_add(&already_connected,1);
//			if (owner!=NULL)
//				owner->ConnectionAlreadyExists(this);
//			if (notified_connect==0)
//				atomic_add(&notified_connect,1);
			
		}
		//printf("Connection::StartUsing done.\n");
}
void Connection::StopUsing()
{
	BAutolock alock(lock);
	if (alock.IsLocked())
		in_use=0;
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
//	fflush(stdout);
//	BAutolock alock(lock);
	if (lock.LockWithTimeout(15000)==B_OK) {
		uint32 bytes=0;
		if (!ReadBuffer) {
			// No buffer created yet
			ReadBuffer = new unsigned char[BufferSize];
		}
	
		if (!IsInUse())
		{
			if (use_ssl) {
				try {
					cryptPopData(cryptSession,(unsigned char*)ReadBuffer, BufferSize, (int*)&bytes);
				}
				catch(...) {
				}
		
			} else				
				bytes=recv(socket_id,(char *)ReadBuffer, BufferSize, 0);
			bytes=0;
			lock.Unlock();
			return;
		}
		if (socket_id>=0) {
			if (!nonblocking)
			{//set the socket to non-blocking mode for the receive then set back
				int option=1;
				setsockopt(socket_id,SOL_SOCKET,SO_NONBLOCK,&option,sizeof(option));
			}
			
			if (use_ssl) {
				try {
					cryptPopData(cryptSession,(unsigned char*)ReadBuffer, BufferSize, (int*)&bytes);
				}
				catch(...) {
				}
		
			} else				
				bytes=recv(socket_id,(char *)ReadBuffer, BufferSize,0);
			if (!nonblocking)
			{//set the socket to non-blocking mode for the receive then set back
				int option=0;
				setsockopt(socket_id,SOL_SOCKET,SO_NONBLOCK,&option,sizeof(option));
			}
		}
		
		if (bytes>0) { //negative byte counts are errors... zero ones are useless
//			printf("[RetrieveData] just retrieved %ld bytes\n",bytes);
			fflush(stdout);
			//call the networkable object's data waiting function.
			
				if (first_data_received_time==0L)
				{
//					printf("setting first_data_received_time\n");
					first_data_received_time=real_time_clock_usecs();
				}
			bigtime_t currenttime=real_time_clock_usecs();

			double tdiff=(currenttime-first_data_received_time)/1000000.0;
//			printf("%Ld bytes received\t%1.2f seconds\n",session_bytes_received,tdiff);
			session_bytes_received+=bytes;
			if (tdiff<1.0)
			{
				bytes_per_second=session_bytes_received*(1.0/tdiff);
			} else {
				bytes_per_second=session_bytes_received/(tdiff/1.0);
			}
			printf("Estimated Bytes Per Second: %1.4f, bytes received: %li\n",bytes_per_second, bytes);
			total_bytes_received+=bytes;
			TCPMan->AddBytesReceived(bytes);
			AppendData(ReadBuffer, bytes);
			if (bytes == BufferSize) {
				// We might have been able to receive more if we had a bigger buffer
				// Make it a bit bigger
				BufferSize += InitialBufferSize;
				printf("Resizing receive buffer to %li\n", BufferSize);
				delete[] ReadBuffer;
				ReadBuffer = new unsigned char[BufferSize];
			}
			else {
				// Buffer was big enough. Just reset the bytes written
				memset(ReadBuffer, 0, bytes);
			}
			lastusedtime=real_time_clock();
			
//			if (owner!=NULL)
//				owner->DataIsWaiting(this);
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
		
		
		lock.Unlock();
		}
	//	printf("Connection::Retrieve() is done.\n");
}
int32 Connection::LastUsed(){
	return lastusedtime;
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
	return last_error;
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
