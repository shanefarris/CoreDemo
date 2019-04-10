#include "CFpsFrameListener.h"
#include "CInputManager.h"

CFpsFrameListener::CFpsFrameListener(RenderWindow* win) : CFrameListenerFramework(win) 
{ 
	m_GuiManager = Core::GUI::CGuiManager::Instance();
	CInputManager::Instance()->GetKeyboard()->setEventCallback(this);
	CInputManager::Instance()->GetMouse()->setEventCallback(this);
}

bool CFpsFrameListener::frameRenderingQueued(const FrameEvent& evt)
{
	CFrameListenerFramework::frameRenderingQueued(evt);

	//if(m_Window->isClosed())	
	//	return false;

	////Need to capture/update each device
	//m_Keyboard->capture();
	//m_Mouse->capture();

	//if (m_Player) 
	//{
	//	m_Player->Update(evt.timeSinceLastFrame, m_Keyboard, m_Mouse);

	//	// 1st Person
	//	if (m_Keyboard->isKeyDown(OIS::KC_F1)) 
	//	{
	//		if(m_Camera)
	//		{
	//			m_Player->SetVisible(false);
	//			m_Camera->SetMode(ECM_FPS);
	//			m_Camera->InstantUpdate(m_Player->GetCameraNode()->_getDerivedPosition(), 
	//									m_Player->GetSightNode()->_getDerivedPosition());
	//			m_Camera->SetTightness(0.01f);
	//		}
	//	}
	//	// 3rd Person
	//	if (m_Keyboard->isKeyDown(OIS::KC_F2))  
	//	{
	//		if(m_Camera)
	//		{
	//			m_Camera->SetMode(ECM_CHASE);
	//			m_Player->SetVisible(true);
	//			m_Camera->InstantUpdate(m_Player->GetPlayerNode()->_getDerivedPosition(), 
	//									m_Player->GetSightNode()->_getDerivedPosition());
	//			m_Camera->SetTightness(1.0f);
	//		}
	//	}
	//	// Free flying
	//	if (m_Keyboard->isKeyDown(OIS::KC_F3))  
	//	{
	//		if(m_Camera)
	//		{
	//			m_Camera->SetMode(ECM_FREE);
	//		}
	//	}
	//}

	return true;
}


bool CFpsFrameListener::mouseMoved(const OIS::MouseEvent &arg)
{
	return true;
}

bool CFpsFrameListener::mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	return true;
}

bool CFpsFrameListener::mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	return true;
}

bool CFpsFrameListener::keyPressed(const OIS::KeyEvent &arg)
{
	if(arg.key == OIS::KC_ESCAPE)
	{
		CGameManager::Instance()->SetReadyToAdvance(true);
	}

	return true;
}

bool CFpsFrameListener::keyReleased(const OIS::KeyEvent &arg)
{
	return true;
}