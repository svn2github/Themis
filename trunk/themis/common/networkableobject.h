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

namespace _Themis_Networking_ {
	class Connection;
	class NetworkableObject {
		public:
			virtual void ConnectionEstablished(Connection *connection);
			virtual void ConnectionAlreadyExists(Connection *connection);
			virtual void ConnectionTerminated(Connection *connection);
			virtual void DataIsWaiting(Connection *connection);
			virtual void ConnectionError(Connection *connection);
			virtual void ConnectionFailed(Connection *connection);
			virtual void DestroyingConnectionObject(Connection *connection);
		
	};
};

#endif
