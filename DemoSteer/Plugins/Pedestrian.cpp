// An autonomous "pedestrian":
// follows paths, avoids collisions with obstacles and other pedestrians

#include <iomanip>
#include <sstream>
#include "PolylineSegmentedPathwaySingleRadius.h"
#include "CFpsVehicle.h"
#include "OpenSteerDemo.h"
#include "Proximity.h"
#include "Color.h"

namespace {

    using namespace Core::AI;


    // ----------------------------------------------------------------------------


    typedef AbstractProximityDatabase<AbstractVehicle*> ProximityDatabase;
    typedef AbstractTokenForProximityDatabase<AbstractVehicle*> ProximityToken;


    // ----------------------------------------------------------------------------


    // How many pedestrians to create when the plugin starts first?
    int const gPedestrianStartCount = 100;
    // creates a path for the PlugIn
    PolylineSegmentedPathwaySingleRadius* getTestPath (void);
    PolylineSegmentedPathwaySingleRadius* gTestPath = NULL;
    SphereObstacle gObstacle1;
    SphereObstacle gObstacle2;
    ObstacleGroup gObstacles;
    reVector3Df gEndpoint0;
    reVector3Df gEndpoint1;
    bool gUseDirectedPathFollowing = true;
    // ------------------------------------ xxxcwr11-1-04 fixing steerToAvoid
    RectangleObstacle gObstacle3 (7,7);
    // ------------------------------------ xxxcwr11-1-04 fixing steerToAvoid

    // this was added for debugging tool, but I might as well leave it in
    bool gWanderSwitch = true;


    // ----------------------------------------------------------------------------


    class Pedestrian : public CFpsVehicle
    {
    public:

        // type for a group of Pedestrians
        typedef std::vector<Pedestrian*> groupType;

        // constructor
        Pedestrian (ProximityDatabase& pd)
        {
            // allocate a token for this boid in the proximity database
            proximityToken = NULL;
            newPD (pd);

            // reset Pedestrian state
            reset ();
        }

        // destructor
        virtual ~Pedestrian ()
        {
            // delete this boid's token in the proximity database
            delete proximityToken;
        }

        // reset all instance state
        void reset (void)
        {
            // reset the vehicle 
            CFpsVehicle::reset ();

            // max speed and max steering force (maneuverability) 
            setMaxSpeed (2.0);
            setMaxForce (8.0);

            // initially stopped
            setSpeed (0);

            // size of bounding sphere, for obstacle avoidance, etc.
            setRadius (0.5); // width = 0.7, add 0.3 margin, take half

            // set the path for this Pedestrian to follow
            path = getTestPath ();

            // set initial position
            // (random point on path + random horizontal offset)
            const float d = path->length() * frandom01();
            const float r = path->radius();
            const reVector3Df randomOffset = Vec3Utilities::randomVectorOnUnitRadiusXZDisk () * r;
            setPosition (path->mapPathDistanceToPoint (d) + randomOffset);

            // randomize 2D heading
            RandomizeHeadingOnXZPlane ();

            // pick a random direction for path following (upstream or downstream)
            pathDirection = (frandom01() > 0.5) ? -1 : +1;

            // trail parameters: 3 seconds with 60 points along the trail
            setTrailParameters (3, 60);

            // notify proximity database that our position has changed
            proximityToken->updateForNewPosition (position());
        }

        // per frame simulation update
        void update (const float currentTime, const float elapsedTime)
        {
            // apply steering force to our momentum
            ApplySteeringForce (determineCombinedSteering (elapsedTime),
                                elapsedTime);

            // reverse direction when we reach an endpoint
            if (gUseDirectedPathFollowing)
            {
                const Color darkRed (0.7f, 0, 0);
                float const pathRadius = path->radius();
                
                if (Vec3Utilities::distance (position(), gEndpoint0) < pathRadius )
                {
                    pathDirection = +1;
                    annotationXZCircle (pathRadius, gEndpoint0, darkRed, 20);
                }
                if (Vec3Utilities::distance (position(), gEndpoint1) < pathRadius )
                {
                    pathDirection = -1;
                    annotationXZCircle (pathRadius, gEndpoint1, darkRed, 20);
                }
            }

            // annotation
            AnnotationVelocityAcceleration (5, 0);
            recordTrailVertex (currentTime, position());

            // notify proximity database that our position has changed
            proximityToken->updateForNewPosition (position());
        }

