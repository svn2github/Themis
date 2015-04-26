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
#ifndef _network_connection_
#define _network_connection_

#include <Locker.h>
#include <SupportDefs.h>
#include <List.h>
#include "netbuffer.h"
//#include "networkableobject.h"
#include "cryptlib.h"
#ifdef  USEBONE
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#define closesocket close
#else
#ifdef HAIKU
#include <posix/sys/socket.h>
#else
#include <socket.h>
#endif
#endif

#include <netdb.h>

// Declarations
class BufferPool;

namespace _Themis_Networking_ {
	class TCPManager;
	class NetworkableObject;
	/*!
	\brief This class handles each individual connection and stores received data temporarily.
	
	
	*/
	class Connection {
		private:
			const int32 InitialBufferSize;
			int32 BufferSize;
			unsigned char * ReadBuffer;
			static int32 SystemReady;
		
			friend class TCPManager; //lazy man's hack for the moment, to get
									//access to socket_id, ssl, etc... Write functions
									//to access these things the right way.
			BLocker lock;
			off_t bytes_read_by_owner;
		
			off_t session_bytes_sent, session_bytes_received;
			off_t total_bytes_sent, total_bytes_received;
			off_t session_bytes_out;
	/*!
	\brief 
	
	
	*/
			int32 socket_id;
	/*!
	\brief 
	
	
	*/
			sockaddr_in sa;
		
	/*!
	\brief 
	
	
	*/
			void Init();
	/*!
	\brief 
	
	
	*/
			int32 session_id;
	/*!
	\brief A pointer to the object that is using this object.
	
	
	*/
			NetworkableObject *owner;
	/*!
	\brief 
	
	
	*/
			static BLocker *ListLock;
	/*!
	\brief 
	
	
	*/
			static BList *ConnectionList;
	/*!
	\brief 
	
	
	*/
			static void AddToList(Connection *newcomer);
	/*!
	\brief 
	
	
	*/
			static void RemoveFromList(Connection *target);
	/*!
	\brief 
	
	
	*/
			volatile int32 in_use;
	/*!
	\brief 
	
	
	*/
			int32 lastusedtime;
	/*!
	\brief 
	
	
	*/
			const char *host_name;
	/*!
	\brief 
	
	
	*/
			uint16 host_port;
	/*!
	\brief 
	
	
	*/
			int32 connection_result;
		

	/*!
	\brief 
	
	
	*/
			bool use_ssl;
	/*!
	\brief 
	
	
	*/
			bool nonblocking;
			CRYPT_SESSION cryptSession;
			CRYPT_MODE_TYPE cryptMode;
			CRYPT_ALGO_TYPE cryptAlgo;
			CRYPT_CERTIFICATE ssl_certificate;
			const char *ssl_server_name;
			const char *ssl_org_unit_name;
			const char *ssl_org_name;
			const char *ssl_country_name;
			int32 ssl_encrypt_bits;
			int32 last_error;
			double bytes_per_second;
			bigtime_t first_data_received_time;
			volatile int32 notified_connect;
			volatile int32 notified_disconnect;
			volatile int32 already_connected;
			BufferPool * mBuffers;
		
		public:
			uint16 EncryptionBits();
			const char *SSLServerName();
			const char *SSLOrgName();
			const char *SSLOrgUnitName();
			const char *SSLCountry();
			int32	Error();
			void	ClearError();
			static const char *	ErrorString(int32 which=NO_ERROR);
			double BytesPerSecond();
			int32 ConnectionResult();
			bool NotifiedConnect();
			void NotifyConnect();
			bool NotifiedDisconnect();
			void NotifyDisconnect();
	/*!
	\brief This function releases the object for further use.
	
	This is a secondary way to release the object for further use. By calling this function,
	a class derived from NetworkableObject can stop using this Connection object without
	directly calling the TCPManager. This makes it a bit quicker than calling DoneWithSession()
	or Disconnect(), although it will not terminate the connection to the server. For
	all intents and purposes, calling OwnerRelease() is identical to calling DoneWithSession()
	with the exception that DoneWithSession is a TCPManager function.
	*/
			void OwnerRelease();
	/*!
	\brief 
	
	
	*/
			static TCPManager *TCPMan;
	/*!
	\brief 
	
	
	*/
			Connection(NetworkableObject *NetObject,const char *host, uint16 port,bool secure=false,bool async=true);
	/*!
	\brief 
	
	
	*/
			~Connection();
	/*!
	\brief 
	
	
	*/
			void AssignNetObject(NetworkableObject *net_obj);
			
