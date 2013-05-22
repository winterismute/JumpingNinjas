#include "Session.h"
#include "../Game/Game.h"
#include "OgreVector3.h"
#include "../Game/Score/ScoreManager.h"
#include <boost/lexical_cast.hpp>

SessionType Session::sessionType;
bool Session::threading;

using namespace RakNet;

//==============The following methods are local methods that respond to RPC3 calls
void ReceiveInformation(RakNet::RakString msg, RakNet::RPC3* rpcFromNetwork){
	std::string msgStr = msg.C_String();
	printf("Message from '%i': %s\n", rpcFromNetwork,msgStr.c_str());
	
}

void SyncClients(RakString numberOfClientsStr){
	int numberOfClients = atoi(numberOfClientsStr.C_String());
	int numberOfChars = Game::getSingletonPtr()->GetCharacters().size();
	std::vector<RenderableChar*> characters = Game::getSingletonPtr()->GetCharacters();

	for(int i = 2; i <= numberOfClients; i++){
		std::stringstream ss;
		ss << "Client" << i;
		bool found = false;
		for(int j = 0; j < numberOfChars; j++){
			if(characters.at(j)->GetNetworkName()==ss.str()){
				found = true;
				break;
			}
		}
		if(!found){
			Game::getSingletonPtr()->CreateCharacter(ss.str().c_str());	
		}
	}
}

void UpdateCharacter(RakString name, RakString state, RakString velX, RakString velY, RakString velZ, RakString xPos, RakString yPos, RakString zPos, RakNet::RPC3* rpcFromNetwork){
	std::vector<RenderableChar*> characters = Game::getSingletonPtr()->GetCharacters();
	RenderableChar* localChar = Game::getSingletonPtr()->getLocalPlayer();
	for (std::vector<RenderableChar*>::iterator i = characters.begin(); i != characters.end(); ++i) {
		if((*i)->GetNetworkName() == name.C_String() && (*i)->GetNetworkName() != localChar->GetNetworkName()){
			(*i)->setVelX(atoi(velX));
			(*i)->setVelY(atoi(velY));
			(*i)->setVelZ(atoi(velZ));
			Ogre::Vector3 vec;
			vec.x = atoi(xPos);
			vec.y = atoi(yPos);
			vec.z = atoi(zPos);
			(*i)->SetPosition(vec/10000);
			//(*i)->translateX(vec.x - (*i)->getWorldPosition().x);
			//(*i)->translateY(vec.y - (*i)->getWorldPosition().y);
			(*i)->SetState((RenderableChar::STATE_TYPE)atoi(state));
			return;
		}
	}
}

void UpdateCharacterVelocity(RakString name, RakString velX, RakString velY, RakString velZ, RakNet::RPC3* rpcFromNetwork){
	std::vector<RenderableChar*> characters = Game::getSingletonPtr()->GetCharacters();
	for (std::vector<RenderableChar*>::iterator i = characters.begin(); i != characters.end(); ++i) {
		if((*i)->GetNetworkName() == name.C_String()){
			(*i)->setVelX(atoi(velX));
			(*i)->setVelY(atoi(velY));
			(*i)->setVelZ(atoi(velZ));
			return;
		}
	}
}

void AddScore(RakString name){
	ScoreManager::getSingletonPtr()->addPoint(name.C_String());
}

void SetScore(RakString name, RakString points){
	ScoreManager::getSingletonPtr()->setScore(name.C_String(), boost::lexical_cast<int>(points.C_String()));
}

void ResetStreak(RakString name){
	ScoreManager::getSingletonPtr()->resetStreak(name.C_String());
}

void RemoveCharacter(RakString name){
	Game::getSingletonPtr()->RemoveCharacter(name.C_String());
}

void ReceiveID(RakString id){
	Game::getSingletonPtr()->getLocalPlayer()->SetNetworkName(id.C_String());
}
//==========================================

Session::Session(SessionType type)
{
	rpc3Inst = new RakNet::RPC3;
	rpc3Inst->SetNetworkIDManager(&networkIDManager);
	rakPeer = RakNet::RakPeerInterface::GetInstance();
	//we want to find out if the user is client and host, but for now assume it is a client
	Session::sessionType = type;
}

