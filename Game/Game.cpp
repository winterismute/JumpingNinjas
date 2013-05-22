//|||||||||||||||||||||||||||||||||||||||||||||||

#include "Game.h"
#include "boost/lexical_cast.hpp"
#include "Level\LevelBlock.h"

#include <OgreLight.h>
#include <OgreWindowEventUtilities.h>
#include "../Network/Session.h"
#include "../Sound/Engine/SoundEngine.h"
#include "../Network/Event/NetworkEventManager.h"
#include "../Network/Event/ScoreEvent.h"
#include "Score/ScoreManager.h"
//|||||||||||||||||||||||||||||||||||||||||||||||

template<> Game* Ogre::Singleton<Game>::ms_Singleton = 0;
//template<> Game& Ogre::Singleton<Game>::getSingleton() =0;
//template<> Game* Ogre::Singleton<Game>::getSingletonPtr() =0;

Game& Game::getSingleton(){
	return *ms_Singleton;
};
Game* Game::getSingletonPtr(){
	return ms_Singleton;
};

Game::Game() : myLevel(NULL), myLevelFactory(NULL), cameraPosGame(Ogre::Vector3(0.0f, 0.0f, 0.0f)), cameraPosMap(Ogre::Vector3(0.0f, 0.0f, 0.0f))
{
	session = new Session(CLIENT);
	SetIsNetworkGame(false);
}

//|||||||||||||||||||||||||||||||||||||||||||||||