        // compute combined steering force: move forward, avoid obstacles
        // or neighbors if needed, otherwise follow the path and wander
        reVector3Df determineCombinedSteering (const float elapsedTime)
        {
            // move forward
            reVector3Df steeringForce = forward();

            // probability that a lower priority behavior will be given a
            // chance to "drive" even if a higher priority behavior might
            // otherwise be triggered.
            const float leakThrough = 0.1f;

            // determine if obstacle avoidance is required
            reVector3Df obstacleAvoidance;
            if (leakThrough < frandom01())
            {
                const float oTime = 6; // minTimeToCollision = 6 seconds
    // ------------------------------------ xxxcwr11-1-04 fixing steerToAvoid
    // just for testing
    //             obstacleAvoidance = steerToAvoidObstacles (oTime, gObstacles);
    //             obstacleAvoidance = steerToAvoidObstacle (oTime, gObstacle1);
    //             obstacleAvoidance = steerToAvoidObstacle (oTime, gObstacle3);
                obstacleAvoidance = steerToAvoidObstacles (oTime, gObstacles);
    // ------------------------------------ xxxcwr11-1-04 fixing steerToAvoid
            }

            // if obstacle avoidance is needed, do it
            if (obstacleAvoidance != reVector3Df::ZERO)
            {
                steeringForce += obstacleAvoidance;
            }
            else
            {
                // otherwise consider avoiding collisions with others
                reVector3Df collisionAvoidance;
                const float caLeadTime = 3;

                // find all neighbors within maxRadius using proximity database
                // (radius is largest distance between vehicles traveling head-on
                // where a collision is possible within caLeadTime seconds.)
                const float maxRadius = caLeadTime * maxSpeed() * 2;
                neighbors.clear();
                proximityToken->findNeighbors (position(), maxRadius, neighbors);

                if (leakThrough < frandom01())
                    collisionAvoidance =
                        steerToAvoidNeighbors (caLeadTime, neighbors) * 10;

                // if collision avoidance is needed, do it
                if (collisionAvoidance != reVector3Df::ZERO)
                {
                    steeringForce += collisionAvoidance;
                }
                else
                {
                    // add in wander component (according to user switch)
                    if (gWanderSwitch)
                        steeringForce += steerForWander (elapsedTime);

                    // do (interactively) selected type of path following
                    const float pfLeadTime = 3;
                    const reVector3Df pathFollow =
                        (gUseDirectedPathFollowing ?
                         steerToFollowPath (pathDirection, pfLeadTime, *path) :
                         steerToStayOnPath (pfLeadTime, *path));

                    // add in to steeringForce
                    steeringForce += pathFollow * 0.5;
                }
            }

            // return steering constrained to global XZ "ground" plane
			steeringForce.y = 0;
            return steeringForce;
        }


        // draw this pedestrian into scene
        void draw (void)
        {
            drawBasic2dCircularVehicle (*this, Color::gGray50);
            drawTrail ();
        }


        // called when steerToFollowPath decides steering is required
        void annotatePathFollowing (const reVector3Df& future,
                                    const reVector3Df& onPath,
                                    const reVector3Df& target,
                                    const float outside)
        {
            const Color yellow (1, 1, 0);
            const Color lightOrange (1.0f, 0.5f, 0.0f);
            const Color darkOrange  (0.6f, 0.3f, 0.0f);
            const Color yellowOrange (1.0f, 0.75f, 0.0f);

            // draw line from our position to our predicted future position
            annotationLine (position(), future, yellow);

            // draw line from our position to our steering target on the path
            annotationLine (position(), target, yellowOrange);

            // draw a two-toned line between the future test point and its
            // projection onto the path, the change from dark to light color
            // indicates the boundary of the tube.
            const reVector3Df boundaryOffset = Vec3Utilities::normalise(onPath - future) * outside;
            const reVector3Df onPathBoundary = future + boundaryOffset;
            annotationLine (onPath, onPathBoundary, darkOrange);
            annotationLine (onPathBoundary, future, lightOrange);
        }

        // called when steerToAvoidCloseNeighbors decides steering is required
        // (parameter names commented out to prevent compiler warning from "-W")
        void annotateAvoidCloseNeighbor (const AbstractVehicle& other,
                                         const float /*additionalDistance*/)
        {
            // draw the word "Ouch!" above colliding vehicles
            const float headOn = forward().dotProduct(other.forward()) < 0;
            const Color green (0.4f, 0.8f, 0.1f);
            const Color red (1, 0.1f, 0);
            const Color color = headOn ? red : green;
            const char* string = headOn ? "OUCH!" : "pardon me";
            const reVector3Df location = position() + reVector3Df (0, 0.5f, 0);
            if (Core::AI::annotationIsOn())
                draw2dTextAt3dLocation (*string, location, color, drawGetWindowWidth(), drawGetWindowHeight());
        }


