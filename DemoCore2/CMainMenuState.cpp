#if 0
#include "CMainMenuState.h"
#include "CGuiManager.h"
#include "CInputManager.h"
#include "CGameManager.h"
#include "CGuiStrategy_MyGui.h"
#include "CMainMenuPage1.h"

using namespace Core::GUI;

CGuiStrategy_MyGui* Strategy = 0;
CGameManager* m_GameManager = CGameManager::Instance();
CInputManager* m_InputManager = CInputManager::Instance();

CMainMenuPage1* page = 0;

CMainMenuState::CMainMenuState()
{
}

void CMainMenuState::Start()
{
	try
	{
		CGameManager::Instance()->AddResourceArchive("Media.zip", "MainMenu");

		CGuiManager* manager = CGuiManager::Instance();
		Strategy = (CGuiStrategy_MyGui*)manager->CreateStrategy("MainMenu");
		//Strategy->GetGui()->load("Wallpaper0.layout");

		page = new CMainMenuPage1("Menu1.layout");
		page->eventButtonPress = MyGUI::newDelegate(this, &CMainMenuState::NotifyMouseButtonClick);
		page->setVisible(true);

		MyGUI::PointerManager::getInstance().setVisible(true);

		m_InputManager->AddKeyListener(this, "MainMenu");
		m_InputManager->AddMouseListener(this, "MainMenu");

		mIsAnim = false;

	}
	catch(MyGUI::Exception& e)
	{
		std::cerr << e.getFullDescription() << std::endl;
	}
}

void CMainMenuState::Update(f32 deltaTimeSecs)
{
	Strategy->GetGui()->injectFrameEntered(deltaTimeSecs);
}

void CMainMenuState::End()
{
	m_InputManager->RemoveKeyListener("MainMenu");
	m_InputManager->RemoveMouseListener("MainMenu");

	page->~CMainMenuPage1();
	Strategy->Desactivate();
	CGameManager::Instance()->SetReadyToAdvance(true);
}

bool CMainMenuState::mouseMoved(const OIS::MouseEvent &arg) 
{
	return Strategy->MouseMoved(arg);
}

bool CMainMenuState::mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id) 
{ 
	return Strategy->MousePressed(arg, id);
}

bool CMainMenuState::mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	return Strategy->MouseReleased(arg, id);
}

bool CMainMenuState::keyPressed(const OIS::KeyEvent &arg) 
{
	if( arg.key == OIS::KC_ESCAPE )
	{
		m_GameManager->SetQuit(true);
	}

	return Strategy->KeyPressed(arg);
}

bool CMainMenuState::keyReleased(const OIS::KeyEvent &arg) 
{
	return Strategy->KeyReleased(arg);
}

void CMainMenuState::NotifyMouseButtonClick(MyGUI::WidgetPtr sender, bool isCompleted)
{
	if (isCompleted)
	{
		mIsAnim = false;
		return;
	}

	if (mIsAnim) 
		return;

	mIsAnim = true;

	page->setVisible(false);

	page->setVisible(true);
}

#endif