Game::~Game()
{
	if (session != NULL) session->threading = false;
	
	for (std::vector<RenderableChar*>::iterator it = local_players.begin(); it != local_players.end(); ++it) {
		delete (*it);
	}

	for (std::vector<RenderableChar*>::iterator it = characters_vector.begin(); it != characters_vector.end(); ++it) {
		delete (*it);
	}

	if (myLevel != NULL) {
		delete myLevel;
		myLevel = NULL;
	}
	if (myLevelFactory != NULL) {
		delete myLevelFactory;
		myLevelFactory = NULL;
	}
	if (session != NULL) {
		delete session;
		session = NULL;
	}
	if (GameCollisionHandler != NULL) {
		delete GameCollisionHandler;
		GameCollisionHandler = NULL;
	}
	delete OgreFW::getSingletonPtr();
	// THERE SHOULD BE WAY MORE THINGS
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void Game::startGame()
{
	//Init singletons
	new OgreFW();
	new SoundEngine();
	new ScoreManager();
	new NetworkEventManager();

	if(!OgreFW::getSingletonPtr()->initOgre("Lapins sautillant", this, 0))
	//if(!OgreFW::getSingletonPtr()->initOgre("Lapilles sautillant", NULL, NULL))
		return;

	// VERY IMPORTANT: THE EFFECTSFACTORY NEEDS
	new EffectsFactory();

	m_bShutdown = false;

	// Init random number generator
	srand( (unsigned int) time(NULL) + boost::interprocess::detail::get_current_process_id() );

	OgreFW::getSingletonPtr()->m_pLog->logMessage("Game initialized!");

	// Init the level Fectory
	//myLevelFactory = new RandomLevelFactory(OgreFW::getSingletonPtr()->m_pSceneMgr);
	myLevelFactory = new LevelFromFileFactory(OgreFW::getSingletonPtr()->m_pSceneMgr);

	// Set Ogre LOD pars
	Ogre::MaterialManager::getSingleton().setDefaultTextureFiltering(Ogre::TFO_TRILINEAR);
	Ogre::MaterialManager::getSingleton().setDefaultAnisotropy(1);

	NetworkEventManager::getSingletonPtr()->registerListener(this);
	// Proceed
	setupScene();
	runGame();
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void Game::setupScene()
{
	// Init stuff for render to texture - Minimap
	// First the texture
	rtt_texture_minimap = Ogre::TextureManager::getSingleton().createManual("RttTex",
			Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, Ogre::TEX_TYPE_2D,
			OgreFW::getSingletonPtr()->m_pRenderWnd->getWidth(),
			OgreFW::getSingletonPtr()->m_pRenderWnd->getHeight(),
			0, Ogre::PF_R8G8B8, Ogre::TU_RENDERTARGET);
	// Then, get its render target
	renderTextureTarget = rtt_texture_minimap->getBuffer()->getRenderTarget();
	renderTextureTarget->addViewport(OgreFW::getSingletonPtr()->m_pCamera);
	renderTextureTarget->getViewport(0)->setClearEveryFrame(true);
	renderTextureTarget->getViewport(0)->setBackgroundColour(Ogre::ColourValue::Black);
	renderTextureTarget->getViewport(0)->setOverlaysEnabled(false);
	// Set it active!
	renderTextureTarget->setActive(true);
	// Last, the mini rectangle
	mMiniScreen = new Ogre::Rectangle2D(true);
	mMiniScreen->setCorners(0.75f, -0.75f, 1.0f, -1.0f);
	mMiniScreen->setBoundingBox(Ogre::AxisAlignedBox(-100000.0f * Ogre::Vector3::UNIT_SCALE, 100000.0f * Ogre::Vector3::UNIT_SCALE));
	// Scenenode, then add
	mMiniScreenSN = OgreFW::getSingletonPtr()->m_pSceneMgr->getRootSceneNode()->createChildSceneNode("MiniScreenNode");
	mMiniScreenSN->attachObject(mMiniScreen);
	// The texture needs its material
	Ogre::MaterialPtr renderMaterial = Ogre::MaterialManager::getSingleton().create("RttMat", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	Ogre::Technique* matTechnique = renderMaterial->createTechnique();
	matTechnique->createPass();
	renderMaterial->getTechnique(0)->getPass(0)->setLightingEnabled(false);
	renderMaterial->getTechnique(0)->getPass(0)->createTextureUnitState("RttTex");
	mMiniScreen->setMaterial("RttMat");
	//Register this as a listener
	renderTextureTarget->addListener(this);

	// Init the CollisionHandler
	GameCollisionHandler = new CollisionHandler(OgreFW::getSingletonPtr()->m_pSceneMgr);

	// Init the first level
	myLevel = myLevelFactory->create();
	// Pass the LevelObjects to the collision_handler
	std::vector<LevelBlock*> levobjs = myLevel->getLevelObjects();
	for (std::vector<LevelBlock*>::iterator i = levobjs.begin(); i != levobjs.end(); ++i) {
		GameCollisionHandler->addNewObject(*i);
	}

	// The skybox!
	OgreFW::getSingletonPtr()->m_pSceneMgr->setSkyDome(true, "Examples/CloudySky");

	// Activate shadows
	OgreFW::getSingletonPtr()->m_pSceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_MODULATIVE);

	// Create Lights
	// Set the ambient light
	OgreFW::getSingletonPtr()->m_pSceneMgr->setAmbientLight(Ogre::ColourValue(0.15, 0.15, 0.15));

	// Create a Directional light
	Ogre::Light* lptr = OgreFW::getSingletonPtr()->m_pSceneMgr->createLight("dirLight");
	lptr->setType(Ogre::Light::LT_DIRECTIONAL);
	lptr->setDiffuseColour(Ogre::ColourValue(0.4, 0.4, 0));
	lptr->setSpecularColour(Ogre::ColourValue(0.4, 0.4, 0));
	lptr->setDirection(Ogre::Vector3( 0, -1, 0 ));
	sceneLights.push_back(lptr);

	// Create a Point Light
	Ogre::Light* lptrpoint = OgreFW::getSingletonPtr()->m_pSceneMgr->createLight("pointLight");
	lptrpoint->setType(Ogre::Light::LT_POINT);
	lptrpoint->setPosition(Ogre::Vector3(0, OBJ_SIZE_Y * myLevel->getHeight() * 2, 0));
	lptrpoint->setDiffuseColour(1.0, 0.0, 0.0);
    lptrpoint->setSpecularColour(1.0, 0.0, 0.0);
	sceneLights.push_back(lptrpoint);

	// Camera settings - TODO
	//OgreFW::getSingletonPtr()->moveCameraToPos(Ogre::Vector3( OBJ_SIZE_X, OBJ_SIZE_Y, OBJ_SIZE_Z));
	OgreFW::getSingletonPtr()->moveCameraToPos(Ogre::Vector3( myLevel->getWidth() * OBJ_SIZE_X / 2, OBJ_SIZE_Y * myLevel->getHeight() / 2, OBJ_SIZE_Z * 5));

	//OgreFW::getSingletonPtr()->setCameraLookAt(Ogre::Vector3(OBJ_SIZE_X * myLevel->getWidth() /2, OBJ_SIZE_Y, 0.0f));
	OgreFW::getSingletonPtr()->setCameraLookAt(Ogre::Vector3( myLevel->getWidth() * OBJ_SIZE_X / 2, OBJ_SIZE_Y * (myLevel->getHeight() -1) / 2, 0.0f));

	// Set the camerapos for rendering the minimap
	cameraPosMap = Ogre::Vector3(OBJ_SIZE_X * myLevel->getWidth() / 2, OBJ_SIZE_Y * myLevel->getHeight() / 2, OBJ_SIZE_Z * 10);

	// Request a sunEffect for creating the sun
	EffectsFactory::getSingletonPtr()->createSunEffect(myLevel->getWidth() * myLevel->getObjectSide() / 2 , myLevel->getHeight() * myLevel->getObjectSide() / 2 , -60.0f);

	// We move to the first game state
	currentGameStage = MAIN_MENU_STATE;

	// Load the GUI sheet
	OgreFW::getSingletonPtr()->mGUIRootWindow = CEGUI::WindowManager::getSingleton().loadWindowLayout("myL2.layout");
	//CEGUI::Window *guiRoot = CEGUI::WindowManager::getSingleton().getWindow(
	CEGUI::System::getSingleton().setGUISheet(OgreFW::getSingletonPtr()->mGUIRootWindow);
	CEGUI::Window* eventButton = OgreFW::getSingletonPtr()->mGUIRootWindow->getChild("MainMenuRoot/LocalGame");
	if (eventButton) {
		std::cout << "Yes I got it" << std::endl;
		// Register handler
		eventButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::SubscriberSlot(&Game::setupLocalGame, this));
		//std::cout << "Text: " << eventButton->getText() << std::endl;
	}
	eventButton = OgreFW::getSingletonPtr()->mGUIRootWindow->getChild("MainMenuRoot/NetClient");
	if (eventButton) {
		eventButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::SubscriberSlot(&Game::setupClientGame, this));
	}
	eventButton = OgreFW::getSingletonPtr()->mGUIRootWindow->getChild("MainMenuRoot/NetServer");
	if (eventButton) {
		eventButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::SubscriberSlot(&Game::setupServerGame, this));
	}

}

