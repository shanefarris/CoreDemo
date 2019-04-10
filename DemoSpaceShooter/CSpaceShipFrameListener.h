#ifndef __CSPACESHIPFRAMELISTENER_H__
#define __CSPACESHIPFRAMELISTENER_H__

#include "CFrameListenerFramework.h"
#include "CPlayer.h"
#include "CCamera.h"
#include "CGuiManager.h"

using namespace Core;

// Event handler to add ability to alter curvature
class CSpaceShipFrameListener : public CFrameListenerFramework
{
private:
	static float fDefDim;
	static float fDefVel;

	Core::GUI::CGuiManager*	m_GuiManager;

 public:
	CSpaceShipFrameListener(RenderWindow* win, CCamera* cam);
	bool frameRenderingQueued(const FrameEvent& evt);

	bool mouseMoved(const OIS::MouseEvent &arg);
	bool mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id);
	bool mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id);
	bool keyPressed(const OIS::KeyEvent &arg);
	bool keyReleased(const OIS::KeyEvent &arg);
};


#endif // __CSPACESHIPFRAMELISTENER_H__