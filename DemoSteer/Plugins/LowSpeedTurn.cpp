// "Low Speed Turn" test fixture
//
// Used to evaluate vehicle response at low speed to backward-directed
// steering force

#include <iomanip>
#include <sstream>
#include "CFpsVehicle.h"
#include "OpenSteerDemo.h"
#include "Color.h"

namespace {

    using namespace Core::AI;


    // ----------------------------------------------------------------------------


    class LowSpeedTurn : public CFpsVehicle
    {
    public:

        // constructor
        LowSpeedTurn () {reset ();}

        // reset state
        void reset (void)
        {
            // reset vehicle state
            CFpsVehicle::reset ();

            // speed along Forward direction.
            setSpeed (startSpeed);

            // initial position along X axis
            setPosition (startX, 0, 0);

            // steering force clip magnitude
            setMaxForce (0.3f);

            // velocity  clip magnitude
            setMaxSpeed (1.5f);

            // for next instance: step starting location
            startX += 2;

            // for next instance: step speed
            startSpeed += 0.15f;

            // 15 seconds and 150 points along the trail
            setTrailParameters (15, 150);
        }

        // draw into the scene
        void draw (void)
        {
            drawBasic2dCircularVehicle (*this, Color::gGray50);
            drawTrail ();
        }

        // per frame simulation update
        void update (const float currentTime, const float elapsedTime)
        {
            ApplySteeringForce (steering (), elapsedTime);

            // annotation
            AnnotationVelocityAcceleration ();
            recordTrailVertex (currentTime, position());
        }

        // reset starting positions
        static void resetStarts (void)
        {
            startX = 0;
            startSpeed = 0;
        }

        // constant steering force
        reVector3Df steering (void) {return reVector3Df (1, 0, -1);}

        // for stepping the starting conditions for next vehicle
        static float startX;
        static float startSpeed;
    };


    float LowSpeedTurn::startX;
    float LowSpeedTurn::startSpeed;


    // ----------------------------------------------------------------------------
    // PlugIn for OpenSteerDemo


    const int lstCount = 5;
    const float lstLookDownDistance = 18;
    const reVector3Df lstViewCenter (7, 0, -2);
    const reVector3Df lstPlusZ (0, 0, 1);


    class LowSpeedTurnPlugIn : public PlugIn
    {
    public:

        const char* name (void) {return "Low Speed Turn";}

        float selectionOrderSortKey (void) {return 0.05f;}

        // be more "nice" to avoid a compiler warning
        virtual ~LowSpeedTurnPlugIn() {}

        void open (void)
        {
            // create a given number of agents with stepped inital parameters,
            // store pointers to them in an array.
            LowSpeedTurn::resetStarts ();
            for (int i = 0; i < lstCount; i++) all.push_back (new LowSpeedTurn);

            // initial selected vehicle
            OpenSteerDemo::selectedVehicle = *all.begin();

            // initialize camera
            OpenSteerDemo::camera.mode = Core::AI::Camera::cmFixed;
            OpenSteerDemo::camera.fixedUp = lstPlusZ;
            OpenSteerDemo::camera.fixedTarget = lstViewCenter;
            OpenSteerDemo::camera.fixedPosition = lstViewCenter;
            OpenSteerDemo::camera.fixedPosition.y += lstLookDownDistance;
            OpenSteerDemo::camera.lookdownDistance = lstLookDownDistance;
            OpenSteerDemo::camera.fixedDistVOffset = OpenSteerDemo::camera2dElevation;
            OpenSteerDemo::camera.fixedDistDistance = OpenSteerDemo::cameraTargetDistance;
        }

        void update (const float currentTime, const float elapsedTime)
        {
            // update, draw and annotate each agent
            for (iterator i = all.begin(); i != all.end(); i++)
            {
                (**i).update (currentTime, elapsedTime);
            }
        }

        void redraw (const float currentTime, const float elapsedTime)
        {
            // selected vehicle (user can mouse click to select another)
            AbstractVehicle& selected = *OpenSteerDemo::selectedVehicle;

            // vehicle nearest mouse (to be highlighted)
            AbstractVehicle& nearMouse = *OpenSteerDemo::vehicleNearestToMouse ();

            // update camera
            OpenSteerDemo::updateCamera (currentTime, elapsedTime, selected);

            // draw "ground plane"
            OpenSteerDemo::gridUtility (selected.position());
          
            // update, draw and annotate each agent
            for (iterator i = all.begin(); i != all.end(); i++)
            {
                // draw this agent
                LowSpeedTurn& agent = **i;
                agent.draw ();

                // display speed near agent's screen position
                const Color textColor (0.8f, 0.8f, 1.0f);
                const reVector3Df textOffset (0, 0.25f, 0);
                const reVector3Df textPosition = agent.position() + textOffset;
                std::ostringstream annote;
                annote << std::setprecision (2)
                       << std::setiosflags (std::ios::fixed)
                       << agent.speed()
                       << std::ends;
                draw2dTextAt3dLocation (annote, textPosition, textColor, drawGetWindowWidth(), drawGetWindowHeight());
            }

            // highlight vehicle nearest mouse
            OpenSteerDemo::highlightVehicleUtility (nearMouse);
        }

        void close (void)
        {
            for (iterator i = all.begin(); i!=all.end(); i++) delete (*i);
            all.clear ();
        }

        void reset (void)
        {
            // reset each agent
            LowSpeedTurn::resetStarts ();
            for (iterator i = all.begin(); i!=all.end(); i++) (**i).reset();
        }

        const AVGroup& allVehicles (void) {return (const AVGroup&) all;}

        std::vector<LowSpeedTurn*> all; // for allVehicles
        typedef std::vector<LowSpeedTurn*>::const_iterator iterator;
    };


    LowSpeedTurnPlugIn gLowSpeedTurnPlugIn;



    // ----------------------------------------------------------------------------

} // anonymous namespace
