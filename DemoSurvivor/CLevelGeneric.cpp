#include "CLevelGeneric.h"
#include "CoreEngine.h"

#include "Structures.h"

using namespace Core;

Physics::IPhysicsStrategy* physics = 0;
IInventoryItem* item = 0;
CPlayer* player1;
CPlayer* player2;
CPlayer* player3;
CPlayer* player4;
CPlayer* player5;

bool CreatePlane()
{
	using namespace Core::Physics;
	IPhysicsStrategy* physics = CPhysicsManager::Instance()->GetStrategyByIndex(0);

	if(physics)
	{
		physics->AddInfinitePlane(NULL);
		if(!Nature::CNatureManager::Instance()->GetWorld())
		{
			Nature::NaturePackage* npk = new Nature::NaturePackage();
			npk->cam = CCameraManager::Instance()->GetCamera();
			npk->ed = CGameManager::Instance()->GetEngineDevice();
			npk->rw = CGameManager::Instance()->GetRenderWindow();
			npk->sm = CGameManager::Instance()->GetSceneManager();
			npk->vp = CCameraManager::Instance()->GetViewport();
			Nature::CNatureManager::Instance()->CreateWorld(npk, NULL, NULL, NULL);
		}
	}
	else
		return false;

	return true;
}

void CreateWeapon()
{
	//CWeapon* weapon = new CWeapon(physics);
	//weapon->Name = "some_weapon";
	//weapon->MeshName = "m4a1_1_test.mesh";
	//weapon->OverlayName = "Soldier/Target";
	//weapon->MeshScale = 0.1f;
	//weapon->Range = 300;
	//weapon->Load();

	//CProjectile* projectile = new CProjectile(physics);
	//projectile->Damage = 90;

	//CMagazine* mag = new CMagazine();
	//mag->Capacity = 15;
	//mag->Desc = "15 round clip";
	//mag->Name = "5.56mm";
	//mag->ProjectileName = "5.56mm";
	//mag->SetProjectile(projectile);

	//weapon->SetMagazine(mag);

	//CItemsManager::Instance()->AddItem(weapon);
	CItemsManager::Instance()->PlaceItem("m4_test", reVector3Df(0, 20, 0), physics);
	item = CItemsManager::Instance()->Pickup("m4_test");
}

void CLevelGeneric::Update(const f32& deltaTime)
{
	Physics::CPhysicsManager::Instance()->Update(deltaTime);
	CGameManager::Instance()->UpdateGame(deltaTime);
}

bool CLevelGeneric::Load()
{
	try{
		//CGameManager::Instance()->CreateScene("OctreeSceneManager", "main_scene");

		physics = Physics::CPhysicsManager::Instance()->CreateStrategy("physics");
		physics->InitWorld();
		
		SceneImporterPackage sip;
		sip.SoundManager = Sound::CSoundManager::Instance();
		sip.PhysicsStrategy = physics;
		sip.GameManager = CGameManager::Instance();
		sip.CameraManager = CCameraManager::Instance();
		sip.ItemsManager = CItemsManager::Instance();
		sip.GameObjectManager = CGameObjectManager::Instance();
		sip.NatureManager = Nature::CNatureManager::Instance();
		sip.ResourceGroupName = "CLevelGeneric";
		SceneLoader::CSceneImporter* sceneImporter = new SceneLoader::CSceneImporter(sip);
		sceneImporter->ImportScene("level.txt");
		physics->ShowDebug(true);
		
		CCameraManager::Instance()->SetCameraPos(reVector3Df(0, 100, 200));
		CCameraManager::Instance()->SetMoveSpeed(200.0f);
		
		player1 = CGameObjectManager::Instance()->CreatePlayer("SOLIDER1", EPT_TEAM_1, "player1", physics, false);
		player1->SetPlayerPosition(0, 0, 500);
		player1->DebugLoS(true);
		player1->SetDirection(reVector3Df::NEGATIVE_UNIT_Z);
		player1->SetCharacterState(EPS_FIND_ENEMY);
		
		player2 = CGameObjectManager::Instance()->CreatePlayer("ZOMBIE1", EPT_TEAM_2, "player2", physics, false);
		player2->SetPlayerPosition(5, 0, 5);
		//player2->DebugLoS();
		//player2->SetCharacterState(EPS_SEEK_ENEMY);
		
		player3 = CGameObjectManager::Instance()->CreatePlayer("ZOMBIE1", EPT_TEAM_2, "player3", physics, false);
		player3->SetPlayerPosition(50, 0, 5);
		//player3->DebugLoS();
		//player3->SetCharacterState(EPS_SEEK_ENEMY);
		
		player4 = CGameObjectManager::Instance()->CreatePlayer("ZOMBIE1", EPT_TEAM_2, "player4", physics, false);
		player4->SetPlayerPosition(100, 0, -200);
		//player4->DebugLoS();
		//player4->SetCharacterState(EPS_SEEK_ENEMY);
		
		//player5 = CGameObjectManager::Instance()->CreatePlayer("NINJA1", EPT_TEAM_2, "player5", physics, false);
		//player5->SetPlayerPosition(100, 0, 200);
		//player5->DebugLoS();
		//player5->SetCharacterState(EPS_FIND_ENEMY);
		
		CreateWeapon();
		player1->AddItem(item);
		
		CreatePlane();
		
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