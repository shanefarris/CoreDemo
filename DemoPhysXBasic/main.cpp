#include "windows.h"
#include "Example1.hpp" 
#include "Example2.hpp" 
#include "Example3.hpp" 
#include "Example4.hpp" 
#include "Example5.hpp" 
 
int main(int argc, char **argv)
{
    Example5 app;

    try 
	{
        app.go();
    } 
	catch(Exception& e) 
	{
        std::cerr << "An exception has occurred: " << e.getFullDescription();
    }

    return 0;
}
