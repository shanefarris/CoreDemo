#include "NaviDemo.h"
#include <windows.h>

int main()
{
	try
	{
		NaviDemo demo;

		while(!demo.shouldQuit)
			demo.Update();
	}
	catch(Ogre::Exception& e)
	{
		std::cout << e.getFullDescription().c_str() << std::endl;
	}

	return 0;
}