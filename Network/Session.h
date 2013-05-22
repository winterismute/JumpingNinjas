#pragma once

#include <stdio.h>
#include <stdlib.h>
#include "Gets.h"
#include "RakNetTypes.h"
#include "MessageIdentifiers.h"
#include "RakPeerInterface.h"
#include "RPC3.h"
#include "RakSleep.h"
#include "NetworkIDObject.h"
#include "NetworkIDManager.h"
#include <string>
#include "BitStream.h"
#include <process.h> 
#include "../Game/Entity/Player/RenderableChar.h"
#include "Event/ScoreEvent.h"

enum SessionType{
	CLIENT, HOST
};

class Session
{
public:
	Session(SessionType);
	~Session(void);

	//Indicates if the player is HOST or CLIENT
	static SessionType sessionType;
	//bool that indicates if the client/host thread is active
	static bool threading;

	static void SendInformation(RakNet::RakString msg, RakNet::RPC3*, RakNet::RPC3*);
	void SendVelocity(Ogre::String, int, int, int);
	void SendCharacterUpdate(RenderableChar*);
	void Connect(std::string, int);
	void CreateHost(int, std::string);
	static void StartHost(void* pVoid);
	static void StartClient(void* pVoid);

	void SyncCharacters(RakNet::RPC3*);

	void SendScore(ScoreEvent* evt);

private:
	//connection ip
	std::string ip;
	//connection port
	int port;

	//RPC3 instance used to make function calls on other PC's
	RakNet::RPC3 *rpc3Inst;
	//RakNet networkIDManager used to send information through RakNet
	RakNet::NetworkIDManager networkIDManager;
	//Basic RakNet networking interface
	RakNet::RakPeerInterface* rakPeer;	
};

/*
** list with arguments, used to send information to a static method
*/
class ArgumentList
{
    public:
		std::string ip;
        int port;
		std::string password;
		RakNet::RakPeerInterface* inter;
		SessionType type;
		RakNet::RPC3* rpc3;
};