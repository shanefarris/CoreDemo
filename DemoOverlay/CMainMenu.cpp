#ifdef COMPILE_OVERLAYS
#include "CMainMenu.h"
#include "CInputManager.h"
#include "CCameraManager.h"
#include "CGameObjectManager.h"
#include "CPlayer.h"

#include "Overlays/ButtonWidget.h"
#include "Overlays/MouseCursor.h"
#include "Overlays/ImageWidget.h"
#include "Overlays/LabelWidget.h"
#include "Overlays/EntryWidget.h"
#include "Overlays/BigTitleWidget.h"
#include "Overlays/TextEntryWidget.h"
#include "Overlays/YesNoBoxWidget.h"
#include "Overlays/Compass.h"
#include "Overlays/FileBrowserWidget.h"
#include "Overlays/Notifications.h"

using namespace Core::Overlays;

f32	mParentBoundLeft = 0.0f;
f32	mParentBoundRight = 800.0f;
f32	mParentBoundTop = 0.0f;
f32	mParentBoundBottom = 600.0f;

ButtonWidget* button = 0;
MouseCursor* mouse = 0;
EntryWidget* btnList[15] = { 0 };
BigTitleWidget* titleWidget = 0;
TextEntryWidget* textEntry = 0;
YesNoBoxWidget* yesNoBox = 0;
Compass* compass = 0;
FileBrowserWidget* fbw = 0;
Notifications* note = 0;

CPlayer* player = 0;


CMainMenu::CMainMenu()
{ 
	CGameManager* m_GameManager = CGameManager::Instance();
	CInputManager::Instance()->GetKeyboard()->setEventCallback(this);
	CInputManager::Instance()->GetMouse()->setEventCallback(this);

	m_GameManager->AddResourceArchive(".\\Media.zip", "CLevelOverlay");

    // Position the camera
	CCameraManager::Instance()->CreateCamera(ECM_FREE);

	mParentBoundRight = m_GameManager->GetRenderWindow()->getWidth();
	mParentBoundBottom = m_GameManager->GetRenderWindow()->getHeight();
}

bool CMainMenu::Load()
{
#pragma region Setup mouse/overlay panel/button
	mouse = new MouseCursor();
	mouse->init();
	mouse->setImage("cursor_black.png");
	mouse->setWindowDimensions(m_GameManager->GetRenderWindow()->getWidth(),
							   m_GameManager->GetRenderWindow()->getHeight());
	mouse->setVisible(true);

	// Setup overlay panel
	Overlay* mPanelOverlay = OverlayManager::getSingleton().create("overlay");
	mPanelOverlay->setZOrder(500);
	mPanelOverlay->show();

	OverlayElement* pBaseElement = OverlayManager::getSingleton().createOverlayElement("Panel", "p");
	OverlayContainer* pContainer = static_cast<Ogre::OverlayContainer*>(pBaseElement);
	mPanelOverlay->add2D(pContainer);

	// Setup button
	button = new ButtonWidget();
	button->init("btnExit", 0, 0, 640, 480, .1, .8, "Exit", false);
	button->activate();
	bool test = button->show();
	pContainer->addChild(button->getElement());
#pragma endregion

#pragma region Setup image/label
	ImageWidget* image = new ImageWidget();
	image->init("logo", "Core/LogoMat", 118, 118, 0.1f, 0.1f, 
		mParentBoundLeft, mParentBoundTop, mParentBoundRight, mParentBoundBottom);
	image->setAlignment("center","center");
	image->show();
	pContainer->addChild(image->getElement());

	LabelWidget* label = new LabelWidget();
	label->init("label", mParentBoundLeft, mParentBoundTop, mParentBoundRight, 
		mParentBoundBottom, 0.0f, -0.03f, "This is a label");
	label->setAlignment("bottom","center");	
	label->setColour(0.4f, 0.0f, 0.0f, 1.0f);
	pContainer->addChild(label->getElement());

	//Setup button list
	f32 mXProp = 0.5f;
	f32 mYProp = 0.5f;
	f32 mXPos = 0.0f;
	f32 mYPos = 0.0f;
	f32 mWidgetWidthPixels = 500.0f;
	f32 mWidgetHeightPixels = 300.0f;
	for (int i = 0; i < 3; i++) 
	{
		float increment = i * 0.055f;
		String name = StringConverter::toString(i);
		btnList[i] = new EntryWidget();
		btnList[i]->init(name.c_str(), mXPos, mYPos, mXPos + mWidgetWidthPixels, mYPos + mWidgetHeightPixels, 
			0.15f, 0.2f + increment, "Entry");
		pContainer->addChild(btnList[i]->getElement());
	}
#pragma endregion

#pragma region Setup big title/text entry
	titleWidget = new BigTitleWidget();
	titleWidget->init("unique name", mParentBoundLeft, mParentBoundTop, mParentBoundRight, 
		mParentBoundBottom, "Title");
	pContainer->addChild(titleWidget->getElement());
	titleWidget->show();

	textEntry = new TextEntryWidget();
	textEntry->init("textEntry", mXPos, mYPos, mXPos + mWidgetWidthPixels, mYPos + mWidgetHeightPixels, 
		0.8f, 0.3f);
	pContainer->addChild(textEntry->getElement());
	textEntry->show();
	textEntry->setTitleText("Scenario Title");
	textEntry->setTypedText("...");

	// Setup yes no box
	yesNoBox = new YesNoBoxWidget();
	yesNoBox->init("yesNoBox", mParentBoundLeft, mParentBoundTop, mParentBoundRight, mParentBoundBottom, 0.5f, 0.5f);
	yesNoBox->show();
#pragma endregion

#pragma region HUD stuff

	f32 hudHealthHeight = 40;
	f32 hudHealthWidth = 50;
	ImageWidget* imgHealth = new ImageWidget();
	imgHealth->init("health", "Core/HUD/Health", hudHealthWidth, hudHealthHeight, 
		mParentBoundBottom - (hudHealthHeight + 5), // X 
		5,											// Y
		mParentBoundLeft, mParentBoundTop, mParentBoundRight, mParentBoundBottom);
	imgHealth->show();
	pContainer->addChild(imgHealth->getElement());

	LabelWidget* txtHealth = new LabelWidget();
	txtHealth->init("txtHealth", mParentBoundLeft, mParentBoundTop, mParentBoundRight, 
		mParentBoundBottom, 0, 0, "100");
	txtHealth->moveAbsWidget(30, 570);
	txtHealth->setColour(1, 1, 1, 1.0f);
	pContainer->addChild(txtHealth->getElement());

#pragma endregion

#pragma region FileBrowserWidget, Notification

	// FileBrowserWidget
	fbw = new FileBrowserWidget();
	fbw->init("fbw", mParentBoundLeft, mParentBoundRight, mParentBoundTop, mParentBoundBottom, .5, 0);
	fbw->show();
	fbw->hide();  // don't need to demo it for now.

	// Notifications
	note = new Notifications();
	note->AddNotification("test");
	note->Show();

#pragma endregion

#pragma region Compass
	compass = new Compass(200, 50);
	compass->show();
	player = CGameObjectManager::Instance()->CreatePlayer(EPT_SOLIDER1, "Player", NULL, true);

#pragma endregion

	m_GameManager->GetEngineDevice()->addFrameListener(this);
	return true;
}

