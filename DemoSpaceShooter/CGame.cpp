#include "CGame.h"

using namespace Core;

CGame::CGame()
{
	m_GameManager = CGameManager::Instance();
	m_InputManager = CInputManager::Instance();
	LastTime = m_Timer.getMilliseconds();
	m_ActiveState = NULL;
}

CGame::~CGame()
{
}

void CGame::Run()
{
	Level = new CLevelSpace();

	m_States.push_back(Level);

	m_GameManager->SetReadyToAdvance(true);
	
	while(m_GameManager->GetQuit() == false)
	{
		Update();
	}
}

void CGame::Update()
{
	f32 deltaTime = (m_Timer.getMilliseconds() - LastTime) / 1000.0f;
	LastTime = m_Timer.getMilliseconds();

	// if there is an active state...
	if(m_ActiveState)
	{
		if(m_GameManager->GetEngineDevice()->renderOneFrame() == false)
			m_GameManager->SetQuit(true);
		m_ActiveState->Update(deltaTime);			// update level state
		Ogre::WindowEventUtilities::messagePump();	// Ogre specific:handle any window events
		m_InputManager->Capture();					// get any keyboard/mouse events
	}

	// check if ready to advance to next state
	if(m_GameManager->GetReadyToAdvance())
	{
		// if already have active one, exit out
		if(m_ActiveState)
		{
			m_ActiveState->End();	// close out state
			delete m_ActiveState;	// free up resources
		}

		// if there is another state in the queue
		if(m_States.size() > 0)
		{
			m_ActiveState = m_States.back();	// assign next state
			m_ActiveState->Load();			// trigger the rendering of new state
			m_States.pop_back();		// clear out state from the queue
			m_GameManager->SetReadyToAdvance(false);
		}
		else
		{
			m_GameManager->SetQuit(true);			// sense no more states, exit out
			return;
		}
	}
}
