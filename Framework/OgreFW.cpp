//|||||||||||||||||||||||||||||||||||||||||||||||

#include "OgreFW.h"

//|||||||||||||||||||||||||||||||||||||||||||||||

using namespace Ogre;

//|||||||||||||||||||||||||||||||||||||||||||||||

template<> OgreFW* Ogre::Singleton<OgreFW>::ms_Singleton = 0;

//|||||||||||||||||||||||||||||||||||||||||||||||

OgreFW::OgreFW()
{
    m_MoveSpeed			= 0.1f;
    m_RotateSpeed		= 0.3f;

    m_bShutDownOgre		= false;
    m_iNumScreenShots	= 0;

    m_pRoot				= 0;
    m_pSceneMgr			= 0;
    m_pRenderWnd		= 0;
    m_pCamera			= 0;
    m_pViewport			= 0;
    m_pLog				= 0;
    m_pTimer			= 0;

    m_pInputMgr			= 0;
    m_pKeyboard			= 0;
    m_pMouse			= 0;

	mGUIRootWindow = NULL;

    //m_pTrayMgr          = 0;
    m_FrameEvent        = Ogre::FrameEvent();

	m_PluginsCfg = Ogre::StringUtil::BLANK;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

bool OgreFW::initOgre(Ogre::String wndTitle, OIS::KeyListener *pKeyListener, OIS::MouseListener *pMouseListener)
{
    Ogre::LogManager* logMgr = new Ogre::LogManager();

    m_pLog = Ogre::LogManager::getSingleton().createLog("OgreLogfile.log", true, true, false);
    m_pLog->setDebugOutputEnabled(true);

	// Deal with the plugin configuration string name

#ifdef _DEBUG
    m_PluginsCfg = "plugins_d.cfg";
#else
    m_PluginsCfg = "plugins.cfg";
#endif
	
	m_pRoot = new Ogre::Root(m_PluginsCfg);

	// Deal with resources loading
	Ogre::String secName, typeName, archName;
    Ogre::ConfigFile cf;

#ifdef _DEBUG
	cf.load("resources_d.cfg");
#else
	cf.load("resources.cfg");
#endif

    Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();
    while (seci.hasMoreElements())
    {
        secName = seci.peekNextKey();
        Ogre::ConfigFile::SettingsMultiMap *settings = seci.getNext();
        Ogre::ConfigFile::SettingsMultiMap::iterator i;
        for (i = settings->begin(); i != settings->end(); ++i)
        {
            typeName = i->first;
            archName = i->second;
            Ogre::ResourceGroupManager::getSingleton().addResourceLocation(archName, typeName, secName);
        }
    }
	
	// configure
	// Show the configuration dialog and initialise the system
	if(!(m_pRoot->restoreConfig() || m_pRoot->showConfigDialog()))
	{
		return false;
	}

    m_pRoot->initialise(false);
	m_pRenderWnd = m_pRoot->createRenderWindow(wndTitle, 800, 600, false);

    m_pSceneMgr = m_pRoot->createSceneManager(ST_GENERIC, "SceneManager");
    m_pSceneMgr->setAmbientLight(Ogre::ColourValue(0.7f, 0.7f, 0.7f));

    m_pCamera = m_pSceneMgr->createCamera("Camera");
    m_pCamera->setPosition(Vector3(0, 0, 60));
    m_pCamera->lookAt(Vector3(0, 0, 0));
    m_pCamera->setNearClipDistance(1);

	m_pCamera->setFarClipDistance(50000);
    if (m_pRoot->getRenderSystem()->getCapabilities()->hasCapability(Ogre::RSC_INFINITE_FAR_PLANE))
    {
        m_pCamera->setFarClipDistance(0);   // enable infinite far clip distance if we can
    }

    m_pViewport = m_pRenderWnd->addViewport(m_pCamera);
    m_pViewport->setBackgroundColour(ColourValue(0.8f, 0.7f, 0.6f, 1.0f));

    m_pCamera->setAspectRatio(Real(m_pViewport->getActualWidth()) / Real(m_pViewport->getActualHeight()));

    m_pViewport->setCamera(m_pCamera);

    unsigned long hWnd = 0;
    OIS::ParamList paramList;
    m_pRenderWnd->getCustomAttribute("WINDOW", &hWnd);

    paramList.insert(OIS::ParamList::value_type("WINDOW", Ogre::StringConverter::toString(hWnd)));

    m_pInputMgr = OIS::InputManager::createInputSystem(paramList);

    m_pKeyboard = static_cast<OIS::Keyboard*>(m_pInputMgr->createInputObject(OIS::OISKeyboard, true));
    m_pMouse = static_cast<OIS::Mouse*>(m_pInputMgr->createInputObject(OIS::OISMouse, true));

    m_pMouse->getMouseState().height = m_pRenderWnd->getHeight();
    m_pMouse->getMouseState().width	 = m_pRenderWnd->getWidth();

    if(pKeyListener == 0)
        m_pKeyboard->setEventCallback(this);
    else
        m_pKeyboard->setEventCallback(pKeyListener);

    if(pMouseListener == 0)
        m_pMouse->setEventCallback(this);
    else
        m_pMouse->setEventCallback(pMouseListener);

    Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);
    Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

    m_pTimer = new Ogre::Timer();
    m_pTimer->reset();

	/*
    m_pTrayMgr = new OgreBites::SdkTrayManager("TrayMgr", m_pRenderWnd, m_pMouse, this);
    m_pTrayMgr->showFrameStats(OgreBites::TL_BOTTOMLEFT);
    m_pTrayMgr->showLogo(OgreBites::TL_BOTTOMRIGHT);
    m_pTrayMgr->hideCursor();
	*/

    m_pRenderWnd->setActive(true);

	// Think it's time to setup the GUI
	mGUIRenderer = &CEGUI::OgreRenderer::bootstrapSystem((*m_pRenderWnd));

	CEGUI::Imageset::setDefaultResourceGroup("Imagesets");
	CEGUI::Font::setDefaultResourceGroup("Fonts");
	CEGUI::Scheme::setDefaultResourceGroup("Schemes");
	CEGUI::WidgetLookManager::setDefaultResourceGroup("LookNFeel");
	CEGUI::WindowManager::setDefaultResourceGroup("Layouts");

	CEGUI::SchemeManager::getSingleton().create("TaharezLook.scheme");

	CEGUI::System::getSingleton().setDefaultMouseCursor("TaharezLook", "MouseArrow");

	mGUIRootWindow = CEGUI::WindowManager::getSingleton().createWindow( "DefaultWindow", "_MasterRoot" );
	CEGUI::System::getSingleton().setGUISheet(mGUIRootWindow);

	return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

OgreFW::~OgreFW()
{
    if(m_pInputMgr)
        OIS::InputManager::destroyInputSystem(m_pInputMgr);

    delete m_pRoot;
}

//|||||||||||||||||||||||||||||||||||||||||||||||
Ogre::MeshPtr OgreFW::createMesh(Ogre::String meshname, float px, float py, float pz, float xex, float yex, float zex)
//Ogre::MeshPtr OgreFW::createMesh2(Ogre::String meshname)
{
    /// Create the mesh via the MeshManager
    Ogre::MeshPtr msh = MeshManager::getSingleton().createManual(meshname, "General");
 
    /// Create one submesh
    SubMesh* sub = msh->createSubMesh();
 
    const float sqrt13 = 0.577350269f; /* sqrt(1/3) */
 
    /// Define the vertices (8 vertices, each consisting of 2 groups of 3 floats
    const size_t nVertices = 8;
    const size_t vbufCount = 3*2*nVertices;
    float vertices[vbufCount] = {
            px, py + yex, pz,        //0 position
            -sqrt13, sqrt13, -sqrt13,     //0 normal
            px + xex, py + yex, pz,         //1 position
            sqrt13, sqrt13, -sqrt13,      //1 normal
            px + xex, py, pz,        //2 position
            sqrt13, -sqrt13, -sqrt13,     //2 normal
            px, py, pz,       //3 position
            -sqrt13, -sqrt13, -sqrt13,    //3 normal
            px, py + yex, pz + zex,         //4 position
            -sqrt13, sqrt13, sqrt13,      //4 normal
            px + xex, py + yex, pz + zex,          //5 position
            sqrt13, sqrt13, sqrt13,       //5 normal
            px + xex, py, pz + zex,         //6 position
            sqrt13, -sqrt13, sqrt13,      //6 normal
            px, py, pz + zex,        //7 position
            -sqrt13, -sqrt13, sqrt13,     //7 normal
    };

	//float uv[]
 
    //RenderSystem* rs = Root::getSingleton().getRenderSystem();
    //RGBA colours[nVertices];
    //RGBA *pColour = colours;
    // Use render system to convert colour value since colour packing varies
    //rs->convertColourValue(ColourValue(1.0,0.0,0.0), pColour++); //0 colour
    //rs->convertColourValue(ColourValue(1.0,1.0,0.0), pColour++); //1 colour
    //rs->convertColourValue(ColourValue(0.0,1.0,0.0), pColour++); //2 colour
    //rs->convertColourValue(ColourValue(0.0,0.0,0.0), pColour++); //3 colour
    //rs->convertColourValue(ColourValue(1.0,0.0,1.0), pColour++); //4 colour
    //rs->convertColourValue(ColourValue(1.0,1.0,1.0), pColour++); //5 colour
    //rs->convertColourValue(ColourValue(0.0,1.0,1.0), pColour++); //6 colour
    //rs->convertColourValue(ColourValue(0.0,0.0,1.0), pColour++); //7 colour
 
    /// Define 12 triangles (two triangles per cube face)
    /// The values in this table refer to vertices in the above table
    const size_t ibufCount = 36;
    unsigned short faces[ibufCount] = {
            0,2,3,
            0,1,2,
            1,6,2,
            1,5,6,
            4,6,5,
            4,7,6,
            0,7,4,
            0,3,7,
            0,5,1,
            0,4,5,
            2,7,3,
            2,6,7
    };
 
    /// Create vertex data structure for 8 vertices shared between submeshes
    msh->sharedVertexData = new VertexData();
    msh->sharedVertexData->vertexCount = nVertices;
 
    /// Create declaration (memory format) of vertex data
    VertexDeclaration* decl = msh->sharedVertexData->vertexDeclaration;
    size_t offset = 0;
    // 1st buffer
    decl->addElement(0, offset, VET_FLOAT3, VES_POSITION);
    offset += VertexElement::getTypeSize(VET_FLOAT3);
    decl->addElement(0, offset, VET_FLOAT3, VES_NORMAL);
    offset += VertexElement::getTypeSize(VET_FLOAT3);
    /// Allocate vertex buffer of the requested number of vertices (vertexCount) 
    /// and bytes per vertex (offset)
    HardwareVertexBufferSharedPtr vbuf = 
        HardwareBufferManager::getSingleton().createVertexBuffer(
        offset, msh->sharedVertexData->vertexCount, HardwareBuffer::HBU_STATIC_WRITE_ONLY);
    /// Upload the vertex data to the card
    vbuf->writeData(0, vbuf->getSizeInBytes(), vertices, true);
 
    /// Set vertex buffer binding so buffer 0 is bound to our vertex buffer
    VertexBufferBinding* bind = msh->sharedVertexData->vertexBufferBinding; 
    bind->setBinding(0, vbuf);
 
	//2nd buffer for us is UV coordinates
	//offset = 0;
	//decl->addElement(1, offset, VET_FLOAT2, VES_TEXTURE_COORDINATES);
	//offset += VertexElement::getTypeSize(VET_FLOAT2);
	//vbuf = HardwareBufferManager::getSingleton().createVertexBuffer(offset, msh->)

    // 3rd buffer
    //offset = 0;
    //decl->addElement(1, offset, VET_COLOUR, VES_DIFFUSE);
    //offset += VertexElement::getTypeSize(VET_COLOUR);
    /// Allocate vertex buffer of the requested number of vertices (vertexCount) 
    /// and bytes per vertex (offset)
    //vbuf = HardwareBufferManager::getSingleton().createVertexBuffer(
    //    offset, msh->sharedVertexData->vertexCount, HardwareBuffer::HBU_STATIC_WRITE_ONLY);
    /// Upload the vertex data to the card
    //vbuf->writeData(0, vbuf->getSizeInBytes(), colours, true);
    /// Set vertex buffer binding so buffer 1 is bound to our colour buffer
    //bind->setBinding(1, vbuf);
 
    /// Allocate index buffer of the requested number of vertices (ibufCount) 
    HardwareIndexBufferSharedPtr ibuf = HardwareBufferManager::getSingleton().
        createIndexBuffer(
        HardwareIndexBuffer::IT_16BIT, 
        ibufCount, 
        HardwareBuffer::HBU_STATIC_WRITE_ONLY);
 
    /// Upload the index data to the card
    ibuf->writeData(0, ibuf->getSizeInBytes(), faces, true);
 
    /// Set parameters of the submesh
    sub->useSharedVertices = true;
    sub->indexData->indexBuffer = ibuf;
    sub->indexData->indexCount = ibufCount;
    sub->indexData->indexStart = 0;
 
    /// Set bounding information (for culling)
	msh->_setBounds(AxisAlignedBox(px, py, pz, px + xex, py + yex, pz + zex));
    //msh->_setBoundingSphereRadius(Math::Sqrt(3*10*10));
	float max = px + xex; 
	if (max < py + yex) max = py + yex;
	if (max < pz + zex) max = pz + zex;
	msh->_setBoundingSphereRadius(Math::Sqrt(3 * max * max));
 
    /// Notify -Mesh object that it has been loaded
    msh->load();

	return msh;
}


Ogre::MeshPtr OgreFW::createMeshImmediate(Ogre::String meshname, Ogre::String matName, float px, float py, float pz, float xex, float yex, float zex)
{
	if (m_pSceneMgr != NULL) {
		Ogre::ManualObject* lManualObject = m_pSceneMgr->createManualObject(Ogre::String("ManObj"));

		lManualObject->setDynamic(false);

		//lManualObject->begin("BaseWhiteNoLighting", Ogre::RenderOperation::OT_TRIANGLE_LIST);
		lManualObject->begin(matName, Ogre::RenderOperation::OT_TRIANGLE_LIST);
		{
			lManualObject->position(px, py + yex, pz);// a vertex
			lManualObject->position(px + xex, py + yex, pz);// a vertex
			lManualObject->position(px + xex, py, pz);// a vertex
			lManualObject->position(px, py, pz);// a vertex
			
			lManualObject->position(px, py + yex, pz + zex);// a vertex
			lManualObject->position(px + xex, py + yex, pz + zex);// a vertex
			lManualObject->position(px + xex, py, pz + zex);// a vertex
			lManualObject->position(px, py, pz + zex);// a vertex
 
			// face behind / front
			lManualObject->triangle(0,1,2);
			lManualObject->triangle(2,3,0);
			lManualObject->triangle(4,6,5);
			lManualObject->triangle(6,4,7);
 
			// face top / down
			lManualObject->triangle(0,4,5);
			lManualObject->triangle(5,1,0);
			lManualObject->triangle(2,6,7);
			lManualObject->triangle(7,3,2);
 
			// face left / right
			lManualObject->triangle(0,7,4);
			lManualObject->triangle(7,0,3);
			lManualObject->triangle(1,5,6);
			lManualObject->triangle(6,2,1);	

		}
		lManualObject->end();

		Ogre::String lResourceGroup = Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME;
		//delete lManualObject;
		Ogre::MeshPtr mp = lManualObject->convertToMesh(meshname);
		//OGRE_DELETE lManualObject;
		return mp;
	}
}


bool OgreFW::keyPressed(const OIS::KeyEvent &keyEventRef)
{
    if(m_pKeyboard->isKeyDown(OIS::KC_ESCAPE))
    {
        m_bShutDownOgre = true;
        return true;
    }

    if(m_pKeyboard->isKeyDown(OIS::KC_SYSRQ))
    {
        m_pRenderWnd->writeContentsToTimestampedFile("BOF_Screenshot_", ".jpg");
        return true;
    }

    if(m_pKeyboard->isKeyDown(OIS::KC_M))
    {
        static int mode = 0;

        if(mode == 2)
        {
            m_pCamera->setPolygonMode(PM_SOLID);
            mode = 0;
        }
        else if(mode == 0)
        {
            m_pCamera->setPolygonMode(PM_WIREFRAME);
            mode = 1;
        }
        else if(mode == 1)
        {
            m_pCamera->setPolygonMode(PM_POINTS);
            mode = 2;
        }
    }

    if(m_pKeyboard->isKeyDown(OIS::KC_O))
    {
		/*
        if(m_pTrayMgr->isLogoVisible())
        {
            m_pTrayMgr->hideLogo();
            m_pTrayMgr->hideFrameStats();
        }
        else
        {
            m_pTrayMgr->showLogo(OgreBites::TL_BOTTOMRIGHT);
            m_pTrayMgr->showFrameStats(OgreBites::TL_BOTTOMLEFT);
        }
		*/
    }

    return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

bool OgreFW::keyReleased(const OIS::KeyEvent &keyEventRef)
{
    return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

bool OgreFW::mouseMoved(const OIS::MouseEvent &evt)
{
	CEGUI::System &sys = CEGUI::System::getSingleton();
	sys.injectMouseMove(evt.state.X.rel, evt.state.Y.rel);
	// Scroll wheel.
	if (evt.state.Z.rel) sys.injectMouseWheelChange(evt.state.Z.rel / 120.0f);


    m_pCamera->yaw(Degree(evt.state.X.rel * -0.1f));
    m_pCamera->pitch(Degree(evt.state.Y.rel * -0.1f));

    return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

bool OgreFW::mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
{
	CEGUI::System::getSingleton().injectMouseButtonDown(convertButton(id));
    return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

bool OgreFW::mouseReleased(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
{
	CEGUI::System::getSingleton().injectMouseButtonUp(convertButton(id));
    return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void OgreFW::updateOgre(double timeSinceLastFrame)
{
    m_MoveScale = m_MoveSpeed   * (float)timeSinceLastFrame;
    m_RotScale  = m_RotateSpeed * (float)timeSinceLastFrame;

    m_TranslateVector = Vector3::ZERO;

    getInput();
    updateCamera();

    m_FrameEvent.timeSinceLastFrame = timeSinceLastFrame;
    //m_pTrayMgr->frameRenderingQueued(m_FrameEvent);
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void OgreFW::updateCamera()
{
    if(m_pKeyboard->isKeyDown(OIS::KC_LSHIFT)) 
        m_pCamera->moveRelative(m_TranslateVector);
    else
        m_pCamera->moveRelative(m_TranslateVector / 10);
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void OgreFW::getInput()
{
	/*
    if(m_pKeyboard->isKeyDown(OIS::KC_A))
        m_TranslateVector.x = -m_MoveScale;

    if(m_pKeyboard->isKeyDown(OIS::KC_D))
        m_TranslateVector.x = m_MoveScale;

    if(m_pKeyboard->isKeyDown(OIS::KC_W))
        m_TranslateVector.z = -m_MoveScale;

    if(m_pKeyboard->isKeyDown(OIS::KC_S))
        m_TranslateVector.z = m_MoveScale;
	*/
}

//|||||||||||||||||||||||||||||||||||||||||||||||