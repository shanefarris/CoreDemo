#include "CLevelSound.h"
#include "CCustomFpsFrameListener.h"
#include "CGameManager.h"

using namespace Core;

CLevelSound::CLevelSound()
{
}

CLevelSound::~CLevelSound()
{
}

void CLevelSound::Update(const f32& elapsedTime)
{
}

void CLevelSound::End()
{
	m_GameManager->RemoveResourceArchive(".\\Media.zip", "CLevelSound");
}

bool CLevelSound::Load()
{
	m_GameManager->AddResourceArchive(".\\Media.zip", "CLevelSound");

	std::cout << "**********************************************" << std::endl;
	std::cout << "Q - play siren" << std::endl;
	std::cout << "Y - stop siren" << std::endl;
	std::cout << "W - play theme" << std::endl;
	std::cout << "U - stop them" << std::endl;
	std::cout << "**********************************************" << std::endl;

	// Setup FrameListener
	m_FrameListenerFramework = new CCustomFpsFrameListener(m_GameManager->GetRenderWindow());
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
		game.AddGameState(new CLevelSound());
		game.Run();
    } 
	catch( Ogre::Exception& e ) 
	{
		fprintf(stderr, "An exception has occurred: %s\n", e.what());
    }
    
    return 0;
}