//This method connects this game instance to a host game instance through the network
void Session::Connect(std::string ip, int port){
	this->ip = ip;
	this->port = port;
	//the argumentList is build to have these parameters available in the static StartCliet method
	ArgumentList* pArgumentList = new ArgumentList;
	pArgumentList->inter = rakPeer;
	pArgumentList->password = "";
	pArgumentList->type = CLIENT;
	pArgumentList->port = port;
	pArgumentList->ip = ip;
	pArgumentList->rpc3 = rpc3Inst;

	//start the thread where we constantly listen to the network
	threading = true;
	_beginthread(StartClient, 0, pArgumentList);
}

//This static method is called when the client wants to connect to the server
void Session::StartClient(void * pVoid){
	//A parameter list is given with needed variables
	ArgumentList* pArgumentList = (ArgumentList*)pVoid;
	RakNet::RakPeerInterface* pClient;
	pClient = pArgumentList->inter;
	SessionType type = pArgumentList->type;
	RakNet::RPC3* rpc3Inst = pArgumentList->rpc3;
	SystemAddress clientID=UNASSIGNED_SYSTEM_ADDRESS;

	//Register the RPC3 functions we want to call upon
	RPC3_REGISTER_FUNCTION(rpc3Inst, ReceiveInformation);
	RPC3_REGISTER_FUNCTION(rpc3Inst, UpdateCharacter);
	RPC3_REGISTER_FUNCTION(rpc3Inst, UpdateCharacterVelocity);
	RPC3_REGISTER_FUNCTION(rpc3Inst, SyncClients);
	RPC3_REGISTER_FUNCTION(rpc3Inst, ReceiveID);
	RPC3_REGISTER_FUNCTION(rpc3Inst, RemoveCharacter);
	
	//Create the basic connection with help of RakNet
	SocketDescriptor socketDescriptor(0,0);
	socketDescriptor.socketFamily=AF_INET;
	pClient->Startup(1,&socketDescriptor, 1);
	pClient->Ping( "255.255.255.255", 50000, true, 0 );

	// Holds packets
	Packet* p;

	//Make sure the RPC3 is attached as a plugin, else it won't work
	pClient->AttachPlugin(rpc3Inst);

	char message[2048];
	// Loop for input
	while (Session::threading)
	{
		// This sleep keeps RakNet responsive
		RakSleep(30);
		#ifdef _WIN32
			Sleep(30);
		#else
			usleep(30 * 1000);
		#endif

		// Get a packet from either the server or the client
		for (p=pClient->Receive(); p; pClient->DeallocatePacket(p), p=pClient->Receive())
		{
			switch (p->data[0])
			{
			case ID_DISCONNECTION_NOTIFICATION:
				printf("ID_DISCONNECTION_NOTIFICATION\n");
				break;
			case ID_ALREADY_CONNECTED:
				printf("ID_ALREADY_CONNECTED\n");
				break;
			case ID_CONNECTION_ATTEMPT_FAILED:
				printf("Connection attempt failed\n");
				break;
			case ID_NO_FREE_INCOMING_CONNECTIONS:
				printf("ID_NO_FREE_INCOMING_CONNECTIONS\n");
				break;
			case ID_UNCONNECTED_PONG:
				// Found the server
				pClient->Connect(p->systemAddress.ToString(false),p->systemAddress.GetPort(),0,0,0);
				break;
			case ID_CONNECTION_REQUEST_ACCEPTED:
				// This tells the client they have connected
				printf("ID_CONNECTION_REQUEST_ACCEPTED\n");
				Game::getSingletonPtr()->CreateCharacter("PlayerHost");
				//Game::getSingletonPtr()->SetPlayer(
				break;
			case ID_NEW_INCOMING_CONNECTION:
			{
				printf("New incoming connection");
				break;
			}
			case ID_RPC_REMOTE_ERROR:
				{
					// Recipient system returned an error
					switch (p->data[1])
					{
					case RakNet::RPC_ERROR_NETWORK_ID_MANAGER_UNAVAILABLE:
						printf("RPC_ERROR_NETWORK_ID_MANAGER_UNAVAILABLE\n");
						break;
					case RakNet::RPC_ERROR_OBJECT_DOES_NOT_EXIST:
						printf("RPC_ERROR_OBJECT_DOES_NOT_EXIST\n");
						break;
					case RakNet::RPC_ERROR_FUNCTION_INDEX_OUT_OF_RANGE:
						printf("RPC_ERROR_FUNCTION_INDEX_OUT_OF_RANGE\n");
						break;
					case RakNet::RPC_ERROR_FUNCTION_NOT_REGISTERED:
						printf("RPC_ERROR_FUNCTION_NOT_REGISTERED\n");
						break;
					case RakNet::RPC_ERROR_FUNCTION_NO_LONGER_REGISTERED:
						printf("RPC_ERROR_FUNCTION_NO_LONGER_REGISTERED\n");
						break;
					case RakNet::RPC_ERROR_CALLING_CPP_AS_C:
						printf("RPC_ERROR_CALLING_CPP_AS_C\n");
						break;
					case RakNet::RPC_ERROR_CALLING_C_AS_CPP:
						printf("RPC_ERROR_CALLING_C_AS_CPP\n");
						break;
					}
					printf("Function: %s", p->data+2);
				}
			}
		}
	}
	//When we are done as a client, close everything and delete RPC3
	Session::threading = false;
	pClient->Shutdown(20,0);
	RakNet::RakPeerInterface::DestroyInstance(pClient);
	delete rpc3Inst;
}

