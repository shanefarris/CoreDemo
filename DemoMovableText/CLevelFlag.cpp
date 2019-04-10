#include "CLevelFlag.h"
#include "CPhysicsManager.h"
#include "CCharacterState.h"
#include "CGameObjectManager.h"
#include "CCameraManager.h"

using namespace Core;


CLevelFlag::CLevelFlag()
{
}

CLevelFlag::~CLevelFlag()
{
}

void CLevelFlag::Update(const f32& elapsedTime)
{
	CCameraManager::Instance()->Update(elapsedTime);
	CGameObjectManager::Instance()->Update(elapsedTime);
}

void CLevelFlag::End()
{
	m_GameManager->RemoveResourceArchive(".\\Media.zip", "OgreOde");
}

bool CLevelFlag::Load()
{
	// setup player
	CCameraManager::Instance()->CreateCamera(ECM_FREE);

	m_GameManager->AddResourceArchive(".\\Media.zip", "CLevelFlag");


    // Define a floor plane mesh
	Entity* entFloor;
    Plane p;
    p.normal = reVector3Df::UNIT_Y;
    p.d = 0;
    MeshManager::getSingleton().createPlane(
        "FloorPlane", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, 
        p, 10000, 10000, 10, 10, true, 1, 60, 60, reVector3Df::UNIT_Z);

    // Create an entity (the floor)
	entFloor = m_GameManager->GetSceneManager()->createEntity("floor", "FloorPlane");
    entFloor->setMaterialName("Scenario/Floor");

	// Attach to child of root node, better for culling (otherwise bounds are the combination of the 2)
    m_GameManager->GetSceneManager()->getRootSceneNode()->createChildSceneNode()->attachObject(entFloor);

	// Create some AI units
	for(int i = 0; i < 10; i++)
	{
		CPlayer* player = CGameObjectManager::Instance()->CreatePlayer(
			EPT_TEST1, 
			String("player_" + StringConverter::toString(i)).c_str(), 
			NULL);
		player->SetPlayerPosition(reVector3Df(i * 20, 2, i * 10));
		player->SetCharacterState(EPS_WANDERING);
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