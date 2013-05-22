#ifndef _NETWORKEVENTMANAGER
#define _NETWORKEVENTMANAGER
#include <OgreSingleton.h>
#include <vector>

#include "NetworkEvent.h"
#include "NetworkEventListener.h"

class NetworkEventManager :
	public Ogre::Singleton<NetworkEventManager>
{
public:
	NetworkEventManager(void);
	void fireEvent(NetworkEvent* evt);
	void update();
	void registerListener(NetworkEventListener* listener);

	static NetworkEventManager* getSingletonPtr();
	~NetworkEventManager(void);

private:
	std::vector<NetworkEvent*> m_events;
	std::vector<NetworkEventListener*> m_listeners;
};

#endif // ifndef _NETWORKEVENTMANAGER