//This method sets up a hosted game
void Session::CreateHost(int port, std::string password){
	//the argumentList is build to have these parameters available in the static StartHost method
	ArgumentList* pArgumentList = new ArgumentList;
	pArgumentList->inter = rakPeer;
	pArgumentList->password = password;
	pArgumentList->port = port;
	pArgumentList->rpc3 = rpc3Inst;
	
	//start the thread where we constantly listen to the network
	threading = true;
	_beginthread(	StartHost,
					0, 
					pArgumentList);
	Session::sessionType = HOST;
}

//This static method is called when the user wants to host a server
void Session::StartHost(void* pVoid){
	//A parameter list is given with needed variables
	ArgumentList* pArgumentList = (ArgumentList*)pVoid;
	RakPeerInterface *pServer;
	//Filling my fields from argument list.
	pServer=pArgumentList->inter;
	std::string password = pArgumentList->password;
	int port = pArgumentList->port;
	RakNet::RPC3* rpc3Inst = pArgumentList->rpc3;
	//map that keeps track of who is who
	std::map<RakNet::SystemAddress, std::string> clientCharacterMap;

	//Register the functions we want to call RPC3 upon
	RPC3_REGISTER_FUNCTION(rpc3Inst, ReceiveInformation);
	RPC3_REGISTER_FUNCTION(rpc3Inst, UpdateCharacter);
	RPC3_REGISTER_FUNCTION(rpc3Inst, UpdateCharacterVelocity);
	RPC3_REGISTER_FUNCTION(rpc3Inst, SyncClients);
	RPC3_REGISTER_FUNCTION(rpc3Inst, ReceiveID);
	RPC3_REGISTER_FUNCTION(rpc3Inst, RemoveCharacter);
	RPC3_REGISTER_FUNCTION(rpc3Inst, AddScore);
	
	//Set up the host using RakNet
	pServer->SetIncomingPassword(password.c_str(), (int)strlen(password.c_str()));
	pServer->SetTimeoutTime(5000,UNASSIGNED_SYSTEM_ADDRESS);
	SystemAddress clientID=UNASSIGNED_SYSTEM_ADDRESS;
	RakNet::SocketDescriptor socketDescriptor(port,0);
	socketDescriptor.socketFamily=AF_INET;
	bool b = pServer->Startup(10,&socketDescriptor, 1 );
	pServer->SetMaximumIncomingConnections(10);

	pServer->SetOccasionalPing(true);
	pServer->SetUnreliableTimeout(1000);

	DataStructures::List<RakNetSmartPtr<RakNetSocket> > sockets;
	pServer->GetSockets(sockets);	
	
	// Holds packets
	Packet* p;
	// Record the first client that connects to us so we can pass it to the ping function
	clientID=UNASSIGNED_SYSTEM_ADDRESS;

	pServer->AttachPlugin(rpc3Inst);

	char message[2048];

	// Loop for input
	while (threading)
	{
		for (p=pServer->Receive(); p; pServer->DeallocatePacket(p), p=pServer->Receive())
		{
			switch (p->data[0])
			{
			case ID_DISCONNECTION_NOTIFICATION:
				{
				printf("ID_DISCONNECTION_NOTIFICATION\n");
				RakString name(clientCharacterMap[p->systemAddress].c_str());
				RemoveCharacter(name);
				rpc3Inst->CallC("RemoveCharacter", name);
				break;
				}
			case ID_ALREADY_CONNECTED:
				printf("ID_ALREADY_CONNECTED\n");
				break;
			case ID_CONNECTION_ATTEMPT_FAILED:
				printf("Connection attempt failed\n");
				break;
			case ID_NO_FREE_INCOMING_CONNECTIONS:
				printf("ID_NO_FREE_INCOMING_CONNECTIONS\n");
				break;
			case ID_UNCONNECTED_PONG:
				// Found the server
				pServer->Connect(p->systemAddress.ToString(false),p->systemAddress.GetPort(),0,0,0);
				break;
			case ID_CONNECTION_REQUEST_ACCEPTED:
				// This tells the client they have connected
				printf("ID_CONNECTION_REQUEST_ACCEPTED\n");
				//Session::SendInformation(RakNet::RakString("The italians are invading utrecht!"), rpc3Inst, 0);
				//Game::getSingletonPtr()->
				break;
			case ID_NEW_INCOMING_CONNECTION:
			{
				//When there is a new incoming connection we are change the local player to the host
				printf("New incoming connection\n");
				Game::getSingletonPtr()->getLocalPlayer()->SetNetworkName("PlayerHost");
				std::stringstream ss, ss2;
				ss << "Client" << (Game::getSingletonPtr()->GetCharacters().size()+1);
				//create a new character for the client
				Game::getSingletonPtr()->CreateCharacter(ss.str());
				clientCharacterMap[p->systemAddress] = ss.str();

				RPC3::CallExplicitParameters params;
				params.systemAddress = p->systemAddress;
				params.broadcast = false;
				//Make sure the client knows who he is
				rpc3Inst->CallExplicit("ReceiveID", &params, RakString::RakString(ss.str().c_str()));
				ss2 << Game::getSingletonPtr()->GetCharacters().size();
				//Synchronize the players with all clients; make sure they have all the characters on the screen
				RPC3::CallExplicitParameters params2;
				//params2.systemAddress = p->systemAddress;
				params2.broadcast = true;
				rpc3Inst->CallExplicit("SyncClients", &params2, RakNet::RakString(ss2.str().c_str()));
				//rpc3Inst->CallC("SyncClients", RakNet::RakString(ss2.str().c_str()));
				break;
			}				
			case ID_RPC_REMOTE_ERROR:
				{
					// Recipient system returned an error
					switch (p->data[1])
					{
					case RakNet::RPC_ERROR_NETWORK_ID_MANAGER_UNAVAILABLE:
						printf("RPC_ERROR_NETWORK_ID_MANAGER_UNAVAILABLE\n");
						break;
					case RakNet::RPC_ERROR_OBJECT_DOES_NOT_EXIST:
						printf("RPC_ERROR_OBJECT_DOES_NOT_EXIST\n");
						break;
					case RakNet::RPC_ERROR_FUNCTION_INDEX_OUT_OF_RANGE:
						printf("RPC_ERROR_FUNCTION_INDEX_OUT_OF_RANGE\n");
						break;
					case RakNet::RPC_ERROR_FUNCTION_NOT_REGISTERED:
						printf("RPC_ERROR_FUNCTION_NOT_REGISTERED\n");
						break;
					case RakNet::RPC_ERROR_FUNCTION_NO_LONGER_REGISTERED:
						printf("RPC_ERROR_FUNCTION_NO_LONGER_REGISTERED\n");
						break;
					case RakNet::RPC_ERROR_CALLING_CPP_AS_C:
						printf("RPC_ERROR_CALLING_CPP_AS_C\n");
						break;
					case RakNet::RPC_ERROR_CALLING_C_AS_CPP:
						printf("RPC_ERROR_CALLING_C_AS_CPP\n");
						break;
					}
					printf("Function: %s", p->data+2);
				}
			}
		}
	}
	pServer->Shutdown(30,0);
	RakNet::RakPeerInterface::DestroyInstance(pServer);
	delete rpc3Inst;
}

