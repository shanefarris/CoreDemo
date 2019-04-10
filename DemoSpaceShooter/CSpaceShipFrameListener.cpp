#include "CSpaceShipFrameListener.h"
#include "CInputManager.h"

CSpaceShipFrameListener::CSpaceShipFrameListener(RenderWindow* win, CCamera* cam) : CFrameListenerFramework(win, cam) 
{ 
	m_GuiManager = Core::GUI::CGuiManager::Instance();
	CInputManager::Instance()->GetKeyboard()->setEventCallback(this);
	CInputManager::Instance()->GetMouse()->setEventCallback(this);
}

bool CSpaceShipFrameListener::frameRenderingQueued(const FrameEvent& evt)
{
	//if(CFrameListenerFramework::frameRenderingQueued(evt) == false)
	//	return false;

	m_Keyboard->capture();

	if (m_Player) 
	{
		m_Player->Update(evt.timeSinceLastFrame, m_Keyboard, 0);
		if (m_Camera) 
		{
			switch (m_Camera->GetMode()) 
			{
				case ECM_CHASE: // 3rd person chase
					m_Camera->Update(evt.timeSinceLastFrame, 
									m_Player->GetCameraNode()->_getDerivedPosition(), 
									m_Player->GetSightNode()->_getDerivedPosition());
					break;
				case ECM_FIXED: // 3rd person fixed
					m_Camera->Update(evt.timeSinceLastFrame, 
									reVector3Df(0.0f, 200.0f, 0.0f), 
									m_Player->GetSightNode()->_getDerivedPosition());
					break;
				case ECM_FPS: // 1st person
					m_Camera->Update(evt.timeSinceLastFrame, 
									m_Player->GetPlayerNode()->_getDerivedPosition(), 
									m_Player->GetSightNode()->_getDerivedPosition());
					break;
				case ECM_TOP_DOWN: // Top down view
					m_Camera->Update(evt.timeSinceLastFrame, 
									m_Player->GetCameraNode()->_getDerivedPosition() + reVector3Df(0.0f, 1000.0f, 0.0f), 
									m_Player->GetSightNode()->_getDerivedPosition());
					break;
				case ECM_SIDE: // side view
					m_Camera->Update(evt.timeSinceLastFrame, 
									m_Player->GetCameraNode()->_getDerivedPosition() + reVector3Df(-1000.0f, 0.0f, 0.0f), 
									m_Player->GetSightNode()->_getDerivedPosition());
					break;
			}
		}
	}

	// 3rd Person - Chase Camera
	if (m_Keyboard->isKeyDown(OIS::KC_F1)) 
	{
		m_Camera->SetMode(ECM_CHASE);
		if (m_Player)
			m_Player->SetVisible(true);
		if (m_Camera) 
		{
			if (m_Player)
				m_Camera->InstantUpdate(m_Player->GetCameraNode()->_getDerivedPosition(), m_Player->GetSightNode()->_getDerivedPosition());
			m_Camera->SetTightness(0.01f);
		}
	}
	// 3rd Person - Fixed Camera
	if (m_Keyboard->isKeyDown(OIS::KC_F2)) 
	{
		m_Camera->SetMode(ECM_FIXED);
		if (m_Player)
			m_Player->SetVisible(true);
		if (m_Camera) 
		{
			if (m_Player)
				m_Camera->InstantUpdate (reVector3Df(0.0f, 200.0f, 0.0f), m_Player->GetSightNode()->_getDerivedPosition());
			m_Camera->SetTightness(0.01f);
		}
	}
	// 1st Person
	if (m_Keyboard->isKeyDown(OIS::KC_F3))  
	{
		m_Camera->SetMode(ECM_FPS);
		if (m_Player)
			m_Player->SetVisible(false);
		if (m_Camera) 
		{
			if(m_Player)
				m_Camera->InstantUpdate(m_Player->GetPlayerNode()->_getDerivedPosition(), m_Player->GetSightNode()->_getDerivedPosition());
			m_Camera->SetTightness(1.0f);
		}
	}
	// Top down view
	if (m_Keyboard->isKeyDown(OIS::KC_F4)) 
	{
		m_Camera->SetMode(ECM_TOP_DOWN);
		if (m_Player)
			m_Player->SetVisible(true);
		if (m_Camera) 
		{
			if (m_Player)
				m_Camera->InstantUpdate (reVector3Df(0.0f, 1000.0f, 0.0f), m_Player->GetSightNode()->_getDerivedPosition());
			m_Camera->SetTightness(0.01f);
		}
	}
	// Side down view
	if (m_Keyboard->isKeyDown(OIS::KC_F5)) 
	{
		m_Camera->SetMode(ECM_SIDE);
		if (m_Player)
			m_Player->SetVisible(true);
		if (m_Camera) 
		{
			if (m_Player)
				m_Camera->InstantUpdate (reVector3Df(-1000.0f, 0.0f, 0.0f), m_Player->GetSightNode()->_getDerivedPosition());
			m_Camera->SetTightness(0.01f);
		}
	}

	// Exit if we press Esc
	if(m_Keyboard->isKeyDown(OIS::KC_ESCAPE))
		return false;

	//m_GuiManager->FrameStarted(evt.timeSinceLastFrame);

	return true;
}


bool CSpaceShipFrameListener::mouseMoved(const OIS::MouseEvent &arg)
{
	return true;
}

bool CSpaceShipFrameListener::mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	return true;
}

bool CSpaceShipFrameListener::mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	return true;
}

bool CSpaceShipFrameListener::keyPressed(const OIS::KeyEvent &arg)
{
	return true;
}

bool CSpaceShipFrameListener::keyReleased(const OIS::KeyEvent &arg)
{
	return true;
}