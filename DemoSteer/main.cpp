#include "OpenSteerDemo.h"        // OpenSteerDemo application
#include "Draw.h"                 // OpenSteerDemo graphics

// To include EXIT_SUCCESS
#include <cstdlib>


int main (int argc, char **argv) 
{
    // initialize OpenSteerDemo application
    Core::AI::OpenSteerDemo::initialize ();

    // initialize graphics
    Core::AI::initializeGraphics (argc, argv);

    // run the main event processing loop
    Core::AI::runGraphics ();  
    return EXIT_SUCCESS;
}


// ------------------------------------------------------------------------
