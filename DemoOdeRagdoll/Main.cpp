#ifdef COMPILE_ODE

#pragma lib("OgreOde.lib);

#include "CGame.h"
#include "LevelTerrain.h"

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#endif

int main(int argc, char **argv)
{
	Core::CGame app;
    try 
	{
		app.AddGameState(new LevelTerrain());
		app.Run();
    } 
	catch( Ogre::Exception& e ) 
	{
		fprintf(stderr, "An exception has occurred: %s\n", e.what());
    }
    
    return 0;
}

#else

void main() { }

#endif