//|||||||||||||||||||||||||||||||||||||||||||||||

void Game::populatePlayers(int nplayers = 1)
{
	// This is a non-networked game
	for (int i=0; i < nplayers; i++) {
		std::string nm = "Player_";
		nm += boost::lexical_cast<std::string>(i+1);
		//RenderableChar* pl = new RenderableChar(Ogre::Vector3( (OBJ_SIZE_X * 2), OBJ_SIZE_Y +5, 0.0), Ogre::Vector3(0.02f), OgreFW::getSingletonPtr()->m_pSceneMgr, nm);
		RenderableChar* pl = new RenderableChar(Ogre::Vector3(OBJ_SIZE_X * (i+1), OBJ_SIZE_Y * 0.5 + 0.1, 0.0f), Ogre::Vector3(0.02f), OgreFW::getSingletonPtr()->m_pSceneMgr, nm);
		pl->setVisible(true);
		local_players.push_back(pl);
		// Add the player also to the collision Manager
		GameCollisionHandler->addNewObject(pl);
		if (i==0) {
			//OgreFW::getSingletonPtr()->m_pCamera->setAutoTracking(true, local_players[0]->getMainNode());
		}
		ScoreManager::getSingletonPtr()->addPlayer(pl);
	}
}

int Game::depopulatePlayers()
{
	OgreFW::getSingletonPtr()->m_pCamera->setAutoTracking(false);
	int l = local_players.size();
	for (int i=0; i < l; i++) {
		delete local_players[i];
		local_players[i] = NULL;
	}
	local_players.clear();
	GameCollisionHandler->clearAllMovingObjects();
	return l;
}

void Game::depopulateLevel()
{
	if (myLevel != NULL) {
		delete myLevel;
		myLevel = NULL;
	}
	GameCollisionHandler->clearAllStaticObjects();
}

