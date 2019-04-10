#include "CLevelFlag.h"
#include "CPhysicsManager.h"
#include "CCameraManager.h"
#include "CGameObjectManager.h"
#include "CFpsVehicle.h"
#include "CSerializer.h"
#include "CPlayer.h"

using namespace Core;

bool save(s32 timeId)
{
	Core::SceneLoader::CSerializer* s = new Core::SceneLoader::CSerializer(EST_XML);
	s->SaveGame();
	CGameManager::Instance()->SetReadyToAdvance(true);
	return true;
}

CLevelFlag::CLevelFlag()
{
}

CLevelFlag::~CLevelFlag()
{
}

void CLevelFlag::Update(const f32& elapsedTime)
{
	CGameObjectManager::Instance()->Update(elapsedTime);
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

	// Create some AI units
	for(int i = 0; i < 1; i++)
	{
		CPlayer* robot = CGameObjectManager::Instance()->CreatePlayer("ROBOT1", EPT_TEAM_1);
		robot->SetPlayerPosition(reVector3Df(i * 100, 0, i * 20));
		robot->SetCharacterState(EPS_WANDERING);
		robot->GetAiVehicle()->setMaxForce(50.0f);
	}

	// Add some obstacles
	SceneNode* node = 0;
	Entity* entity = 0;
	for(int i = 0; i < 20; i++)
	{
		f32 x = ((float) rand ()) / ((float) RAND_MAX) * 200;
		f32 z = ((float) rand ()) / ((float) RAND_MAX) * 200;
		node = m_SceneManager->getRootSceneNode()->createChildSceneNode(
			StringConverter::toString(i), 
			reVector3Df(x, 0, z));

		entity = m_SceneManager->createEntity(StringConverter::toString(i), "crate.mesh");
		node->attachObject(entity);
		CGameObjectManager::Instance()->AddSphereObstacle(entity, node, true);
	}

	// setup player
	CCameraManager::Instance()->GetCamera()->setPosition(0, 100, 100);

	// Setup FrameListener
	m_FrameListenerFramework = new CFpsFrameListener(m_GameManager->GetRenderWindow());
	m_GameManager->GetEngineDevice()->addFrameListener(m_FrameListenerFramework);

	//m_FrameListenerFramework->AddTimer(10000, save);

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
