#include "CLevelSpace.h"
#include "CPhysicsManager.h"
#include "CSceneImporter.h"
#include "CPhysicsStrategy_Ode.h"

using namespace Core;
using namespace Core::SceneLoader;
using namespace Core::Physics;

CPhysicsStrategy_Ode* m_PhysicsStrategy = 0;

CLevelSpace::CLevelSpace()
{
}

CLevelSpace::~CLevelSpace()
{
}

void CLevelSpace::Update(f32 elapsedTime, OIS::Keyboard* Input, OIS::Mouse* Mouse)
{
	m_PhysicsStrategy->Update(elapsedTime);
}

void CLevelSpace::End()
{
	m_GameManager->RemoveResourceArchive(".\\Media.zip", "CLevelSpace");
}

bool CLevelSpace::Load()
{
	m_GameManager->AddResourceArchive(".\\Media.zip", "CLevelSpace");

	// Setup physics
	m_PhysicsStrategy = (CPhysicsStrategy_Ode*)Physics::CPhysicsManager::Instance()->CreateStrategy("physics");
	m_PhysicsStrategy->InitWorld();

	CSceneImporter* importer = new CSceneImporter(m_GameManager, m_PhysicsStrategy, m_SoundManager, 0, 0, "CLevelSpace");
	importer->ImportScene(".\\Level.xml");

	// Create a skybox
	m_SceneManager->setSkyBox(true, "SpaceSkyBox", 50 );

	// Create a light
	Light* l = m_SceneManager->createLight("MainLight");
	l->setPosition(20,80,50);

	// Setup the player
	m_Player = new CSpaceShipPlayer("ship", m_PhysicsStrategy);

	// Setup FrameListener
	m_FrameListenerFramework = new CSpaceShipFrameListener(m_GameManager->GetRenderWindow(), m_GameManager->GetCurrCamera());
	m_FrameListenerFramework->SetPlayer(m_Player);
	m_FrameListenerFramework->SetCamera(m_GameManager->GetCurrCamera());
	m_GameManager->GetEngineDevice()->addFrameListener(m_FrameListenerFramework);

	m_GameManager->GetCurrCamera()->SetMode(ECM_CHASE);
	// Set the player camera
	m_Player->SetCamera(m_GameManager->GetCurrCamera());

	return true;
}
