// ----------------------------------------------------------------------------
//
//
// One vehicle turning way: a (near) minimal OpenSteerDemo PlugIn

#include <iomanip>
#include <sstream>
#include "CFpsVehicle.h"
#include "OpenSteerDemo.h"
#include "Color.h"

namespace {

    using namespace Core::AI;


    // ----------------------------------------------------------------------------


    class OneTurning : public CFpsVehicle
    {
    public:

        // constructor
        OneTurning () {reset ();}

        // reset state
        void reset (void)
        {
            CFpsVehicle::reset (); // reset the vehicle 
            setSpeed (1.5f);         // speed along Forward direction.
            setMaxForce (0.3f);      // steering force is clipped to this magnitude
            setMaxSpeed (5);         // velocity is clipped to this magnitude
            clearTrailHistory ();    // prevent long streaks due to teleportation 
        }

        // per frame simulation update
        void update (const float currentTime, const float elapsedTime)
        {
            ApplySteeringForce (reVector3Df (-2, 0, -3), elapsedTime);
            AnnotationVelocityAcceleration ();
            recordTrailVertex (currentTime, position());
        }

        // draw this character/vehicle into the scene
        void draw (void)
        {
            drawBasic2dCircularVehicle (*this, Color::gGray50);
            drawTrail ();
        }
    };


    // ----------------------------------------------------------------------------
    // PlugIn for OpenSteerDemo


    class OneTurningPlugIn : public PlugIn
    {
    public:
        
        const char* name (void) {return "One Turning Away";}

        float selectionOrderSortKey (void) {return 0.06f;}

        // be more "nice" to avoid a compiler warning
        virtual ~OneTurningPlugIn() {}

        void open (void)
        {
            gOneTurning = new OneTurning;
            OpenSteerDemo::selectedVehicle = gOneTurning;
            theVehicle.push_back (gOneTurning);

            // initialize camera
            OpenSteerDemo::init2dCamera (*gOneTurning);
            OpenSteerDemo::camera.setPosition (10,
                                               OpenSteerDemo::camera2dElevation,
                                               10);
            OpenSteerDemo::camera.fixedPosition.set (40, 40, 40);
        }

        void update (const float currentTime, const float elapsedTime)
        {
            // update simulation of test vehicle
            gOneTurning->update (currentTime, elapsedTime);
        }

        void redraw (const float currentTime, const float elapsedTime)
        {
            // draw test vehicle
            gOneTurning->draw ();

            // textual annotation (following the test vehicle's screen position)
            std::ostringstream annote;
            annote << std::setprecision (2) << std::setiosflags (std::ios::fixed);
            annote << "      speed: " << gOneTurning->speed() << std::ends;
            draw2dTextAt3dLocation (annote, gOneTurning->position(), Color::gRed, drawGetWindowWidth(), drawGetWindowHeight());
            draw2dTextAt3dLocation (*"start", reVector3Df::ZERO, Color::gGreen, drawGetWindowWidth(), drawGetWindowHeight());

            // update camera, tracking test vehicle
            OpenSteerDemo::updateCamera (currentTime, elapsedTime, *gOneTurning);

            // draw "ground plane"
            OpenSteerDemo::gridUtility (gOneTurning->position());
        }

        void close (void)
        {
            theVehicle.clear ();
            delete (gOneTurning);
            gOneTurning = NULL;
        }

        void reset (void)
        {
            // reset vehicle
            gOneTurning->reset ();
        }

        const AVGroup& allVehicles (void) {return (const AVGroup&) theVehicle;}

        OneTurning* gOneTurning;
        std::vector<OneTurning*> theVehicle; // for allVehicles
    };


    OneTurningPlugIn gOneTurningPlugIn;




    // ----------------------------------------------------------------------------

} // anonymous namespace
