#include "CLevelGeneric.h"
#include "Methods.h"
#include "CoreEngine.h"
#include "CCustomFrameListener.hpp"

#include "CScrollingMessage.h"

IPhysicsStrategy* CLevelGeneric::physics = 0;
f32 CLevelGeneric::TimeUntilNextToggle = 0;
CGuiStrategy_MyGui* CLevelGeneric::Strategy = 0;
CCustomFrameListener* CLevelGeneric::FrameListenerFramework = 0;
CPlayer* CLevelGeneric::solider = 0;
CPlayer* CLevelGeneric::marine = 0;
CPlayer* CLevelGeneric::zombie = 0;
CPlayer* CLevelGeneric::robot = 0;

u32 EditorHelper::counter = 0;

void CLevelGeneric::Update(const f32& deltaTime)
{
	static bool visible = false;

	if(physics)
		physics->Update(deltaTime);

	TimeUntilNextToggle -= deltaTime;
	if(CInputManager::Instance()->GetKeyboard()->isKeyDown(OIS::KC_M) && 
		TimeUntilNextToggle <=0 && 
		!CGameManager::Instance()->GetConsoleVisible())
	{
		visible = !visible;
		TimeUntilNextToggle = 0.5;
	}

	page->SetVisible(visible);
	if(visible || CGameManager::Instance()->GetConsoleVisible())
		CCameraManager::Instance()->SetEnabled(false);
	else
		CCameraManager::Instance()->SetEnabled(true);

	CGameManager::Instance()->UpdateGame(deltaTime);

	//if(player1->CanSee(player2))
	//{
	//	CORE_TEXT("Player_Sight", "I see you");
	//}
	//else
	//{
	//	CORE_TEXT("Player_Sight", "Where are you?");
	//}

	//if(ragdoll)
	//	ragdoll->Update(deltaTime);
}

bool CLevelGeneric::Load()
{
	try{
		CGameManager::Instance()->CreateScene("OctreeSceneManager", "main_scene");
		CGameManager::Instance()->AddResourceArchive("./Media.zip", "CLevelGeneric");

		CGuiManager* manager = CGuiManager::Instance();
		Strategy = (CGuiStrategy_MyGui*)manager->CreateStrategy("MainMenu");
		EditorHelper::SetupPhysics(physics);
		EditorHelper::CreateFloorPlane(physics);

		CCameraManager::Instance()->CreateCamera(ECM_GOD);
		CCameraManager::Instance()->SetCameraPos(reVector3Df(0, 60, 200));
		CCameraManager::Instance()->SetMoveSpeed(60.0f);

		page = new CEditorMenu("Default.layout");

		// Setup FrameListener
		FrameListenerFramework = new CCustomFrameListener(m_GameManager->GetRenderWindow());
		FrameListenerFramework->AddGuiStrategy(Strategy);
		m_GameManager->GetEngineDevice()->addFrameListener(FrameListenerFramework);
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