void Game::loadSavedGame()
{
	GameState gs;
	// create and open an archive for input
	std::ifstream ifs("saved_game");
	boost::archive::text_iarchive ia(ifs);
	// read class state from archive
	ia >> gs;

	// We have a gamestate. Now, let's reset the content of our scene
	depopulateLevel();
	depopulatePlayers();

	// Allocate level
	if (myLevel == NULL) {
		// the above should hold of course
		myLevel = myLevelFactory->createLevelFromSavedGame(gs);
		// Unfortunately, we have to add all the objects to the collision manager one by one
		std::vector<LevelBlock*> objects = myLevel->getLevelObjects();
		for (std::vector<LevelBlock*>::iterator ite = objects.begin(); ite != objects.end(); ++ite) {
			GameCollisionHandler->addNewObject(*ite);
		}
	}
	
	// Allocate players
	int pn = gs.getPlayerStateNumber();
	populatePlayers(pn);
	// Set the properties of players
	for (int i=0; i < pn; i++) {
		PlayerState ps = gs.getPlayerState(i);
		local_players[i]->SetPosition(Ogre::Vector3(ps.getPosX(), ps.getPosY(), ps.getPosZ()));
		local_players[i]->SetName(ps.getName());
		// Set the scores
		//std::cout << "Size: " << scoreVector.size() << std::endl;
		ScoreManager::getSingletonPtr()->setScore(ps.getName(), gs.getScore(i));
	}

}

void Game::saveGame()
{
	std::ofstream ofs("saved_game");
	GameState gs;
	std::cout << "DONE 1" << std::endl;
	// Populate the game state
	// Add local players' states
	for (std::vector<RenderableChar*>::iterator ite = local_players.begin(); ite != local_players.end(); ++ite) {
		Ogre::Vector3 p = (*ite)->getWorldPosition();
		gs.addPlayerState(p.x, p.y, p.z, (*ite)->GetName());
		// Add score
		gs.addScore(ScoreManager::getSingletonPtr()->getScore((*ite)->GetName()));
	}
	// Add level blocks
	if (myLevel != NULL) {
		std::vector<LevelBlock*> myobjs = myLevel->getLevelObjects(); 
		for (std::vector<LevelBlock*>::iterator ite = myobjs.begin(); ite != myobjs.end(); ++ite) {
			Ogre::Vector3 p = (*ite)->getWorldPosition();
			gs.addLevelBlockState(p.x, p.y, p.z, (*ite)->getExtension());
		}
	}

	// save data to archive
    {
        boost::archive::text_oarchive oa(ofs);
        // write class instance to archive
        oa << gs;
    	// archive and stream closed when destructors are called
    }
	std::cout << "Game Saved!" << std::endl;
}

void Game::gotoNextLevel()
{
	// We reset our state
	depopulateLevel();
	int np = depopulatePlayers();

	// Then we get the next level
	if (myLevel == NULL) {
		myLevel = myLevelFactory->create();
		// Resync the collisionhandler
		std::vector<LevelBlock*> levobjs = myLevel->getLevelObjects();
		for (std::vector<LevelBlock*>::iterator i = levobjs.begin(); i != levobjs.end(); ++i) {
			GameCollisionHandler->addNewObject(*i);
		}
	}

	// And we put the players back
	populatePlayers(np);
}

void Game::CheckAndRespawnPlayers()
{
	// When A Player is dead, he is simply invisible
	for (std::vector<RenderableChar*>::iterator ite = local_players.begin(); ite != local_players.end(); ++ite) {
		if ( (*ite)->getCharState() == RenderableChar::SPAWNING ) {
			std::cout << "I should respawn!" << std::endl;
			(*ite)->resetVelX();
			(*ite)->resetVelY();
			(*ite)->setCharState(RenderableChar::IDLE);
			
			bool free = false;
			Ogre::RaySceneQuery* msq;
			float x;
			float y;
			int c = 0;
			
			// Here, we look for a position in the level which is collision free
			// TODO: IN NARROW LEVELS, THIS CAN LEAD TO INFINITE LOOPS!
			do {
				if (c > 20) {
					x = 10.0f;
					y = 10.0f;
					free = true;
				}
				x = Ogre::Math::RangeRandom(myLevel->getObjectSide(), myLevel->getObjectSide() * (myLevel->getWidth() -1));
				y = Ogre::Math::RangeRandom(myLevel->getObjectSide(), myLevel->getObjectSide() * (myLevel->getHeight()));	
				
				std::cout << "GENX: " << x << " --- GENY: " << y << std::endl;

				msq = OgreFW::getSingletonPtr()->m_pSceneMgr->createRayQuery(Ogre::Ray(Ogre::Vector3(x, y, 0.0f), Ogre::Vector3::UNIT_Y));
				msq->setSortByDistance(true, 3);
				//Ogre::RaySceneQuery	msq->setRay(Ogre::Ray( ( (*ite)->getWorldPosition() + Ogre::Vector3(0.0f, (*ite)->getAABB().getHalfSize().y, 0.0f) ), Ogre::Vector3::UNIT_Y ));
				Ogre::RaySceneQueryResult& result = msq->execute();
			
				for (Ogre::RaySceneQueryResult::iterator itr = result.begin(); itr != result.end(); ++itr) {
					if ( itr->distance >= (*ite)->getAABB().getSize().x ){
						free = true;
						break;
					}
				}
				c++;
			} while (free != true);
			(*ite)->SetPosition(Ogre::Vector3(x, y + 0.1, 0.0f));
			(*ite)->setVisible(true);
			msq->clearResults();
		}
	}
}

