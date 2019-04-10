#include "CLevelFlag.h"
#include "CPhysicsManager.h"
#include "CCameraManager.h"
#include "CGameManager.h"
#include "CCharacterState.h"
#include "CGameObjectManager.h"
#include "CPlayer.h"

using namespace Core;

CLevelFlag::CLevelFlag()
{
}

CLevelFlag::~CLevelFlag()
{
}

void CLevelFlag::Update(const f32& elapsedTime)
{
	CGameObjectManager::Instance()->Update(elapsedTime);
	CCameraManager::Instance()->Update(elapsedTime);
}

void CLevelFlag::End()
{
	m_GameManager->RemoveResourceArchive(".\\Media.zip", "OgreOde");
}

bool CLevelFlag::Load()
{
	m_GameManager->AddResourceArchive(".\\Media.zip", "OgreOde");

	// Camera
	CCameraManager::Instance()->CreateCamera(ECM_FREE);
	CCameraManager::Instance()->GetCamera()->setPosition(0, 100, 100);

	// Create a physics strategy
	Core::Physics::IPhysicsStrategy* physics = Core::Physics::CPhysicsManager::Instance()->CreateStrategy("physics");
	if(physics)
	{
		physics->InitWorld();
		physics->ShowDebug(true);
	}

	Entity *ent;
    // Define a floor plane mesh
    Plane p;
    p.normal = reVector3Df::UNIT_Y;
    p.d = 0;
    MeshManager::getSingleton().createPlane(
        "FloorPlane", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, 
        p, 10000, 10000, 10, 10, true, 1, 60, 60, reVector3Df::UNIT_Z);

    // Create an entity (the floor)
	ent = m_GameManager->GetSceneManager()->createEntity("floor", "FloorPlane");
    ent->setMaterialName("Scenario/Floor");

	// Attach to child of root node, better for culling (otherwise bounds are the combination of the 2)
    m_GameManager->GetSceneManager()->getRootSceneNode()->createChildSceneNode()->attachObject(ent);

	// Create robot to pursue
	CPlayer* wander = CGameObjectManager::Instance()->CreatePlayer("ZOMBIE1", EPT_TEAM_1);
	wander->SetPlayerPosition(reVector3Df(300, 0, 50));
	wander->SetCharacterState(EPS_WANDERING);

	// Add some crates
	SceneNode *node;
	Entity *entity;
	for (unsigned int i = 0; i < 50; ++i) 
	{
		node = m_SceneManager->getRootSceneNode()->createChildSceneNode(
			StringConverter::toString(i), 
			reVector3Df(25 * i, 0 * i, 25));
		node->setScale(0.5, 0.5, 0.5);
		entity = m_SceneManager->createEntity(StringConverter::toString(i), "crate.mesh");
		node->attachObject(entity);

		CGameObjectManager::Instance()->AddSphereObstacle(entity, node, true);
		//m_PhysicsStrategy->AddCube(node, entity, 1.0f, 1.0f, 1.0f);
	}

	// Create some AI units
	for(int i = 0; i < 10; i++)
	{
		CPlayer* zombie = CGameObjectManager::Instance()->CreatePlayer("ZOMBIE1", EPT_TEAM_1);
		zombie->SetPlayerPosition(reVector3Df(i * 20, 2, i * 10));
		zombie->SetCharacterState(EPS_SEEKING);
		zombie->GetCharacterState()->SetTarget(wander->GetAiVehicle());
	}

	// Setup FrameListener
	m_FrameListenerFramework = new CFpsFrameListener(m_GameManager->GetRenderWindow());
	m_GameManager->GetEngineDevice()->addFrameListener(m_FrameListenerFramework);

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
		game.AddGameState(new CLevelFlag());
		game.Run();
    } 
	catch( Ogre::Exception& e ) 
	{
		fprintf(stderr, "An exception has occurred: %s\n", e.what());
    }
    
    return 0;
}