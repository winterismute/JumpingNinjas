//|||||||||||||||||||||||||||||||||||||||||||||||

#ifndef GAME_H
#define GAME_H

//|||||||||||||||||||||||||||||||||||||||||||||||

#include <OgreSingleton.h>
#include <OgreMath.h>

#include "../Framework/OgreFW.h"

#include "Level/Level.h"
#include "Level/LevelFactory.h"
#include "Level/RandomLevelFactory.h"
#include "Level/LevelFromFileFactory.h"

#include "Entity/Player/RenderableChar.h"
#include "Entity/CollisionHandler.h"
#include "../Network/Event/NetworkEventListener.h"
#include "../Network/Event/NetworkEventManager.h"

#include "GameState.h"

//#include <OgreParticle.h>

#include <string>
#include <cstdlib>
#include <ctime>
#include "boost/interprocess/detail/os_thread_functions.hpp"

class Session;

#include "EffectsFactory.h"

#define OBJ_SIZE_X 10.0f
#define OBJ_SIZE_Y 10.0f
#define OBJ_SIZE_Z 10.0f

//|||||||||||||||||||||||||||||||||||||||||||||||

class Game : public OIS::KeyListener, public Ogre::RenderTargetListener, Ogre::Singleton<Game>, NetworkEventListener

{
public:

	enum game_stage {MAIN_MENU_STATE, PLAYING_STATE};

	Game();
	~Game();

	virtual void startGame();
	
	void update(float dt);

	bool keyPressed(const OIS::KeyEvent &keyEventRef);
	bool keyReleased(const OIS::KeyEvent &keyEventRef);

	static Game& getSingleton(void);
	static Game* getSingletonPtr(void);

	void SetPlayer(RenderableChar* character);
	void CreateCharacter(std::string);
	void RemoveCharacter(std::string);
	std::vector<RenderableChar*> GetCharacters(void);
	
	inline Session* GetSession() 
	{
		return session;
	}

	inline RenderableChar* getLocalPlayer(int num = 1)
	{
		if ( (num - 1) >= 0 &&  (num - 1) < local_players.size()) {
			return local_players[num - 1];
		}
		else return NULL;
	}

	inline game_stage getCurrentGameStage()
	{
		return currentGameStage;
	}

	inline bool GetIsNetworkGame(){
		return isNetworkGame;
	}

	inline void SetIsNetworkGame(bool val){
		isNetworkGame = val;
	}
	//Template method for deleting vector of elements
	template <class C> void FreeClear( C & cntr ) {
		for ( typename C::iterator it = cntr.begin(); 
			it != cntr.end(); ++it ) {
				delete * it;
		}
		cntr.clear();
	}

protected:
	std::vector<RenderableChar*> local_players;

	game_stage currentGameStage;

	virtual int depopulatePlayers();
	virtual void depopulateLevel();

	virtual void loadSavedGame();
	virtual void saveGame();
	virtual void gotoNextLevel();

	virtual void CheckAndRespawnPlayers();
	virtual void checkAndSyncScore();

	virtual void populatePlayers(int nplayers);
	void setupScene();

	bool setupLocalGame(const CEGUI::EventArgs& e);
	bool setupServerGame(const CEGUI::EventArgs& e);
	bool setupClientGame(const CEGUI::EventArgs& e);
	void setupGUI();

	void runGame();
	void onNetworkEvent(NetworkEvent* evt);

	inline virtual void preRenderTargetUpdate(const Ogre::RenderTargetEvent& evt)
	{
		mMiniScreen->setVisible(false);
		cameraPosGame = OgreFW::getSingletonPtr()->getCameraPos();
		OgreFW::getSingletonPtr()->moveCameraToPos(cameraPosMap);
	}

	inline virtual void postRenderTargetUpdate(const Ogre::RenderTargetEvent& evt)
	{
		mMiniScreen->setVisible(true);
		OgreFW::getSingletonPtr()->moveCameraToPos(cameraPosGame);
	}

	// Game CollisionManager
	CollisionHandler* GameCollisionHandler;

	std::vector<RenderableChar*> characters_vector;

	// For minimap
	Ogre::TexturePtr rtt_texture_minimap;
	Ogre::RenderTexture* renderTextureTarget;
	Ogre::Rectangle2D* mMiniScreen;
	Ogre::SceneNode* mMiniScreenSN;

	Session* session;
	// Needed for level generation
	Level* myLevel;
	LevelFactory* myLevelFactory;

	Ogre::Vector3 cameraPosGame;
	Ogre::Vector3 cameraPosMap;

	std::vector<Ogre::SceneNode*> levelBlocks;
	std::vector<Ogre::Light*> sceneLights;

	bool						m_bShutdown;
	bool isNetworkGame;

private:
	void checkMovementKeys();
	bool                        m_movementKeysPressed;
};

//|||||||||||||||||||||||||||||||||||||||||||||||

#endif 

//|||||||||||||||||||||||||||||||||||||||||||||||