        // (parameter names commented out to prevent compiler warning from "-W")
        void annotateAvoidNeighbor (const AbstractVehicle& threat,
                                    const float /*steer*/,
                                    const reVector3Df& ourFuture,
                                    const reVector3Df& threatFuture)
        {
            const Color green (0.15f, 0.6f, 0.0f);

            annotationLine (position(), ourFuture, green);
            annotationLine (threat.position(), threatFuture, green);
            annotationLine (ourFuture, threatFuture, Color::gRed);
            annotationXZCircle (radius(), ourFuture,    green, 12);
            annotationXZCircle (radius(), threatFuture, green, 12);
        }

        // xxx perhaps this should be a call to a general purpose annotation for
        // xxx "local xxx axis aligned box in XZ plane" -- same code in in
        // xxx CaptureTheFlag.cpp
        void annotateAvoidObstacle (const float minDistanceToCollision)
        {
            const reVector3Df boxSide = side() * radius();
            const reVector3Df boxFront = forward() * minDistanceToCollision;
            const reVector3Df FR = position() + boxFront - boxSide;
            const reVector3Df FL = position() + boxFront + boxSide;
            const reVector3Df BR = position()            - boxSide;
            const reVector3Df BL = position()            + boxSide;
            const Color white (1,1,1);
            annotationLine (FR, FL, white);
            annotationLine (FL, BL, white);
            annotationLine (BL, BR, white);
            annotationLine (BR, FR, white);
        }

        // switch to new proximity database -- just for demo purposes
        void newPD (ProximityDatabase& pd)
        {
            // delete this boid's token in the old proximity database
            delete proximityToken;

            // allocate a token for this boid in the proximity database
            proximityToken = pd.allocateToken (this);
        }

        // a pointer to this boid's interface object for the proximity database
        ProximityToken* proximityToken;

        // allocate one and share amoung instances just to save memory usage
        // (change to per-instance allocation to be more MP-safe)
        static AVGroup neighbors;

        // path to be followed by this pedestrian
        // XXX Ideally this should be a generic Pathway, but we use the
        // XXX getTotalPathLength and radius methods (currently defined only
        // XXX on PolylinePathway) to set random initial positions.  Could
        // XXX there be a "random position inside path" method on Pathway?
        PolylineSegmentedPathwaySingleRadius* path;

        // direction for path following (upstream or downstream)
        int pathDirection;
    };


    AVGroup Pedestrian::neighbors;


    // ----------------------------------------------------------------------------
    // create path for PlugIn 
    //
    //
    //        | gap |
    //
    //        f      b
    //        |\    /\        -
    //        | \  /  \       ^
    //        |  \/    \      |
    //        |  /\     \     |
    //        | /  \     c   top
    //        |/    \g  /     |
    //        /        /      |
    //       /|       /       V      z     y=0
    //      / |______/        -      ^
    //     /  e      d               |
    //   a/                          |
    //    |<---out-->|               o----> x
    //


