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
/*!
\file
\brief The TCP/IP connection manager declaration.


*/

#ifndef _tcp_manager_
#define _tcp_manager_

#include "connection.h"
#include "networkableobject.h"
#include <OS.h>
#include <Locker.h>
namespace _Themis_Networking_ {
//	class Connection;
	
/*!
\brief The TCP/IP connection manager class.

This class is what the older TCPLayer class should have been. It is designed to handle
multiple connections to multiple hosts quickly and efficiently.
*/
	class Connection;
	class TCPManager {
		private:
		friend class Connection;
		//void RemoveConnection(Connection *target);
/*!
\brief Quits the TCPManager thread.


*/
			volatile int32 _quitter_;
/*!
\brief Initializes the TCPManager object.


*/
			void Init();
/*!
\brief Locking mechanism.


*/
			BLocker lock;
#ifdef USEOPENSSL
/*!
\brief SSL method if available.


*/
			SSL_METHOD *sslmeth;
/*!
\brief SSL context if available.


*/
			SSL_CTX* sslctx;
#endif
/*!
\brief The number of connection sessions reused.


*/
			uint64 sessions_reused;
/*!
\brief The total number of connection sessions.


*/
			uint64 sessions_created;
		
/*!
\brief The total number of bytes sent during the execution of Themis.


*/
			off_t total_bytes_sent;
/*!
\brief The total number of bytes received during the execution of Themis.


*/
			off_t total_bytes_received;
			static TCPManager *_manager_;
			thread_id thread;
			static void AlarmHandler(int signum);
			static sem_id process_sem_1;
			static sem_id process_sem_2;
		
		
		public:
/*!
\brief The constructor.


*/
			TCPManager();
/*!
\brief The destructor.


*/
			~TCPManager();
			static TCPManager *ManagerPointer();
		
		
/*!
\brief Quit the TCPManager.


*/
			void Quit();
			void Start();
			static int32 _Thread_Starter(void *arg);
			int32 _Manager_Thread();
		
/*!
\brief Create a new connection.


*/
			Connection *CreateConnection(NetworkableObject *net_obj,const char *host,uint16 port,bool secure=false,bool asynch=true);
/*!
\brief Terminate a connection and destroy the connection object.

This function will go through the process of severing the connection between the browser
and server, then destroying the Connection object. Do not confuse this with the DoneWithSession()
function which will, more or less, just mark the Connection object as being available
again for further use.
*/
			void Disconnect(Connection *connection);
/*!
\brief Reset a connection for reuse.

This function will reset the session data for a particular connection and prepare it
to be used again by another session. Do not confuse this with Disconnect(). This function
does not terminate the connection (actively at least).
*/
			void DoneWithSession(Connection *connection);
/*!
\brief Increase the total number of bytes sent.


*/
			void AddBytesSent(off_t bytes);
/*!
\brief Increase the total number of bytes received.


*/
			void AddBytesReceived(off_t bytes);
		
/*!
\brief Is SSL enabled in this application?


*/
			bool SSLEnabled();
#ifdef USEOPENSSL
/*!
\brief Returns the SSL method if available.


*/
			SSL_METHOD *SSLMethod();
/*!
\brief Returns the SSL context if available.


*/
			SSL_CTX *SSLContext();
#endif
		static const int32	DEFAULT_TIMEOUT	=	300;
		
	};
};

#endif