void Game::checkAndSyncScore()
{
	std::vector<RenderableChar*> mychars = GetCharacters();
	std::string ns;
	for (int i=0; i < mychars.size(); i++ ) {
		ns = boost::lexical_cast<std::string>(i+1);
		OgreFW::getSingletonPtr()->mGUIRootWindow->getChild("Root/P" + ns + "T")->setText(mychars[i]->GetName());
		OgreFW::getSingletonPtr()->mGUIRootWindow->getChild("Root/P" + ns + "S")->setText(boost::lexical_cast<std::string>(ScoreManager::getSingletonPtr()->getScore(mychars[i]->GetName())));
	}

	// Blank out the 
	for (int i = 4 - mychars.size(); i < 4; i++) {
		ns = boost::lexical_cast<std::string>(i+1);
		OgreFW::getSingletonPtr()->mGUIRootWindow->getChild("Root/P" + ns + "T")->setText("");
		OgreFW::getSingletonPtr()->mGUIRootWindow->getChild("Root/P" + ns + "S")->setText("");
	}

	/*
	for (int i=0; i < local_players.size(); i++) {
		CEGUI::Window* myw = NULL;
		//std::cout << "Player: " << i << "  has score: " << ScoreManager::getSingletonPtr()->getScore(local_players[i]->GetName()) << std::endl;
		myw	= OgreFW::getSingletonPtr()->mGUIRootWindow->getChild("ROOTWindow/" + local_players[i]->GetName() + "/Score");
		if (myw != NULL) {
			myw->setText(boost::lexical_cast<std::string>(ScoreManager::getSingletonPtr()->getScore(local_players[i]->GetName())));
			//std::cout << "Here I am" << std::endl;
		}
	}
	*/
}

bool Game::setupServerGame(const CEGUI::EventArgs& e)
{
	if (Session::threading == false) {
		Session::sessionType = HOST;
		session->CreateHost(50000, "");
		SetIsNetworkGame(true);
	}
	
	populatePlayers(1);
	currentGameStage = PLAYING_STATE;
	setupGUI();

	return true;
}
	

bool Game::setupClientGame(const CEGUI::EventArgs& e)
{
	if (Session::threading == false) {
		//session->Connect("127.0.0.1", 50000);
		session->Connect("255.255.255.255", 50000);
		SetIsNetworkGame(true);
	}
	
	populatePlayers(1);
	currentGameStage = PLAYING_STATE;
	setupGUI();

	return true;
}

bool Game::setupLocalGame(const CEGUI::EventArgs& e)
{
	// Let's parse the number of players
	CEGUI::String Msg = OgreFW::getSingletonPtr()->mGUIRootWindow->getChild("ConsoleRoot/EditBox")->getText();
	// Be careful with the content of the textfield!
	int nplayers = 2;
	if (Msg.size() == 1) {
		// Let's do a wonderful chained type cast!
		nplayers = boost::lexical_cast<int>(Msg.c_str());
		if (nplayers > 4) nplayers = 4;
		if (nplayers == 0) nplayers = 2;
	}

	std::cout << "Setup game IS CALLED! Players: " << nplayers << std::endl;
	populatePlayers(nplayers);

	currentGameStage = PLAYING_STATE;
	SetIsNetworkGame(false);
	setupGUI();

	return true;
}

