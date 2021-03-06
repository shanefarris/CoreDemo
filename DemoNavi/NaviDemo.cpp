#include "NaviDemo.h"
#include "Navi/NaviUtilities.h"

using namespace Ogre;
using namespace NaviLibrary;
using namespace NaviLibrary::NaviUtilities;

NaviDemo::NaviDemo()
{
	menubar, status, chat, equip = 0;
	shouldQuit = false;
	renderWin = 0;
	sceneMgr = 0;
	viewport = 0;
	google = 0;

	Root *root = new Root();

	shouldQuit = !root->showConfigDialog();
	if(shouldQuit) return;

	renderWin = root->initialise(true, "NaviDemo");
	sceneMgr = root->createSceneManager("TerrainSceneManager");
	WindowEventUtilities::addWindowEventListener(renderWin, this);

	createScene();

	setupNavis();

	loadInputSystem();
}

NaviDemo::~NaviDemo()
{
	delete NaviManager::GetPointer();
	Root::getSingleton().shutdown();
}

void NaviDemo::createScene()
{
	sceneMgr->setAmbientLight(ColourValue::White);
	sceneMgr->setShadowTechnique(SHADOWTYPE_TEXTURE_ADDITIVE);

	Camera* camera = sceneMgr->createCamera("MainCam");
	viewport = renderWin->addViewport(camera);
	viewport->setBackgroundColour(ColourValue(0.17, 0.14, 0.1));

	camera->setAspectRatio(viewport->getActualWidth() / (Real)viewport->getActualHeight());

	parseResources();
}

void NaviDemo::setupNavis()
{
	// Create the NaviManager make our Navis
	naviMgr = new NaviManager(viewport, ".\\Media");

	menubar = naviMgr->createNavi("menubar", 1024, 128, NaviPosition(BottomCenter));
	menubar->loadFile("menubar.html");
	menubar->bind("hpChange", NaviDelegate(this, &NaviDemo::hpChange));
	menubar->bind("turnOff", NaviDelegate(this, &NaviDemo::turnOff));
	menubar->bind("turnOn", NaviDelegate(this, &NaviDemo::turnOn));
	menubar->bind("openGoogle", NaviDelegate(this, &NaviDemo::requestOpenGoogle));
	//menubar->setMask("navimenu_bg.png");
	menubar->setTransparent(true);
	menubar->setMovable(false);

	status = naviMgr->createNavi("status", 512, 256, NaviPosition(0, 0));
	status->loadFile("status.html");
	//status->setMask("navichat_bg.png");
	status->setTransparent(true);
	status->hide();

	chat = naviMgr->createNavi("chat", 512, 256, NaviPosition(BottomLeft, 40, -150));
	chat->loadURL("http://navi.agelessanime.com/chat/index.html");
	chat->setMask("navichat_bg.png");
	chat->hide();

	equip = naviMgr->createNavi("equip", 256, 512, NaviPosition(Right));
	equip->loadFile("equip.html");
	equip->setMask("naviequip_bg.png");
	equip->bind("itemEquipped", NaviDelegate(this, &NaviDemo::itemEquipped));
	equip->hide();
}

void NaviDemo::turnOn(Navi* caller, const JSArguments& args)
{
	std::string naviName = args.at(0).toString();
	Navi* navi = naviMgr->getNavi(naviName);

	if(naviName == "quit")
	{
		shouldQuit = true;
	}
	else if(navi)
	{
		navi->resetPosition();
		navi->show(true);
		navi->focus();
	}
}

void NaviDemo::turnOff(Navi* caller, const JSArguments& args)
{
	Navi* navi = naviMgr->getNavi(args.at(0).toString());

	if(navi)
		navi->hide(true);
}

void NaviDemo::hpChange(Navi* caller, const JSArguments& args)
{
	static short curHP = 100;

	if(args.at(0).toString() == "up")
		curHP += (rand() % 30) + 1;
	else
		curHP -= (rand() % 30) + 1;

	limit<short>(curHP, 0, 100);

	status->evaluateJS("setHP(?)", JSArgs(curHP));

	printf("Navi Menubar: Current HP is now %d\n", curHP);
}

