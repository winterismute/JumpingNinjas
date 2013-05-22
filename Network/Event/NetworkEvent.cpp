#include "NetworkEvent.h"


NetworkEvent::NetworkEvent(void)
{
}


NetworkEvent::~NetworkEvent(void)
{
}

std::string NetworkEvent::getMethodName() const
{
	return m_methodToCall;
}

NetworkEvent::NetworkEventType NetworkEvent::getType() const
{
	return m_type;
}