    PolylineSegmentedPathwaySingleRadius* getTestPath (void)
    {
        if (gTestPath == NULL)
        {
            const float pathRadius = 2;

            const u32 pathPointCount = 7;
            const float size = 30;
            const float top = 2 * size;
            const float gap = 1.2f * size;
            const float out = 2 * size;
            const float h = 0.5;
            const reVector3Df pathPoints[pathPointCount] =
                {reVector3Df (h+gap-out,     0,  h+top-out),  // 0 a
                 reVector3Df (h+gap,         0,  h+top),      // 1 b
                 reVector3Df (h+gap+(top/2), 0,  h+top/2),    // 2 c
                 reVector3Df (h+gap,         0,  h),          // 3 d
                 reVector3Df (h,             0,  h),          // 4 e
                 reVector3Df (h,             0,  h+top),      // 5 f
                 reVector3Df (h+gap,         0,  h+top/2)};   // 6 g

            gObstacle1.setPosition(interpolate (0.2f, pathPoints[0], pathPoints[1]));
            gObstacle2.setPosition(interpolate (0.5f, pathPoints[2], pathPoints[3]));
            gObstacle1.radius = 3;
            gObstacle2.radius = 5;
            gObstacles.push_back (&gObstacle1);
            gObstacles.push_back (&gObstacle2);
    // ------------------------------------ xxxcwr11-1-04 fixing steerToAvoid

            gObstacles.push_back (&gObstacle3);

    //         // rotated to be perpendicular with path
    //         gObstacle3.setForward (1, 0, 0);
    //         gObstacle3.setSide (0, 0, 1);
    //         gObstacle3.setPosition (20, 0, h);

    //         // moved up to test off-center
    //         gObstacle3.setForward (1, 0, 0);
    //         gObstacle3.setSide (0, 0, 1);
    //         gObstacle3.setPosition (20, 3, h);

    //         // rotated 90 degrees around path to test other local axis
    //         gObstacle3.setForward (1, 0, 0);
    //         gObstacle3.setSide (0, -1, 0);
    //         gObstacle3.setUp (0, 0, -1);
    //         gObstacle3.setPosition (20, 0, h);

            // tilted 45 degrees
            gObstacle3.setForward (Vec3Utilities::normalise(reVector3Df(1,1,0)));
            gObstacle3.setSide (0,0,1);
            gObstacle3.setUp (Vec3Utilities::normalise(reVector3Df(-1,1,0)));
            gObstacle3.setPosition (20, 0, h);

    //         gObstacle3.setSeenFrom (Obstacle::outside);
    //         gObstacle3.setSeenFrom (Obstacle::inside);
            gObstacle3.setSeenFrom (Obstacle::both);

    // ------------------------------------ xxxcwr11-1-04 fixing steerToAvoid

            gEndpoint0 = pathPoints[0];
            gEndpoint1 = pathPoints[pathPointCount-1];

            gTestPath = new PolylineSegmentedPathwaySingleRadius (pathPointCount,
                                                                  pathPoints,
                                                                  pathRadius,
                                                                  false);
        }
        return gTestPath;
    }


    // ----------------------------------------------------------------------------
    // OpenSteerDemo PlugIn


    class PedestrianPlugIn : public PlugIn
    {
    public:

        const char* name (void) {return "Pedestrians";}

        float selectionOrderSortKey (void) {return 0.02f;}

        virtual ~PedestrianPlugIn() {}// be more "nice" to avoid a compiler warning

        void open (void)
        {
            // make the database used to accelerate proximity queries
            cyclePD = -1;
            nextPD ();

            // create the specified number of Pedestrians
            population = 0;
            for (int i = 0; i < gPedestrianStartCount; i++) addPedestrianToCrowd ();

            // initialize camera and selectedVehicle
            Pedestrian& firstPedestrian = **crowd.begin();
            OpenSteerDemo::init3dCamera (firstPedestrian);
			OpenSteerDemo::camera.mode = Core::AI::Camera::cmFixedDistanceOffset;
            OpenSteerDemo::camera.fixedTarget.set (15, 0, 30);
            OpenSteerDemo::camera.fixedPosition.set (15, 70, -70);
        }

        void update (const float currentTime, const float elapsedTime)
        {
            // update each Pedestrian
            for (iterator i = crowd.begin(); i != crowd.end(); i++)
            {
                (**i).update (currentTime, elapsedTime);
            }
        }

