#include "CLevelGeneric.h"
#include "CoreEngine.h"

#include "CRagdollBoneInformation.h"

using namespace Core;
using namespace Core::Physics;

Physics::IPhysicsStrategy* physics = 0;
Entity* ninjaEntity = 0;

void CLevelGeneric::Update(const f32& deltaTime)
{
	if(physics)
		physics->Update(deltaTime);

	CGameManager::Instance()->UpdateGame(deltaTime);
}

bool CLevelGeneric::Load()
{
	try{
		CGameManager::Instance()->CreateScene("OctreeSceneManager", "main_scene");

		CCameraManager::Instance()->SetCameraPos(reVector3Df(0, 60, 200));
		CCameraManager::Instance()->SetMoveSpeed(100.0f);
		
		// Create the phsics
		physics = Physics::CPhysicsManager::Instance()->CreateStrategy("physics", EPS_PHYSX);
		physics->InitWorld();
		physics->AddInfinitePlane(NULL);
		
		CPlayer* player = CGameObjectManager::Instance()->CreatePlayer("NINJA1", EPT_TEAM_1, 0, physics, false);
		player->SetCharacterState(EPS_DEAD);

		m_FrameListenerFramework = new CFpsFrameListener(m_GameManager->GetRenderWindow());
		m_GameManager->GetEngineDevice()->addFrameListener(m_FrameListenerFramework);


	}
	catch(char* e)
	{
		std::cerr << e << std::endl;
	}
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
		game.AddGameState(new CLevelGeneric());
		game.Run();
    } 
	catch( Exception& e ) 
	{
		fprintf(stderr, "An exception has occurred: %s\n", e.what());
    }
    
    return 0;
}