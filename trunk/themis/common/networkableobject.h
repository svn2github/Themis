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
#ifndef _networkable_object_
#define _networkable_object_
#include <SupportDefs.h>
namespace _Themis_Networking_ {
	class Connection;
	/*!
		\brief Classes that will utilize the TCPManager must be derived from NetworkableObject.
	*/
	class NetworkableObject {
		public:
			/*!
			\brief A connection to the server has been established.
			
			
			*/
			virtual uint32 ConnectionEstablished(Connection *connection);
			/*!
			\brief There is already a connection to this server that is available for use.
			
			
			*/
			virtual uint32 ConnectionAlreadyExists(Connection *connection);
			/*!
			\brief The connection to the server has terminated.
			
			
			*/
			virtual uint32 ConnectionTerminated(Connection *connection);
			/*!
			\brief There is data waiting to be received on this Connection.
			
			
			*/
			virtual uint32 DataIsWaiting(Connection *connection);
			/*!
			\brief There has been an error on this Connection.
			
			
			*/
			virtual uint32 ConnectionError(Connection *connection);
			/*!
			\brief The connection attempt with this Connection object was not successful.
			
			
			*/
			virtual uint32 ConnectionFailed(Connection *connection);
			/*!
			\brief This Connection object is about to be deleted.
			
			
			*/
			virtual uint32 DestroyingConnectionObject(Connection *connection);
		
		static const uint32 STATUS_NOTIFICATION_FAILED=0x0;
		static const uint32 STATUS_NOTIFICATION_SUCCESSFUL=0x1;
	};
};

#endif
