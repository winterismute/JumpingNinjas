#ifndef _NETWORKEVENT
#define _NETWORKEVENT
#include <string>
class NetworkEvent
{
	
public:
	enum NetworkEventType {
		SCORE
	};

	NetworkEvent(void);
	std::string getMethodName() const;
	NetworkEventType getType() const;
	~NetworkEvent(void);

protected:
	std::string m_methodToCall;

	NetworkEventType m_type;
};

#endif // _NETWORKEVENT