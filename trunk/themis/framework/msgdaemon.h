/*
Copyright (c) 2001 Z3R0 One. All Rights Reserved.

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
/*!
\file
\brief This file contains the declaration of the MessageDaemon class.


*/
#ifndef _new_message_system_admin_
#define _new_message_system_admin_

#include "msgsystem.h"
/*!
\brief The administrative class of the MessageSystem.

This class acts as an administrative tool for the MessageSystem. It starts and stops the system
and prints statistical information upon shutdown.

\note This is probably the only class in the entire message system that is not actually
a message system target. There is no way to send to or receive messages from the MessageDaemon
class itself. (Not that it's impossible, just that it doesn't register with the system and isn't
meant to be communicated with.)
*/
class MessageDaemon: public MessageSystem {
	public:
		MessageDaemon();
		~MessageDaemon();
		void Start();
		void Stop();
};

#endif
