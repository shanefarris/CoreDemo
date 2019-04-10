#include "CCustomFpsFrameListener.h"
#include "CInputManager.h"
#include "SoundEngine.h"

using namespace Sound;

CSoundManager* SoundManager = 0;

CCustomFpsFrameListener::CCustomFpsFrameListener(RenderWindow* win, CCamera* cam) : CFrameListenerFramework(win, cam) 
{ 
	//m_GuiManager = Core::GUI::CGuiManager::Instance();
	CInputManager::Instance()->GetKeyboard()->setEventCallback(this);
	CInputManager::Instance()->GetMouse()->setEventCallback(this);

	// Setup sound manager
	SoundManager = CSoundManager::Instance();
}

bool CCustomFpsFrameListener::frameRenderingQueued(const FrameEvent& evt)
{
	m_Keyboard->capture();

	if(m_Keyboard->isKeyDown(OIS::KC_Q))
	{
		CSound* sound = SoundManager->CreateSound("siren", "siren.ogg", true);
		if(sound)
			sound->Play();
		else
			std::cout << "Error: sound is null" << std::endl;
	}
	else if(m_Keyboard->isKeyDown(OIS::KC_Y))
	{
		CSound* sound = SoundManager->GetSound("siren");
		if(sound)
			sound->Stop();
		else
			std::cout << "Error: sound is null" << std::endl;
	}
	else if(m_Keyboard->isKeyDown(OIS::KC_W))
	{
		CSound* sound = SoundManager->CreateSound("theme", "TitleThemeDemo_14.ogg", true, true);
		if(sound)
			sound->Play();
		else
			std::cout << "Error: sound is null" << std::endl;
	}
	else if(m_Keyboard->isKeyDown(OIS::KC_U))
	{
		CSound* sound = SoundManager->GetSound("theme");
		if(sound)
			sound->Stop();
		else
			std::cout << "Error: sound is null" << std::endl;
	}

	return CFrameListenerFramework::frameRenderingQueued(evt);
}


bool CCustomFpsFrameListener::mouseMoved(const OIS::MouseEvent &arg)
{
	return true;
}

bool CCustomFpsFrameListener::mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	return true;
}

bool CCustomFpsFrameListener::mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	return true;
}

bool CCustomFpsFrameListener::keyPressed(const OIS::KeyEvent &arg)
{
	return true;
}

bool CCustomFpsFrameListener::keyReleased(const OIS::KeyEvent &arg)
{
	return true;
}