//This method synchronizes the local characters with all connected clients
void Session::SyncCharacters(RakNet::RPC3* rpcFromNetwork){
	std::vector<RenderableChar*> characters = Game::getSingletonPtr()->GetCharacters();
	//for each character on this machine, send an update to all the clients with the velocities of the character
	for (std::vector<RenderableChar*>::iterator i = characters.begin(); i != characters.end(); ++i) {
		RakString xVel, yVel, zVel;
		RakString xPos, yPos, zPos;
		RakString state;
		std::ostringstream buffxVel, buffyVel, buffzVel, buffxPos, buffyPos, buffzPos, buffState;
		buffxVel<<(*i)->getVelocity().x;
		xVel = buffxVel.str().c_str();
		buffyVel<<(*i)->getVelocity().y;
		yVel = buffyVel.str().c_str();
		buffzVel<<(*i)->getVelocity().z;
		zVel = buffzVel.str().c_str();

		buffxPos<<(*i)->getWorldPosition().x*10000;
		xPos = buffxPos.str().c_str();
		buffyPos<<(*i)->getWorldPosition().y*10000;
		yPos = buffyPos.str().c_str();
		buffzPos<<(*i)->getWorldPosition().z*10000;
		zPos = buffzPos.str().c_str();

		buffState<<(int)(*i)->getCharState();
		state = buffState.str().c_str();

		if(rpc3Inst->GetRakPeerInterface() != NULL){
			rpc3Inst->CallC("UpdateCharacter", RakString((*i)->GetNetworkName().c_str()), state, xVel, yVel, zVel, xPos, yPos, zPos, rpcFromNetwork);
			//std::cout << "x = " << x << " y = " << y << " z = " << z << std::endl;
		}
	}
}

