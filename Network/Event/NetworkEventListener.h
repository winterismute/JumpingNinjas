#ifndef _NETWORKEVENTLISTENER
#define  _NETWORKEVENTLISTENER
#include "NetworkEvent.h"

//Interface class
class NetworkEventListener {
public:
	virtual void onNetworkEvent(NetworkEvent* evt) = 0;
};


#endif