#ifndef cookie_man
#define cookie_man

#include "plugclass.h"
#include "cookiedefs.h"
#include <Handler.h>
#include <Message.h>
#include <SupportDefs.h>
#include <StorageKit.h>

class cookieman: public BHandler, public PlugClass {
	private:
	public:
		cookieman(BMessage *info=NULL);
		~cookieman();
		void MessageReceived(BMessage *msg);
		bool IsHandler();
		BHandler *Handler();
		bool IsPersistent();
		uint32 PlugID(){return PlugIDdef;};
		char *PlugName(){return PlugNamedef;};
		float PlugVersion(){return PlugVersdef;};
//		bool RequiresHeartbeat(){return true;};
		void Heartbeat();
		status_t ReceiveBroadcast(BMessage *msg);
		int32 Type();
};


#endif