        void redraw (const float currentTime, const float elapsedTime)
        {
            // selected Pedestrian (user can mouse click to select another)
            AbstractVehicle& selected = *OpenSteerDemo::selectedVehicle;

            // Pedestrian nearest mouse (to be highlighted)
            AbstractVehicle& nearMouse = *OpenSteerDemo::vehicleNearestToMouse ();

            // update camera
            OpenSteerDemo::updateCamera (currentTime, elapsedTime, selected);

            // draw "ground plane"
            if (OpenSteerDemo::selectedVehicle) gridCenter = selected.position();
            OpenSteerDemo::gridUtility (gridCenter);

            // draw and annotate each Pedestrian
            for (iterator i = crowd.begin(); i != crowd.end(); i++) (**i).draw (); 

            // draw the path they follow and obstacles they avoid
            drawPathAndObstacles ();

            // highlight Pedestrian nearest mouse
            OpenSteerDemo::highlightVehicleUtility (nearMouse);

            // textual annotation (at the vehicle's screen position)
            serialNumberAnnotationUtility (selected, nearMouse);

            // textual annotation for selected Pedestrian
            if (OpenSteerDemo::selectedVehicle && Core::AI::annotationIsOn())
            {
                const Color color (0.8f, 0.8f, 1.0f);
                const reVector3Df textOffset (0, 0.25f, 0);
                const reVector3Df textPosition = selected.position() + textOffset;
                const reVector3Df camPosition = OpenSteerDemo::camera.position();
                const float camDistance = Vec3Utilities::distance (selected.position(),
                                                          camPosition);
                const char* spacer = "      ";
                std::ostringstream annote;
                annote << std::setprecision (2);
                annote << std::setiosflags (std::ios::fixed);
                annote << spacer << "1: speed: " << selected.speed() << std::endl;
                annote << std::setprecision (1);
                annote << spacer << "2: cam dist: " << camDistance << std::endl;
                annote << spacer << "3: no third thing" << std::ends;
                draw2dTextAt3dLocation (annote, textPosition, color, drawGetWindowWidth(), drawGetWindowHeight());
            }

            // display status in the upper left corner of the window
            std::ostringstream status;
            status << "[F1/F2] Crowd size: " << population;
            status << "\n[F3] PD type: ";
            switch (cyclePD)
            {
            case 0: status << "LQ bin lattice"; break;
            case 1: status << "brute force";    break;
            }
            status << "\n[F4] ";
            if (gUseDirectedPathFollowing)
                status << "Directed path following.";
            else
                status << "Stay on the path.";
            status << "\n[F5] Wander: ";
            if (gWanderSwitch) status << "yes"; else status << "no";
            status << std::endl;
            const float h = drawGetWindowHeight ();
            const reVector3Df screenLocation (10, h-50, 0);
            draw2dTextAt2dLocation (status, screenLocation, Color::gGray80, drawGetWindowWidth(), drawGetWindowHeight());
        }


        void serialNumberAnnotationUtility (const AbstractVehicle& selected,
                                            const AbstractVehicle& nearMouse)
        {
            // display a Pedestrian's serial number as a text label near its
            // screen position when it is near the selected vehicle or mouse.
            if (&selected && &nearMouse && Core::AI::annotationIsOn())
            {
                for (iterator i = crowd.begin(); i != crowd.end(); i++)
                {
                    AbstractVehicle* vehicle = *i;
                    const float nearDistance = 6;
                    const reVector3Df& vp = vehicle->position();
                    const reVector3Df& np = nearMouse.position();
                    if ((Vec3Utilities::distance (vp, selected.position()) < nearDistance)
                        ||
                        (&nearMouse && (Vec3Utilities::distance (vp, np) < nearDistance)))
                    {
                        std::ostringstream sn;
                        sn << "#"
                           << ((Pedestrian*)vehicle)->SerialNumber
                           << std::ends;
                        const Color textColor (0.8f, 1, 0.8f);
                        const reVector3Df textOffset (0, 0.25f, 0);
                        const reVector3Df textPos = vehicle->position() + textOffset;
                        draw2dTextAt3dLocation (sn, textPos, textColor, drawGetWindowWidth(), drawGetWindowHeight());
                    }
                }
            }
        }

        void drawPathAndObstacles (void)
        {
            typedef u32 u32;
            
            // draw a line along each segment of path
            const PolylineSegmentedPathwaySingleRadius& path = *getTestPath ();
            for (u32 i = 1; i < path.pointCount(); ++i ) {
                drawLine (path.point( i ), path.point( i-1) , Color::gRed);
            }
            
            // draw obstacles
            drawXZCircle (gObstacle1.radius, gObstacle1.position(), Color::gWhite, 40);
            drawXZCircle (gObstacle2.radius, gObstacle2.position(), Color::gWhite, 40);
    // ------------------------------------ xxxcwr11-1-04 fixing steerToAvoid
            {
                float w = gObstacle3.width * 0.5f;
                reVector3Df p = gObstacle3.position ();
                reVector3Df s = gObstacle3.side ();
                drawLine (p + (s * w), p + (s * -w), Color::gWhite);

                reVector3Df v1 = gObstacle3.globalizePosition (reVector3Df (w, w, 0));
                reVector3Df v2 = gObstacle3.globalizePosition (reVector3Df (-w, w, 0));
                reVector3Df v3 = gObstacle3.globalizePosition (reVector3Df (-w, -w, 0));
                reVector3Df v4 = gObstacle3.globalizePosition (reVector3Df (w, -w, 0));

                drawLine (v1, v2, Color::gWhite);
                drawLine (v2, v3, Color::gWhite);
                drawLine (v3, v4, Color::gWhite);
                drawLine (v4, v1, Color::gWhite);
            }
    // ------------------------------------ xxxcwr11-1-04 fixing steerToAvoid
        }

