#include "CLevelGeneric.h"
#include "CoreEngine.h"
#include "SceneImporterPackage.h"

#include "RuntimeEditor\CRuntimeEditorManager.h"

#include "FileDatabase\CoreDal.h"

using namespace Core;
using namespace Core::Physics;

IPhysicsStrategy* physics = 0;

CLevelGeneric::CLevelGeneric() : CGameState()
{
}

CLevelGeneric::~CLevelGeneric()
{
}

void CLevelGeneric::Update(const f32& deltaTime)
{
	if(physics)
		physics->Update(deltaTime);

	CGameObjectManager::Instance()->Update(deltaTime);
	CCameraManager::Instance()->Update(deltaTime);
}

void CLevelGeneric::CreateSceneImporterPackage(SceneImporterPackage& sip)
{
	sip.SoundManager = Sound::CSoundManager::Instance();
	sip.PhysicsStrategy = physics;
	sip.GameManager = m_GameManager;
	sip.CameraManager = CCameraManager::Instance();
	sip.ItemsManager = CItemsManager::Instance();
	sip.GameObjectManager = CGameObjectManager::Instance();
	sip.NatureManager = Nature::CNatureManager::Instance();
	sip.ResourceGroupName = "CLevelGeneric";
}

void CLevelGeneric::End()
{
}

bool CLevelGeneric::Load()
{
	try{
		

		CGameManager::Instance()->CreateScene("OctreeSceneManager", "main_scene");

		// setup physics
		physics = CPhysicsManager::Instance()->CreateStrategy("physics");
		if(physics)
			physics->InitWorld();

		CCameraManager::Instance()->SetCameraPos(reVector3Df(0, 60, 200));
		CCameraManager::Instance()->SetMoveSpeed(60.0f);

		SceneImporterPackage sip;
		CreateSceneImporterPackage(sip);
		Editor::CRuntimeEditorManager::Instance()->StartEditor(sip);
		Editor::CRuntimeEditorManager::Instance()->CreatePlane();

		// Setup FrameListener
		//m_FrameListenerFramework = new CFpsFrameListener(m_GameManager->GetRenderWindow());
		//m_GameManager->GetEngineDevice()->addFrameListener(m_FrameListenerFramework);

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
	catch( Ogre::Exception& e ) 
	{
		fprintf(stderr, "An exception has occurred: %s\n", e.what());
    }
    
    return 0;
}