void Game::setupGUI()
{	
	int nplayers = local_players.size();
	// Destory old window
	CEGUI::System::getSingleton().setGUISheet(NULL);
	//CEGUI::System::getGUISheet
	CEGUI::WindowManager::getSingleton().destroyWindow(OgreFW::getSingletonPtr()->mGUIRootWindow);

	// Load a new GUI sheet
	OgreFW::getSingletonPtr()->mGUIRootWindow = CEGUI::WindowManager::getSingleton().loadWindowLayout("mine.layout");
	//CEGUI::Window *guiRoot = CEGUI::WindowManager::getSingleton().getWindow(
	CEGUI::System::getSingleton().setGUISheet(OgreFW::getSingletonPtr()->mGUIRootWindow);

	std::string ns;
	// Reset the gui
	for (int i=0; i < 4; i++) {
		ns = boost::lexical_cast<std::string>(i+1);
		OgreFW::getSingletonPtr()->mGUIRootWindow->getChild("Root/P" + ns + "T")->setText("Palyer");
		OgreFW::getSingletonPtr()->mGUIRootWindow->getChild("Root/P" + ns + "S")->setText("0");
	}

	/*
	// Now, create a new one
	OgreFW::getSingletonPtr()->mGUIRootWindow = CEGUI::WindowManager::getSingleton().createWindow("DefaultWindow","ROOTWindow" );
	OgreFW::getSingletonPtr()->mGUIRootWindow->setPosition(CEGUI::UVector2(CEGUI::UDim(0,0),CEGUI::UDim(0,0)));
	OgreFW::getSingletonPtr()->mGUIRootWindow->setSize(CEGUI::UVector2(CEGUI::UDim(0.1 * nplayers, 0.0), CEGUI::UDim(0.1, 0.0)));
	//OgreFW::getSingletonPtr()->mGUIRootWindow->setProperty("Image","set:TaharezLook image:full_image");
	
	float xdim = 1.0 / nplayers;
	// Create Sub window entities
	for (int i=0; i < nplayers; i++) {
		CEGUI::Window* subw = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/StaticText", "ROOTWindow/" + local_players[i]->GetName() + "/Name");
		subw->setPosition(CEGUI::UVector2(CEGUI::UDim(xdim * i , 0), CEGUI::UDim(0, 0)));
		subw->setSize(CEGUI::UVector2(CEGUI::UDim(xdim * (i+1), 0), CEGUI::UDim(0.5, 0)));
		subw->setText(local_players[i]->GetName());
		OgreFW::getSingletonPtr()->mGUIRootWindow->addChildWindow(subw);
		subw = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/StaticText", "ROOTWindow/" + local_players[i]->GetName() + "/Score");
		//subw->setText(boost::lexical_cast<std::string>(0));
		subw->setPosition(CEGUI::UVector2(CEGUI::UDim(xdim * i, 0), CEGUI::UDim(0.5, 0)));
		subw->setSize(CEGUI::UVector2(CEGUI::UDim(xdim * (i+1), 0), CEGUI::UDim(1.0, 0)));
		subw->setText("Points");
		OgreFW::getSingletonPtr()->mGUIRootWindow->addChildWindow(subw);
	}
	CEGUI::System::getSingleton().setGUISheet(OgreFW::getSingletonPtr()->mGUIRootWindow);
	*/
}

void Game::runGame()
{
	OgreFW::getSingletonPtr()->m_pLog->logMessage("Start main loop...");
	
	double timeSinceLastFrame = 0;
	double startTime = 0;

	OgreFW::getSingletonPtr()->m_pRenderWnd->resetStatistics();
	

	while(!m_bShutdown && !OgreFW::getSingletonPtr()->isOgreToBeShutDown()) 
	{
		if(OgreFW::getSingletonPtr()->m_pRenderWnd->isClosed())m_bShutdown = true;

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
		Ogre::WindowEventUtilities::messagePump();
#endif	
		if(OgreFW::getSingletonPtr()->m_pRenderWnd->isActive())
		{
			startTime = OgreFW::getSingletonPtr()->m_pTimer->getMillisecondsCPU();
					
			OgreFW::getSingletonPtr()->m_pKeyboard->capture();
			OgreFW::getSingletonPtr()->m_pMouse->capture();

			OgreFW::getSingletonPtr()->updateOgre(timeSinceLastFrame);

			//mMiniScreen->setVisible(false);
			OgreFW::getSingletonPtr()->m_pRoot->renderOneFrame();
			//mMiniScreen->setVisible(false);

			timeSinceLastFrame = OgreFW::getSingletonPtr()->m_pTimer->getMillisecondsCPU() - startTime;
			update(timeSinceLastFrame * 0.001);
		}
		else
		{
			Sleep(1000);
		}
	}

	OgreFW::getSingletonPtr()->m_pLog->logMessage("Main loop quit");
	OgreFW::getSingletonPtr()->m_pLog->logMessage("Shutdown OGRE...");
}


