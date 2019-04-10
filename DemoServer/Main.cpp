#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#include <iostream>
#include <stdio.h>

#include "CNetworkManager.h"

using namespace Core::Network;

int main(int argc, char **argv)
{
    try 
	{
		CNetworkManager* manager = CNetworkManager::Instance();
		if(!manager->StartServer())
		{
			throw("Unable to start server.");
		}

		std::cout << "Server created." << std::endl;
		while(true) 
		{
			manager->Update();
			Sleep(1000);
			std::cout << ".";
		}
    } 
	catch( Ogre::Exception& e ) 
	{
		fprintf(stderr, "An exception has occurred: %s\n", e.what());
    }
    
    return 0;
}
