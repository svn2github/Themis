#ifndef _new_message_system_admin_
#define _new_message_system_admin_

#include "msgsystem.h"

class MessageDaemon: public MessageSystem {
	public:
		MessageDaemon();
		~MessageDaemon();
		void Start();
		void Stop();
};

#endif