        void close (void)
        {
            // delete all Pedestrians
           while (population > 0) removePedestrianFromCrowd ();
        }

        void reset (void)
        {
            // reset each Pedestrian
            for (iterator i = crowd.begin(); i != crowd.end(); i++) (**i).reset ();

            // reset camera position
            OpenSteerDemo::position2dCamera (*OpenSteerDemo::selectedVehicle);

            // make camera jump immediately to new position
            OpenSteerDemo::camera.doNotSmoothNextMove ();
        }

        void handleFunctionKeys (int keyNumber)
        {
            switch (keyNumber)
            {
            case 1:  addPedestrianToCrowd ();                               break;
            case 2:  removePedestrianFromCrowd ();                          break;
            case 3:  nextPD ();                                             break;
            case 4: gUseDirectedPathFollowing = !gUseDirectedPathFollowing; break;
            case 5: gWanderSwitch = !gWanderSwitch;                         break;
            }
        }

        void printMiniHelpForFunctionKeys (void)
        {
            std::ostringstream message;
            message << "Function keys handled by ";
            message << '"' << name() << '"' << ':' << std::ends;
            OpenSteerDemo::printMessage (message);
            OpenSteerDemo::printMessage (message);
            OpenSteerDemo::printMessage ("  F1     add a pedestrian to the crowd.");
            OpenSteerDemo::printMessage ("  F2     remove a pedestrian from crowd.");
            OpenSteerDemo::printMessage ("  F3     use next proximity database.");
            OpenSteerDemo::printMessage ("  F4     toggle directed path follow.");
            OpenSteerDemo::printMessage ("  F5     toggle wander component on/off.");
            OpenSteerDemo::printMessage ("");
        }


        void addPedestrianToCrowd (void)
        {
            population++;
            Pedestrian* pedestrian = new Pedestrian (*pd);
            crowd.push_back (pedestrian);
            if (population == 1) OpenSteerDemo::selectedVehicle = pedestrian;
        }


        void removePedestrianFromCrowd (void)
        {
            if (population > 0)
            {
                // save pointer to last pedestrian, then remove it from the crowd
                const Pedestrian* pedestrian = crowd.back();
                crowd.pop_back();
                population--;

                // if it is OpenSteerDemo's selected vehicle, unselect it
                if (pedestrian == OpenSteerDemo::selectedVehicle)
                    OpenSteerDemo::selectedVehicle = NULL;

                // delete the Pedestrian
                delete pedestrian;
            }
        }


        // for purposes of demonstration, allow cycling through various
        // types of proximity databases.  this routine is called when the
        // OpenSteerDemo user pushes a function key.
        void nextPD (void)
        {
            // save pointer to old PD
            ProximityDatabase* oldPD = pd;

            // allocate new PD
            const int totalPD = 2;
            switch (cyclePD = (cyclePD + 1) % totalPD)
            {
            case 0:
                {
                    const reVector3Df center;
                    const float div = 20.0f;
                    const reVector3Df divisions (div, 1.0f, div);
                    const float diameter = 80.0f; //XXX need better way to get this
                    const reVector3Df dimensions (diameter, diameter, diameter);
                    typedef LQProximityDatabase<AbstractVehicle*> LQPDAV;
                    pd = new LQPDAV (center, dimensions, divisions);
                    break;
                }
            case 1:
                {
                    pd = new BruteForceProximityDatabase<AbstractVehicle*> ();
                    break;
                }
            }

            // switch each boid to new PD
            for (iterator i=crowd.begin(); i!=crowd.end(); i++) (**i).newPD(*pd);

            // delete old PD (if any)
            delete oldPD;
        }


        const AVGroup& allVehicles (void) {return (const AVGroup&) crowd;}

        // crowd: a group (STL vector) of all Pedestrians
        Pedestrian::groupType crowd;
        typedef Pedestrian::groupType::const_iterator iterator;

        reVector3Df gridCenter;

        // pointer to database used to accelerate proximity queries
        ProximityDatabase* pd;

        // keep track of current flock size
        int population;

        // which of the various proximity databases is currently in use
        int cyclePD;
    };


    PedestrianPlugIn gPedestrianPlugIn;




    // ----------------------------------------------------------------------------

} // anonymous namespace
