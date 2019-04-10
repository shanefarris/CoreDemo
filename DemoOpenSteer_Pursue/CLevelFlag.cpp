#include "CLevelFlag.h"
#include "CPhysicsManager.h"
#include "CPlayer.h"
#include "CCameraManager.h"
#include "CCharacterState.h"

using namespace Core;

std::vector<CPlayer*> m_Robots;

CLevelFlag::CLevelFlag()
{
}

CLevelFlag::~CLevelFlag()
{
}

void CLevelFlag::Update(const f32& elapsedTime)
{
	for(int i = 0; i < m_Robots.size(); i++)
	{
		m_Robots[i]->Update(elapsedTime, NULL, NULL);
	}
}

void CLevelFlag::End()
{
	m_GameManager->RemoveResourceArchive(".\\Media.zip", "CLevelFlag");
}

bool CLevelFlag::Load()
{
	m_GameManager->AddResourceArchive(".\\Media.zip", "CLevelFlag");

	// Camera
	CCameraManager::Instance()->CreateCamera(ECM_FREE);
	CCameraManager::Instance()->GetCamera()->setPosition(0, 100, 100);

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
	wander->SetPlayerPosition(reVector3Df(-100, 0, -100));
	m_Robots.push_back(wander);
	wander->SetCharacterState(EPS_WANDERING);

	// Create some AI units
	for(int i = 0; i < 10; i++)
	{
		CPlayer* robot = CGameObjectManager::Instance()->CreatePlayer("ROBOT1", EPT_TEAM_2);
		robot->SetPlayerPosition(reVector3Df(i * 20, 0, i * 10));
		m_Robots.push_back(robot);
		robot->SetCharacterState(EPS_SEEKING);
		robot->GetCharacterState()->SetTarget(wander->GetAiVehicle());
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
