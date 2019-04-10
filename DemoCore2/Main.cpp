#include "CGame.h"
#include "CLevelGeneric.h"

#define WIN32_LEAN_AND_MEAN
#include "windows.h"

s32 main(s32 argc, char **argv);
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT argc) { return main(1, &strCmdLine); }

s32 main(s32 argc, char **argv)
{
	Core::CGame game;
    try 
	{
		CLevelGeneric* level = new CLevelGeneric();
		game.AddGameState(level);
		game.Run();
    } 
	catch( Ogre::Exception& e ) 
	{
		fprintf(stderr, "An exception has occurred: %s\n", e.what());
    }
    
    return 0;
}
