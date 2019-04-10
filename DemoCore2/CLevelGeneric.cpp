#include "CLevelGeneric.h"
#include "CPhysicsManager.h"
#include "CPhysicsStrategy_PhysX.h"
#include "CNatureManager.h"
#include "CLoadingScreen.h"
#include "CSceneImporter.h"
#include "CTextOutput.h"
#include "CItemsManager.h"
#include "CCameraManager.h"
#include "CGameObjectManager.h"

#include "Terrain/OgreTerrainGroup.h"

using namespace Core;
using namespace Core::Physics;
using namespace Core::Nature;

CLevelGeneric::CLevelGeneric() : CGameState()
{
}

CLevelGeneric::~CLevelGeneric()
{
	CPhysicsManager::Instance()->DeleteStrategy("CLevelGeneric");
	CPhysicsManager::Instance()->DeleteStrategy("Weapons");
}

void CLevelGeneric::Update(const f32& deltaTime)
{
	CCameraManager::Instance()->Update(deltaTime);

	if(m_PhysicsStrategy)
		m_PhysicsStrategy->Update(deltaTime);

	CGameObjectManager::Instance()->Update(deltaTime);
	
	CNatureManager::Instance()->Update(deltaTime);
	
}

void CLevelGeneric::End()
{
}

bool CLevelGeneric::Load()
{
	try{
	OgreSpecific::CLoadingScreen* screen = new OgreSpecific::CLoadingScreen();

	m_GameManager->AddResourceArchive(".\\Media.zip", "CLevelGeneric");
	CItemsManager* m_ItemsManager = CItemsManager::Instance();

	SetupPhysics();

	// load the scene
	m_Importer = new SceneLoader::CSceneImporter(m_GameManager, m_PhysicsStrategy, NULL, m_ItemsManager, 
		CGameObjectManager::Instance(), "CLevelGeneric");
	m_Importer->ImportScene(".\\TestLevel.xml");

	// Fill the scene with some creates
	SceneNode *node;
	Entity *entity;
	for (u32 i = 0; i < 5; ++i) 
	{
		f32 x = ((float) rand ()) / ((float) RAND_MAX) * -10000;
		f32 z = ((float) rand ()) / ((float) RAND_MAX) * 10000;

		node = m_SceneManager->getRootSceneNode()->createChildSceneNode(
			StringConverter::toString(i), 
			reVector3Df(x, 300, z));
		node->setScale(2, 2, 2);
		entity = m_SceneManager->createEntity(StringConverter::toString(i), "cube.1m.mesh");
		node->attachObject(entity);

		if(m_PhysicsStrategy)
			m_PhysicsStrategy->AddCube(node, entity, 1.0f, 1.0f, 0.01f);
	}

	// setup player
	m_Player = CGameObjectManager::Instance()->CreatePlayer(EPT_SOLIDER1, "Player", m_PhysicsStrategy, true);
	m_Player->GetPlayerNode()->setPosition(10, 10, 10);

	// Set Fps camera after the player
	//CCameraManager::Instance()->CreateCamera(ECM_FPS);

	// Setup FrameListener
	m_FrameListenerFramework = new CFpsFrameListener(m_GameManager->GetRenderWindow());
	m_GameManager->GetEngineDevice()->addFrameListener(m_FrameListenerFramework);

	}
	catch(char* e)
	{
		std::cerr << e << std::endl;
		return false;
	}
	return true;
}

void CLevelGeneric::SetupPhysics()
{
	// Setup physics
	m_PhysicsStrategy = (Core::Physics::CPhysicsStrategy_PhysX*)CPhysicsManager::Instance()->CreateStrategy("CLevelGeneric", EPS_PHYSX);
	if(m_PhysicsStrategy)
		m_PhysicsStrategy->InitWorld();
}