void Game::update(float dt)
{	
	// Update mainly local players
	for (std::vector<RenderableChar*>::iterator i = local_players.begin(); i != local_players.end(); ++i) {
		(*i)->update(dt);
	}
	//Update other players
	for (std::vector<RenderableChar*>::iterator i = characters_vector.begin(); i != characters_vector.end(); ++i) {
		(*i)->updateAnimation(dt);
	}

	// Check if any player has fallen off the level
	for (std::vector<RenderableChar*>::iterator i = local_players.begin(); i != local_players.end(); ++i) {
		if ((*i)->getWorldPosition().y < -20.0f ) {
			(*i)->setVisible(false);
			(*i)->setCharState(RenderableChar::SPAWNING);
			// Add a point to all other players
			for (int j=0; j < local_players.size(); j++) {
				if (local_players[j] != (*i)) {
					ScoreManager::getSingletonPtr()->addPoint(local_players[j]);
				}
			}
			//std::cout << "Yes I am here" << std::endl;
		}
	}

	// After the object update, check the collision
	GameCollisionHandler->updateCollisions();

	// Update Scores
	//if (!GetIsNetworkGame()) checkAndSyncScore();
	if (getCurrentGameStage() != MAIN_MENU_STATE) checkAndSyncScore();

	// Check if we need to respawn any player?
	CheckAndRespawnPlayers();
	
	//if(m_movementKeysPressed)
	checkMovementKeys();

	//Update the camera, so it moves with the player
	//Ogre::Vector3 cameraPos = getLocalPlayer()->getWorldPosition();
	//cameraPos.z += 20;
	//cameraPos.y += 20;

	//OgreFW::getSingletonPtr()->m_pCamera->setPosition(cameraPos);

	if(Session::sessionType == HOST){
		session->SyncCharacters(0);
	}
	else if(Session::threading == true){
		for (std::vector<RenderableChar*>::iterator i = local_players.begin(); i != local_players.end(); ++i) {
			//session->SendVelocity((*i)->GetName(), (*i)->getVelocity().x, (*i)->getVelocity().y, (*i)->getVelocity().z);
			session->SendCharacterUpdate((*i));
		}
	}
}

//|||||||||||||||||||||||||||||||||||||||||||||||