void CMainMenu::End()
{
}

bool CMainMenu::frameRenderingQueued(const FrameEvent& evt)
{
	//CFrameListenerFramework::frameRenderingQueued(evt);

	m_Mouse->capture();
	m_Keyboard->capture();
	const OIS::MouseState &ms = m_Mouse->getMouseState();
	mouse->updatePosition(ms.X.abs, ms.Y.abs);

	// Update button
	button->update(ms.X.abs, ms.Y.abs, ms.buttonDown(OIS::MouseButtonID::MB_Left));
	if(button->isClicked())
		CGameManager::Instance()->SetReadyToAdvance(true);

	// Update text box
	bool clicked = false;
	textEntry->update(ms.X.abs, ms.Y.abs, ms.buttonDown(OIS::MouseButtonID::MB_Left));
	if (textEntry->isClicked()) 
	{
		textEntry->setSelected(true);
	}
	
	// Update yes no box
	if (yesNoBox->isShowing())
	{
		yesNoBox->update(ms.X.abs, ms.Y.abs, ms.buttonDown(OIS::MouseButtonID::MB_Left)); // update the popup
		if (yesNoBox->isOKClicked()) 
		{ 
			// check if "Yes" clicked
			yesNoBox->hide(); // close popup
		}
		if (yesNoBox->isCancelClicked()) 
		{ 
			// check if "Cancel" clicked
			yesNoBox->hide(); // close popup and back to file saver details menu
		}
	}

	// Update file browswer
	if(fbw->IsVisible())
	{
		fbw->update(ms.X.abs, ms.Y.abs, ms.buttonDown(OIS::MouseButtonID::MB_Left));
		if(fbw->isOKClicked())
		{
		}
		else if(fbw->isCancelClicked())
		{
		}
	}

	// Notifications
	note->FrameStarted(evt.timeSinceLastFrame);

	// Update compass
	compass->FrameStarted(player->GetPlayerNode());

	return true;
}

bool CMainMenu::frameEnded(const FrameEvent& Event)
{
	return true;
}

bool CMainMenu::mouseMoved(const OIS::MouseEvent &arg)
{
	return true;
}

bool CMainMenu::mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	return true;
}

bool CMainMenu::mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	return true;
}

bool CMainMenu::keyPressed(const OIS::KeyEvent &arg)
{
	if(arg.key == OIS::KC_ESCAPE)
	{
		CGameManager::Instance()->SetReadyToAdvance(true);
	}

	if(textEntry->isSelected())
	{
		char* test = (char*)&arg.text;
		textEntry->setTypedText(test);
	}
	return true;
}

bool CMainMenu::keyReleased(const OIS::KeyEvent &arg)
{
	return true;
}

#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#include "CGame.h"

int main(int argc, char **argv);
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT argc) { return main(1, &strCmdLine); }

int main(int argc, char **argv)
{
	Core::CGame game;
    try 
	{
		game.AddGameState(new CMainMenu());
		game.Run();
    } 
	catch( Ogre::Exception& e ) 
	{
		fprintf(stderr, "An exception has occurred: %s\n", e.what());
    }
    
    return 0;
}

#else

void main() { }

#endif