//This method sends a string to the server/clients
void Session::SendInformation(RakNet::RakString msg, RakNet::RPC3* rpc3, RakNet::RPC3 *rpcFromNetwork){
	rpc3->CallC("ReceiveInformation", msg, rpcFromNetwork);
}

//This method sends the velocities of a single character to the server/clients
void Session::SendVelocity(Ogre::String name, int velX, int velY, int velZ){
	RenderableChar* character = Game::getSingletonPtr()->getLocalPlayer();
	RakString x, y, z;
	std::ostringstream buffx, buffy, buffz;
	buffx<<character->getVelocity().x;
	x = buffx.str().c_str();
	buffy<<character->getVelocity().y;
	y = buffy.str().c_str();
	buffz<<character->getVelocity().z;
	z = buffz.str().c_str();
	if(rpc3Inst->GetRakPeerInterface() != NULL){
		rpc3Inst->CallC("UpdateCharacterVelocity", RakString(name.c_str()), x, y, z, rpc3Inst);
	}
}

void Session::SendCharacterUpdate(RenderableChar* character){
	RakString xVel, yVel, zVel;
	RakString xPos, yPos, zPos;
	RakString state, dt;
	std::ostringstream buffxVel, buffyVel, buffzVel, buffxPos, buffyPos, buffzPos, buffState;
	buffxVel<<character->getVelocity().x;
	xVel = buffxVel.str().c_str();
	buffyVel<<character->getVelocity().y;
	yVel = buffyVel.str().c_str();
	buffzVel<<character->getVelocity().z;
	zVel = buffzVel.str().c_str();

	buffxPos<<character->getWorldPosition().x*10000;
	xPos = buffxPos.str().c_str();
	buffyPos<<character->getWorldPosition().y*10000;
	yPos = buffyPos.str().c_str();
	buffzPos<<character->getWorldPosition().z*10000;
	zPos = buffzPos.str().c_str();

	buffState<<(int)character->getCharState();
	state = buffState.str().c_str();

	if(rpc3Inst->GetRakPeerInterface() != NULL){
		rpc3Inst->CallC("UpdateCharacter", RakString(character->GetNetworkName().c_str()), state, xVel, yVel, zVel, xPos, yPos, zPos, rpc3Inst);
	}
}

Session::~Session(void)
{
	//rakPeer->Shutdown(100,0);
//	RakNet::RakPeerInterface::DestroyInstance(rakPeer);
	//delete rpc3Inst;
}

void Session::SendScore( ScoreEvent* evt )
{
	if(evt->getScoreType() == ScoreEvent::ADDSCORE && ScoreEvent::RESET_STREAK)
		rpc3Inst->CallC(RakString(evt->getMethodName().c_str()), RakString(evt->getPlayerName().c_str()), rpc3Inst);
	else if(evt->getScoreType() == ScoreEvent::SETSCORE)
		rpc3Inst->CallC(RakString(evt->getMethodName().c_str()), RakString(evt->getPlayerName().c_str()), RakString(boost::lexical_cast<std::string>(evt->getPoints()).c_str()), rpc3Inst);
}
