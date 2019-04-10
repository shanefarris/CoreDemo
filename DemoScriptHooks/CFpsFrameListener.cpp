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
	if(CFrameListenerFramework::frameRenderingQueued(evt) == false)
		return false;

	//m_Keyboard->capture();

	//if (m_Player) 
	//{
	//	m_Player->Update(evt.timeSinceLastFrame, m_Keyboard);
	//	if (m_Camera) 
	//	{
	//		switch (m_Camera->GetMode()) 
	//		{
	//			case ECM_FPS: // 1st person
	//				m_Camera->Update(evt.timeSinceLastFrame, 
	//								m_Player->GetWorldPosition(), 
	//								m_Player->GetSightNode()->_getDerivedPosition());
	//				break;
	//			case ECM_SIDE: // side view
	//				m_Camera->Update(evt.timeSinceLastFrame, 
	//								m_Player->GetCameraNode()->_getDerivedPosition() + reVector3Df(-1000.0f, 0.0f, 0.0f), 
	//								m_Player->GetSightNode()->_getDerivedPosition());
	//				break;
	//		}
	//	}
	//}

	//// 3rd Person - Chase Camera
	//if (m_Keyboard->isKeyDown(OIS::KC_F1)) 
	//{
	//	m_Camera->SetMode(ECM_CHASE);
	//	if (m_Player)
	//		m_Player->SetVisible(true);
	//	if (m_Camera) 
	//	{
	//		if (m_Player)
	//			m_Camera->InstantUpdate(m_Player->GetCameraNode()->_getDerivedPosition(), m_Player->GetSightNode()->_getDerivedPosition());
	//		m_Camera->SetTightness(0.01f);
	//	}
	//}
	//// 1st Person
	//if (m_Keyboard->isKeyDown(OIS::KC_F3))  
	//{
	//	m_Camera->SetMode(ECM_FPS);
	//	if (m_Player)
	//		m_Player->SetVisible(false);
	//	if (m_Camera) 
	//	{
	//		if(m_Player)
	//			m_Camera->InstantUpdate(m_Player->GetWorldPosition(), m_Player->GetSightNode()->_getDerivedPosition());
	//		m_Camera->SetTightness(1.0f);
	//	}
	//}

	//// Exit if we press Esc
	//if(m_Keyboard->isKeyDown(OIS::KC_ESCAPE))
	//	return false;

	//m_GuiManager->FrameStarted(evt.timeSinceLastFrame);

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
	return true;
}

bool CFpsFrameListener::keyReleased(const OIS::KeyEvent &arg)
{
	return true;
}