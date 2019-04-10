#include "CMainMenuState.h"
#include "MyGui/MyGUI.h"
#include "MyGui/Platforms/Ogre/MyGUI_OgrePlatform.h"

MyGUI::Gui * mGUI = NULL; 

CMainMenuState::CMainMenuState() : CMenuState()
{
}

void CMainMenuState::Start()
{
	try
	{
		CGameManager* m_GameManager = CGameManager::Instance();

		CCamera* camera = m_GameManager->GetCurrCamera();
		// Create a 3rd person camera as default
		//m_Camera = new CCamera("Camera", m_GameManager->GetSceneManager());
		
		// Position it at 500 in Z direction
		//m_Camera->SetPosition(reVector3Df(0.0f, 0.0f, 500.0f));

		// Look back along -Z
		camera->GetCamera()->lookAt(reVector3Df(0.0f, 0.0f, -300.0f));
		camera->GetCamera()->setNearClipDistance(5);
		
		// If this exist, remove it
		m_GameManager->GetRenderWindow()->removeViewport(0);

		// Create one viewport, entire window
		Viewport* vp = m_GameManager->GetRenderWindow()->addViewport(camera->GetCamera());
		vp->setBackgroundColour(ColourValue(0, 0, 0));
	       
		// Alter the camera aspect ratio to match the viewport
		camera->GetCamera()->setAspectRatio(f32(vp->getActualWidth()) / f32(vp->getActualHeight()));

		MyGUI::OgrePlatform* m_Platform = new MyGUI::OgrePlatform();
		m_Platform->initialise(m_GameManager->GetRenderWindow(), m_GameManager->GetSceneManager());

		mGUI = new MyGUI::Gui();
		mGUI->initialise();

		// load layout
		MyGUI::LayoutManager::getInstance().load("sample.layout");
		// set callback
		MyGUI::ButtonPtr button = mGUI->findWidget<MyGUI::Button>("MyFirstButton");
		  //button->eventMouseButtonClick = MyGUI::newDelegate(CLASS_POINTER, &CLASS_NAME::METHOD_NAME); // CLASS_POINTER is pointer to CLASS_NAME (this)
		  // or
		  //button->eventMouseButtonClick = MyGUI::newDelegate(STATIC_METHOD_NAME);
		  //button->eventMouseButtonClick = MyGUI::newDelegate(GLOBAL_FUNC_NAME);
		mGUI->setVisiblePointer(true);
		
	}
	catch(MyGUI::Exception& e)
	{
		std::cerr << e.getFullDescription() << std::endl;
	}
}

void CMainMenuState::Update(f32 deltaTimeSecs)
{
	// update sounds
	//mSoundMgr->FrameStarted(mSoundNode, deltaTimeSecs);
	mGUI->injectFrameEntered(deltaTimeSecs);
}

void CMainMenuState::End()
{
	//GameManager->GetRenderWindow()->removeAllViewports();
}

bool CMainMenuState::mouseMoved(const OIS::MouseEvent &arg) 
{
	return true;
}

bool CMainMenuState::mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id) 
{ 
	return true;
}

bool CMainMenuState::mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	return true;
}

bool CMainMenuState::keyPressed(const OIS::KeyEvent &arg) 
{
	return true;
}

bool CMainMenuState::keyReleased(const OIS::KeyEvent &arg) 
{
	return true;
}