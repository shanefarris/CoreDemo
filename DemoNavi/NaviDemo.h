#ifndef __NaviDemo_H__
#define __NaviDemo_H__

#include "Ogre.h"
#include "InputManager.h"
#include "Navi/Navi.h"

using namespace NaviLibrary;
using namespace Awesomium;

class NaviDemo : public OIS::MouseListener, public OIS::KeyListener, public Ogre::WindowEventListener
{
	Ogre::Viewport* viewport;
	Ogre::RenderWindow* renderWin;
	Ogre::SceneManager* sceneMgr;
	NaviLibrary::NaviManager* naviMgr;
	NaviLibrary::Navi* menubar, *status, *chat, *equip, *google;
	InputManager* inputMgr;
	void parseResources();
	void loadInputSystem();
public:
	bool shouldQuit;
	NaviDemo();
	~NaviDemo();

	void createScene();
	void setupNavis();

	void Update();

	void turnOn(Navi* caller, const JSArguments& args);
	void turnOff(Navi* caller, const JSArguments& args);
	void hpChange(Navi* caller, const JSArguments& args);
	void messageSent(Navi* caller, const JSArguments& args);
	void itemEquipped(Navi* caller, const JSArguments& args);
	void levelChanged(Navi* caller, const JSArguments& args);
	void requestOpenGoogle(Navi* caller, const JSArguments& args);
	
	bool mouseMoved(const OIS::MouseEvent &arg);
	bool mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id);
	bool mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id);

	bool keyPressed( const OIS::KeyEvent &arg );
	bool keyReleased( const OIS::KeyEvent &arg );	

	void windowMoved(Ogre::RenderWindow* rw);
	void windowResized(Ogre::RenderWindow* rw);
	void windowClosed(Ogre::RenderWindow* rw);
	void windowFocusChange(Ogre::RenderWindow* rw);
};

#endif