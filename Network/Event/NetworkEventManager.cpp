#include "NetworkEventManager.h"

template<> NetworkEventManager* Ogre::Singleton<NetworkEventManager>::ms_Singleton = 0;

NetworkEventManager::NetworkEventManager(void)
{
}


NetworkEventManager::~NetworkEventManager(void)
{
	m_events.clear();
	std::vector<NetworkEvent*>::iterator evtIt;
	for(evtIt = m_events.begin(); evtIt != m_events.end(); ++evtIt){
		delete *evtIt;
	}
}

NetworkEventManager* NetworkEventManager::getSingletonPtr()
{
	return ms_Singleton;
}

void NetworkEventManager::registerListener( NetworkEventListener* listener )
{
	m_listeners.push_back(listener);
}

void NetworkEventManager::update()
{
	std::vector<NetworkEventListener*>::iterator it;
	std::vector<NetworkEvent*>::iterator evtIt;
	for(it = m_listeners.begin(); it != m_listeners.end(); ++it){
		for(evtIt = m_events.begin(); evtIt != m_events.end(); ++evtIt){
			(*it)->onNetworkEvent(*evtIt);
		}
	}

	for(evtIt = m_events.begin(); evtIt != m_events.end(); ++evtIt){
		 delete (*it);
	}
	m_events.clear();
}

void NetworkEventManager::fireEvent( NetworkEvent* evt )
{
	m_events.push_back(evt);
}