bool Game::keyPressed(const OIS::KeyEvent &keyEventRef)
{
	if (currentGameStage == MAIN_MENU_STATE) {
		CEGUI::System &sys = CEGUI::System::getSingleton();
		sys.injectKeyDown(keyEventRef.key);
		sys.injectChar(keyEventRef.text);
		return true;
	}
	else {
		OgreFW::getSingletonPtr()->keyPressed(keyEventRef);
		switch (keyEventRef.key) {
			// HANDLING input of PLAYER 1 
			/*
			case OIS::KC_J:
				getLocalPlayer()->setVelX(-1);
				m_movementKeysPressed = true;
				break;
			case OIS::KC_L:
				getLocalPlayer()->setVelX(1);
				m_movementKeysPressed = true;
				break;
			*/
			case OIS::KC_I:
				getLocalPlayer()->performJump(1);
				//getLocalPlayer()->setVelY(1);
				break;
			case OIS::KC_T:
				if (local_players.size() > 1) getLocalPlayer(2)->performJump(1);
				break;
			case OIS::KC_W:
				if (local_players.size() > 2) getLocalPlayer(3)->performJump(1);
				break;
			case OIS::KC_X:
				saveGame();
				break;
			case OIS::KC_Z:
				loadSavedGame();
				break;
			case OIS::KC_1:
				gotoNextLevel();
				break;
		}
	}

	return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

bool Game::keyReleased(const OIS::KeyEvent &keyEventRef)
{
	if (currentGameStage == MAIN_MENU_STATE) {
		CEGUI::System::getSingleton().injectKeyUp(keyEventRef.key);
		return true;
	}
	else {
		OgreFW::getSingletonPtr()->keyReleased(keyEventRef);
	}

	// Reset velocity only if the character is actually moving in the corresponding direction 
	/*
	switch (keyEventRef.key) {
		// HANDLING input release of PLAYER 1
		case OIS::KC_J:
			if(!OgreFW::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_L))
				m_movementKeysPressed = false;
			break;
		case OIS::KC_L:
			if(!OgreFW::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_J))
				m_movementKeysPressed = false;
			break;
		//case OIS::KC_K:
		//	if (getLocalPlayer()->getVelocity().z > 0)
		//		getLocalPlayer()->resetVelZ();
		//	break;
		//case OIS::KC_I:
		//	if (getLocalPlayer()->getVelocity().z < 0)
		//		getLocalPlayer()->resetVelZ();
		//	break;
	}
	*/
	return true;
}

//This method creates and adds a character to the game.
void Game::CreateCharacter(std::string name){
	//RenderableChar* character = new RenderableChar(Ogre::Vector3(30.0f, OBJ_SIZE_Y * 2, 0.0f ), Ogre::Vector3(0.02), OgreFW::getSingletonPtr()->m_pSceneMgr, name);
	RenderableChar* character = new RenderableChar(Ogre::Vector3(OBJ_SIZE_X, OBJ_SIZE_Y * 0.5 + 0.1, 0.0f), Ogre::Vector3(0.02f), OgreFW::getSingletonPtr()->m_pSceneMgr, name);
	character->setVisible(true);
	characters_vector.push_back(character);
	//make sure the collision on this object is checked
	GameCollisionHandler->addNewObject(character);
	ScoreManager::getSingletonPtr()->addPlayer(character);
}

//This method returns all the characters from the game, both local and network
std::vector<RenderableChar*> Game::GetCharacters(void){
	std::vector<RenderableChar*> characters;
	for(int i = 0; i < characters_vector.size(); i++){
		characters.push_back(characters_vector[i]);
	}
	for(int j = 0; j < local_players.size(); j++){
		characters.push_back(local_players[j]);
	}
	return characters;
}

//This method sets the given character as the main character
void Game::SetPlayer(RenderableChar* character){
	RenderableChar* tempChar = Game::getSingletonPtr()->getLocalPlayer();
	tempChar = character;
}

void Game::RemoveCharacter( std::string name)
{
	for(int i = 0; i < characters_vector.size(); i++){
		if(characters_vector.at(i)->GetNetworkName() == name){
			GameCollisionHandler->removeObject(characters_vector[i]);
			//delete characters_vector[i];
			characters_vector[i]->setVisible(false);
			characters_vector.erase(characters_vector.begin()+i);
			ScoreManager::getSingletonPtr()->removePlayer(name);
		}
	}
	for(int i = 0; i < local_players.size(); i++){
		if(local_players.at(i)->GetNetworkName() == name){
			GameCollisionHandler->removeObject(local_players[i]);
			//delete local_players[i];
			local_players[i]->setVisible(false);
			local_players.erase(local_players.begin()+i);
			ScoreManager::getSingletonPtr()->removePlayer(name);
		}
	}
}

void Game::checkMovementKeys()
{
	if (OgreFW::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_J)) {
		getLocalPlayer()->setVelX(-1);
	}
	if (OgreFW::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_L)) {
		getLocalPlayer()->setVelX(1);
	}
	if (local_players.size() > 1) {
		if (OgreFW::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_F)) {
			getLocalPlayer(2)->setVelX(-1);
		}
		if (OgreFW::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_H)) {
			getLocalPlayer(2)->setVelX(1);
		}
	}
	if (local_players.size() > 2) {
		if (OgreFW::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_A)) {
			getLocalPlayer(3)->setVelX(-1);
		}
		if (OgreFW::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_D)) {
			getLocalPlayer(3)->setVelX(1);
		}
	}

}

void Game::onNetworkEvent( NetworkEvent* evt )
{
	if(Session::sessionType == HOST){
		if(evt->getType() == NetworkEvent::SCORE)
			session->SendScore(static_cast<ScoreEvent*>(evt));
	}
}

//|||||||||||||||||||||||||||||||||||||||||||||||