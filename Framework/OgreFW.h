//|||||||||||||||||||||||||||||||||||||||||||||||

#ifndef OGRE_FW_H
#define OGRE_FW_H

//|||||||||||||||||||||||||||||||||||||||||||||||

#include <OgreCamera.h>
#include <OgreEntity.h>
#include <OgreLogManager.h>
#include <OgreOverlay.h>
#include <OgreOverlayElement.h>
#include <OgreOverlayManager.h>
#include <OgreRoot.h>
#include <OgreViewport.h>
#include <OgreSceneManager.h>
#include <OgreRenderWindow.h>
#include <OgreConfigFile.h>

#include <OISEvents.h>
#include <OISInputManager.h>
#include <OISKeyboard.h>
#include <OISMouse.h>

#include <SdkTrays.h>

#include <CEGUI.h>
#include <RendererModules/Ogre/CEGUIOgreRenderer.h>

/*
	OgreFW class incapsulates many of the features of an Ogre application, included initialization.
	The idea is to keep all the ogre rendering oprations kind of hidden in the layer.
	The 
*/

//|||||||||||||||||||||||||||||||||||||||||||||||

class OgreFW : public Ogre::Singleton<OgreFW>, OIS::KeyListener, OIS::MouseListener
{
public:
	OgreFW();
	~OgreFW();

	Ogre::MeshPtr OgreFW::createMeshImmediate(Ogre::String meshname, Ogre::String matName, float px, float py, float pz, float xex, float yex, float zex);
	Ogre::MeshPtr OgreFW::createMesh(Ogre::String meshname, float px, float py, float pz, float xex, float yex, float zex);

	bool initOgre(Ogre::String wndTitle, OIS::KeyListener *pKeyListener = 0, OIS::MouseListener *pMouseListener = 0);
	void updateOgre(double timeSinceLastFrame);
	void updateCamera();
	void getInput();

	inline void OgreFW::moveCameraToPos(Ogre::Vector3 v3)
	{
		m_pCamera->setPosition(v3);
	}

	inline void OgreFW::setCameraLookAt(Ogre::Vector3 v3)
	{
		m_pCamera->lookAt(v3);
	}

	inline Ogre::Vector3 getCameraPos()
	{
		return m_pCamera->getPosition();
	}

	bool isOgreToBeShutDown()const{return m_bShutDownOgre;}  

	bool keyPressed(const OIS::KeyEvent &keyEventRef);
	bool keyReleased(const OIS::KeyEvent &keyEventRef);

	bool mouseMoved(const OIS::MouseEvent &evt);
	bool mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID id); 
	bool mouseReleased(const OIS::MouseEvent &evt, OIS::MouseButtonID id);
	
	Ogre::Root*					m_pRoot;
	Ogre::SceneManager*			m_pSceneMgr;
	Ogre::RenderWindow*			m_pRenderWnd;
	Ogre::Camera*				m_pCamera;
	Ogre::Viewport*				m_pViewport;
	Ogre::Log*					m_pLog;
	Ogre::Timer*				m_pTimer;
	
	OIS::InputManager*			m_pInputMgr;
	OIS::Keyboard*				m_pKeyboard;
	OIS::Mouse*					m_pMouse;

	/// This is for the GUI
    CEGUI::OgreRenderer* mGUIRenderer;
	CEGUI::Window* mGUIRootWindow;

private:

	CEGUI::MouseButton convertButton(OIS::MouseButtonID buttonID)
	{
		switch (buttonID)
		{
			case OIS::MB_Left:
				return CEGUI::LeftButton;
			case OIS::MB_Right:
				return CEGUI::RightButton;
			case OIS::MB_Middle:
				return CEGUI::MiddleButton;
			default:
				return CEGUI::LeftButton;
		}
	}

	OgreFW(const OgreFW&);
	OgreFW& operator= (const OgreFW&);
	
	// Plugin configuration
	Ogre::String m_PluginsCfg;

	//OgreBites::SdkTrayManager*	m_pTrayMgr;
    Ogre::FrameEvent            m_FrameEvent;
	int							m_iNumScreenShots;

	bool						m_bShutDownOgre;
	
	Ogre::Vector3				m_TranslateVector;
	Ogre::Real					m_MoveSpeed; 
	Ogre::Degree				m_RotateSpeed; 
	float						m_MoveScale; 
	Ogre::Degree				m_RotScale;
};

//|||||||||||||||||||||||||||||||||||||||||||||||

#endif 

//|||||||||||||||||||||||||||||||||||||||||||||||