void NaviDemo::messageSent(Navi* caller, const JSArguments& args)
{
	std::string nickname = args.at(0).toString();
	
	status->evaluateJS("$('playerName').setHTML(?)", JSArgs(nickname));

	printf("Navi Chat: %s\n", nickname.c_str());
}

void NaviDemo::itemEquipped(Navi* caller, const JSArguments& args)
{
	printf("Navi Equip: Item '%s' equipped!\n", args.at(0).toString().c_str());
}

void NaviDemo::levelChanged(Navi* caller, const JSArguments& args)
{
	int level = args.at(0).toInteger();

	printf("Navi Status: Level has been changed to: %d\n", level);
}

void NaviDemo::requestOpenGoogle(Navi* caller, const JSArguments& args)
{
	if(!google)
		google = naviMgr->createNavi("googleNavi", 800, 600, NaviPosition(Center));

	google->loadURL("http://hulu.com");
	google->focus();
}

void NaviDemo::Update()
{
	naviMgr->Update();
	Root::getSingleton().renderOneFrame();
	Ogre::WindowEventUtilities::messagePump();
	InputManager::getSingletonPtr()->capture();
}

void NaviDemo::parseResources()
{
    ConfigFile cf;
	cf.load("resources.cfg");
    ConfigFile::SectionIterator seci = cf.getSectionIterator();

    String secName, typeName, archName;
    while(seci.hasMoreElements())
    {
        secName = seci.peekNextKey();
        ConfigFile::SettingsMultiMap *settings = seci.getNext();
        ConfigFile::SettingsMultiMap::iterator i;
        for(i = settings->begin(); i != settings->end(); ++i)
        {
            typeName = i->first;
            archName = i->second;
            ResourceGroupManager::getSingleton().addResourceLocation(archName, typeName, secName);
        }
    }

	ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
}

void NaviDemo::loadInputSystem()
{
	inputMgr = InputManager::getSingletonPtr();
    inputMgr->initialise(renderWin);
    inputMgr->addMouseListener(this, "NaviDemoMouseListener");
	inputMgr->addKeyListener(this, "NaviDemoKeyListener");
}

bool NaviDemo::mouseMoved(const OIS::MouseEvent &arg)
{
	if(arg.state.Z.rel != 0) naviMgr->injectMouseWheel(arg.state.Z.rel);

	return naviMgr->injectMouseMove(arg.state.X.abs, arg.state.Y.abs);
}

bool NaviDemo::mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	return naviMgr->injectMouseDown(id);
}

bool NaviDemo::mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	return naviMgr->injectMouseUp(id);
}

bool NaviDemo::keyPressed( const OIS::KeyEvent &arg )
{
	if(naviMgr->isAnyNaviFocused())
		return true;

	return true;
}

bool NaviDemo::keyReleased( const OIS::KeyEvent &arg )
{
	static bool a = true;
	static bool b = true;

	switch(arg.key)
	{
	case OIS::KC_ESCAPE:
		shouldQuit = true;
		break;
	case OIS::KC_F1:
	{
		const RenderTarget::FrameStats& stats = renderWin->getStatistics();
		
		printf("_________________________\n");
		printf("%-14s%d\n", "Current FPS:", (int)stats.lastFPS);
		printf("%-14s%d\n", "Triangles:", (int)stats.triangleCount);
		printf("%-14s%d\n", "Batches:", (int)stats.batchCount);
		printf("=========================\n");
		break;
	}
	case OIS::KC_F2:
		sceneMgr->getRootSceneNode()->flipVisibility(true);
		break;
	case OIS::KC_F3:
		naviMgr->resetAllPositions();
		break;
	}

	return true;
}

void NaviDemo::windowMoved(RenderWindow* rw) {}

void NaviDemo::windowResized(RenderWindow* rw) 
{
	inputMgr->setWindowExtents(rw->getWidth(), rw->getHeight());
}

void NaviDemo::windowClosed(RenderWindow* rw) 
{
	shouldQuit = true;
}

void NaviDemo::windowFocusChange(RenderWindow* rw) {}