	/*!
	\brief 
	
	
	*/
			static Connection *ConnectionAt(int32 which);
	/*!
	\brief 
	
	
	*/
			static bool HasConnection(Connection *target);
	/*!
	\brief 
	
	
	*/
			static int32 CountConnections();
		
	/*!
	\brief 
	
	
	*/
			void Connect();
	/*!
	\brief 
	
	
	*/
			void ConnectionEstablished();
	/*!
	\brief 
	
	
	*/
			void ConnectionTerminated();
	/*!
	\brief 
	
	
	*/
			void Disconnect();
		
			
	/*!
	\brief 
	
	
	*/
			off_t BytesSent(bool session=true);
	/*!
	\brief 
	
	
	*/
			off_t BytesReceived(bool session=true);
	/*!
	\brief 
	
	
	*/
			bool IsConnected();
	/*!
	\brief 
	
	
	*/
			bool IsBufferEmpty();
	/*!
	\brief 
	
	
	*/
			bool IsDataWaiting();
	/*!
	\brief 
	
	
	*/
			bool IsSecure();
		
	/*!
	\brief 
	
	
	*/
			void EmptyBuffer();
	/*!
	\brief 
	
	
	*/
			off_t DataSize();
			
	/*!
	\brief 
	
	
	*/
			off_t Receive(void *data, off_t max_size);
			
	/*!
	\brief 
	
	
	*/
			status_t AppendData(void *data, off_t size);
	/*!
	\brief 
	
	
	*/
			int32 SessionID();
	/*!
	\brief 
	
	
	*/
			int32 NewSession();
	/*!
	\brief 
	
	
	*/
			int32 IsInUse();
	/*!
	\brief 
	
	
	*/
			void StartUsing();
	/*!
	\brief 
	
	
	*/
			void StopUsing();
	/*!
	\brief 
	
	
	*/
			int32 LastUsed();
	/*!
	\brief 
	
	
	*/
			bool Matches(const char *host, uint16 port);

	/*!
	\brief 
	
	
	*/
			off_t Send(void *data, off_t datasize);
	/*!
	\brief 
	
	
	*/
			void RetrieveData();
			void TimeOut();
		
		//various status codes
		static const int32	STATUS_CONNECTION_IN_PROGRESS	=	1010;
		static const int32	STATUS_CONNECTION_ESTABLISHED	=	1011;
		static const int32	STATUS_CONNECTION_ALREADY_ESTABLISHED	=	1012;
		
		static const int32	STATUS_CONNECTION_LOST	=	-1000;
		static const int32	STATUS_CONNECTION_DISCONNECTED	=	STATUS_CONNECTION_LOST;
		static const int32	STATUS_BLOCKING	=	-1010;
		static const int32	NO_ERROR	=	0x0;
		
		static const int32	BASE_ERROR	=	0xe000;
		static const int32	ERROR_UNKNOWN	=	BASE_ERROR+1;
		static const int32	ERROR_SERVER_ADDRESS_NOT_FOUND	=	BASE_ERROR+2;
		static const int32	ERROR_CONNECTION_REFUSED	=	BASE_ERROR+3;
		static const int32	ERROR_WOULD_BLOCK	=	BASE_ERROR+4;
		static const int32	ERROR_SOCKET_INTERRUPTED	=	BASE_ERROR+5;
		static const int32	ERROR_CONNECTION_RESET	=	BASE_ERROR+6;
		static const int32	ERROR_NOT_CONNECTED	=	BASE_ERROR+7;
		static const int32	ERROR_INVALID_SOCKET	=	BASE_ERROR+8;
		static const int32	ERROR_NO_MEMORY	=	BASE_ERROR+9;
		static const int32	ERROR_TIMED_OUT	=	BASE_ERROR+10;
		
		
	};
};

#endif

