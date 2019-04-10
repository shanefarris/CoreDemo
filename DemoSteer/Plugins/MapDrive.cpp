// Driving through map-based obstacles    (OpenSteerDemo PlugIn)
//
// This demonstration is inspired by the DARPA Grand Challenge cross country
// race for autonomous vehicles (http://www.darpa.mil/grandchallenge/).  A
// route is defined as a series of (GPS) waypoints and a width associated
// with each segment between waypoints.  This demo assumes sensors on-board
// the vehicle create a binary map classifying the surrounding terrain into
// drivable and not drivable.  The vehicle tries to follow the route while
// avoiding obstacles and maximizing speed.  When the vehicle finds itself
// in danger of collision, it "gives up" (turns yellow) and slows to a stop.
// If it collides with an obstacle it turns red.  In both cases the
// simulation is restarted.  (This plug-in includes two non-path-following
// demos of map-based obstacle avoidance.  Use F1 to select among them.)
//
// 06-01-05 bknafla: exchanged GCRoute with PolylineSegmentedPathwaySegmentRadii
// 08-16-04 cwr: merge back into OpenSteer code base
// 10-15-03 cwr: created 
//
//
// ----------------------------------------------------------------------------


#include <iomanip>
#include <sstream>
#include <cassert>
#include "OpenSteerDemo.h"
#include "CFpsVehicle.h"
#include "Color.h"
#include "UnusedParameter.h"
#include "PolylineSegmentedPathwaySegmentRadii.h"
#include "QueryPathAlike.h"
#include "QueryPathAlikeUtilities.h"
#include "SegmentedPathAlikeUtilities.h"
#include "Utilities.h"
#include "Vec3Utilities.h"
#include "StandardTypes.h"



// to use local version of the map class
#define OLDTERRAINMAP
#ifndef OLDTERRAINMAP
#include "TerrainMap.h"
#endif

// ----------------------------------------------------------------------------


namespace {

    using namespace Core::AI;

    
    class PointToRadiusMapping : public Core::AI::DontExtractPathDistance {
    public:
        PointToRadiusMapping(): radius( 0.0f ) {}
        
        void setPointOnPathCenterLine( reVector3Df const& ) {}
        void setPointOnPathBoundary( reVector3Df const&  ) {}
        void setRadius( float r ) { radius = r; }
        void setTangent( reVector3Df const& ) {}
        void setSegmentIndex( u32 ) {}
        void setDistancePointToPath( float  ) {}
        void setDistancePointToPathCenterLine( float ) {}
        void setDistanceOnPath( float  ) {}
        void setDistanceOnSegment( float ) {}
        
        float radius;
    };
    
    
    
    class PointToTangentMapping : public Core::AI::DontExtractPathDistance {
    public:
        PointToTangentMapping() : tangent( reVector3Df( 0.0f, 0.0f, 0.0f ) ) {}
        
        void setPointOnPathCenterLine( reVector3Df const& ) {}
        void setPointOnPathBoundary( reVector3Df const&  ) {}
        void setRadius( float ) {}
        void setTangent( reVector3Df const& t ) { tangent = t; }
        void setSegmentIndex( u32 ) {}
        void setDistancePointToPath( float  ) {}
        void setDistancePointToPathCenterLine( float ) {}
        void setDistanceOnPath( float  ) {}
        void setDistanceOnSegment( float ) {}
        
        reVector3Df tangent;
    };


        
    
    class PointToPointOnCenterLineAndOutsideMapping : public Core::AI::DontExtractPathDistance {
    public:
        PointToPointOnCenterLineAndOutsideMapping() : pointOnPathCenterLine( reVector3Df( 0.0f, 0.0f, 0.0f ) ), distancePointToPathBoundary( 0.0f ) {}
        
        void setPointOnPathCenterLine( reVector3Df const& point) { pointOnPathCenterLine = point; }
        void setPointOnPathBoundary( reVector3Df const& ) {}
        void setRadius( float ) {}
        void setTangent( reVector3Df const& ) {}
        void setSegmentIndex( u32 ) {}
        void setDistancePointToPath( float d ) { distancePointToPathBoundary = d; }
        void setDistancePointToPathCenterLine( float ) {}
        void setDistanceOnPath( float  ) {}
        void setDistanceOnSegment( float ) {}    
        
        reVector3Df pointOnPathCenterLine;
        float distancePointToPathBoundary;
    };


        
    
    class PointToOutsideMapping : public Core::AI::DontExtractPathDistance {
    public:
        PointToOutsideMapping() : distancePointToPathBoundary( 0.0f ) {}
        
        void setPointOnPathCenterLine( reVector3Df const& ) {}
        void setPointOnPathBoundary( reVector3Df const&  ) {}
        void setRadius( float ) {}
        void setTangent( reVector3Df const& ) {}
        void setSegmentIndex( u32 ) {}
        void setDistancePointToPath( float d ) { distancePointToPathBoundary = d; }
        void setDistancePointToPathCenterLine( float ) {}
        void setDistanceOnPath( float  ) {}
        void setDistanceOnSegment( float ) {}    
        
        float distancePointToPathBoundary;
    };


    
    class PointToSegmentIndexMapping : public Core::AI::DontExtractPathDistance {
    public:
        PointToSegmentIndexMapping() : segmentIndex( 0 ) {}
        
        void setPointOnPathCenterLine( reVector3Df const& ) {}
        void setPointOnPathBoundary( reVector3Df const&  ) {}
        void setRadius( float ) {}
        void setTangent( reVector3Df const& ) {}
        void setSegmentIndex( u32 i ) { segmentIndex = i; }
        void setDistancePointToPath( float  ) {}
        void setDistancePointToPathCenterLine( float ) {}
        void setDistanceOnPath( float  ) {}
        void setDistanceOnSegment( float ) {}    
        
        u32 segmentIndex;
    };
    
    /**
     * Maps @a point to @a pathway and extracts the radius at the mapping point.
     */
    float mapPointToRadius( Core::AI::PolylineSegmentedPathwaySegmentRadii const& pathway, reVector3Df const& point ) {
        PointToRadiusMapping mapping;
        Core::AI::mapPointToPathAlike( pathway, point, mapping );
        return mapping.radius;
    }
    
    /**
     * Maps @a point to @a pathway and extracts the tangent at the mapping 
     * point.
     */
    reVector3Df mapPointToTangent( Core::AI::PolylineSegmentedPathwaySegmentRadii const& pathway, reVector3Df const& point ) {
        PointToTangentMapping mapping;
        Core::AI::mapPointToPathAlike( pathway, point, mapping );
        return mapping.tangent;
    }
    
    /**
     * Returns @c true if @a point is inside @a pathway segment @a segmentIndex.
     *
     * On point on the boundary isn't inside the pathway.
     */
    bool isInsidePathSegment( Core::AI::PolylineSegmentedPathwaySegmentRadii const& pathway,  
                              u32 segmentIndex, 
                              reVector3Df const& point ) {
        assert( pathway.isValid() && "pathway isn't valid." );
        assert( segmentIndex < pathway.segmentCount() && "segmentIndex out of range." );
        
        float const segmentDistance = pathway.mapPointToSegmentDistance( segmentIndex, point );
        reVector3Df const pointOnSegmentCenterLine = pathway.mapSegmentDistanceToPoint( segmentIndex, segmentDistance );
        float const segmentRadiusAtPoint = pathway.mapSegmentDistanceToRadius( segmentIndex, segmentDistance );
        
        float const distancePointToPointOnSegmentCenterLine = (point - pointOnSegmentCenterLine).length();
        
        return distancePointToPointOnSegmentCenterLine < segmentRadiusAtPoint;
    }
    
    
    /**
     * Maps the @a point to @a pathway and extracts the tangent at the mapping
     * point or of the next path segment as indicated by @a direction if the
     * mapping point is near a path defining point (waypoint).
     *
     * @param pathway Pathway to inspect.
     * @param point Point to map to @a pathway.
     * @param direction Follow the path in path direction (@c 1) or in reverse
     *                  direction ( @c -1 ).
     */
    reVector3Df mapPointAndDirectionToTangent( 
		Core::AI::PolylineSegmentedPathwaySegmentRadii const& pathway, reVector3Df const& point, int direction ) 
	{
        assert( ( ( 1 == direction ) || ( -1 == direction ) ) && "direction must be 1 or -1." );
        
        PointToSegmentIndexMapping mapping;
        Core::AI::mapPointToPathAlike( pathway, point, mapping );
        u32 segmentIndex = mapping.segmentIndex;
        u32 nextSegmentIndex = segmentIndex;
        if ( 0 < direction ) {
            nextSegmentIndex = Core::AI::nextSegment( pathway, segmentIndex );
        } else {
            nextSegmentIndex = Core::AI::previousSegment( pathway, segmentIndex );
        }

        if ( isInsidePathSegment( pathway, nextSegmentIndex, point ) ) {
            segmentIndex = nextSegmentIndex;
        }
        
        // To save calculations to gather the tangent in a sound way the fact is
        // used that a polyline segmented pathway has the same tangent for a
        // whole segment.
        return pathway.mapSegmentDistanceToTangent( segmentIndex, 0.0f ) * static_cast< float >( direction );
        
        /*
         const int segmentIndex = indexOfNearestSegment (point);
         const int nextIndex = segmentIndex + pathFollowDirection;
         const bool insideNextSegment = isInsidePathSegment (point, nextIndex);
         const int i = (segmentIndex +
                     (insideNextSegment ? pathFollowDirection : 0));
         return normals [i] * (float)pathFollowDirection;
        */
    }
    
    /**
     * Returns @c true if @a point is near a waypoint of @a pathway.
     *
     * It is near if its distance to a waypoint of the path is lesser than the
     * radius of one of the segments that the waypoint belongs to.
     *
     * On point on the boundary isn't inside the pathway.
     */
    bool isNearWaypoint( Core::AI::PolylineSegmentedPathwaySegmentRadii const& pathway, reVector3Df const& point ) {
        assert( pathway.isValid() && "pathway must be valid." );
        
        u32 pointIndex = 0;
        
        // Test first waypoint.
        reVector3Df pointPathwayPointVector = point - pathway.point( pointIndex );
        float pointPathwayPointDistance = pointPathwayPointVector.dotProduct( pointPathwayPointVector );
        if ( pointPathwayPointDistance < Core::AI::square( pathway.segmentRadius( pointIndex ) ) ) {
            return true;
        }
        
        // Test other waypoints.
        u32 const maxInnerPointIndex = pathway.pointCount() - 2;
        for ( pointIndex = 1; pointIndex <= maxInnerPointIndex; ++pointIndex ) {
            pointPathwayPointVector = point - pathway.point( pointIndex );
            pointPathwayPointDistance = pointPathwayPointVector.dotProduct( pointPathwayPointVector );
            if ( ( pointPathwayPointDistance < Core::AI::square( pathway.segmentRadius( pointIndex ) ) ) ||
                 ( pointPathwayPointDistance < Core::AI::square( pathway.segmentRadius( pointIndex - 1) ) ) ) {
                return true;
            }
        }

        // Test last waypoint.
        pointPathwayPointVector = point - pathway.point( pointIndex );
        pointPathwayPointDistance = pointPathwayPointVector.dotProduct( pointPathwayPointVector );
        if ( pointPathwayPointDistance < Core::AI::square( pathway.segmentRadius( pointIndex - 1 ) ) ) {
            return true;
        }
        
        
        return false;
        
        /*
         // loop over all waypoints
         for (int i = 1; i < pointCount; i++)
         {
             // return true if near enough to this waypoint
             const float r = maxXXX (radii[i], radii[i+1]);
             const float d = (point - points[i]).length ();
             if (d < r) return true;
         }
         return false;
        */
    }
    
    /**
     * Maps @a point to @a pathway and returns the mapping point on the pathway 
     * boundary and how far outside @a point is from the mapping point.
     */
    reVector3Df mapPointToPointOnCenterLineAndOutside( Core::AI::PolylineSegmentedPathwaySegmentRadii const& pathway, reVector3Df const& point, float& outside ) {
        PointToPointOnCenterLineAndOutsideMapping mapping;
        Core::AI::mapPointToPathAlike( pathway, point, mapping );
        outside = mapping.distancePointToPathBoundary;
        return mapping.pointOnPathCenterLine;
    }
    
    
    /**
     * Maps @a point to @a pathway and returns how far outside @a point is from 
     * the mapping point on the path boundary.
     */
    float mapPointToOutside( Core::AI::PolylineSegmentedPathwaySegmentRadii const& pathway, reVector3Df const& point ) {
        PointToOutsideMapping mapping;
        Core::AI::mapPointToPathAlike( pathway, point, mapping);
        return mapping.distancePointToPathBoundary;    
    }
    
    /**
     * Returns @c true if @a point is inside @a pathway, @c false otherwise.
     * A point on the boundary isn't inside the pathway.
     */
    bool isInsidePathway( Core::AI::PolylineSegmentedPathwaySegmentRadii const& pathway, reVector3Df const& point ) {
        return 0.0f > mapPointToOutside( pathway, point );
    }
    

    u32 mapPointToSegmentIndex(  Core::AI::PolylineSegmentedPathwaySegmentRadii const& pathway, reVector3Df const& point ) 
	{
        PointToSegmentIndexMapping mapping;
        Core::AI::mapPointToPathAlike( pathway, point, mapping );
        return mapping.segmentIndex;
    }
    

    #ifdef OLDTERRAINMAP
    // class BinaryTerrainMap : public TerrainMap
    class TerrainMap
    {
    public:

        // constructor
        TerrainMap (const reVector3Df& c, float x, float z, int r)
            : center(c),
              xSize(x),
              zSize(z),
              resolution(r),
              outsideValue (false),
			  map(resolution * resolution)
        {
            map.reserve (resolution * resolution);
        }

        // destructor
        ~TerrainMap ()
        {
        }

        // clear the map (to false)
        void clear (void)
        {
            for (int i = 0; i < resolution; i++)
                for (int j = 0; j < resolution; j++)
                    setMapBit (i, j, 0);
        }


        // get and set a bit based on 2d integer map index
        bool getMapBit (int i, int j) const
        {
            return map[mapAddress(i, j)];
        }

        bool setMapBit (int i, int j, bool value)
        {
            return map[mapAddress(i, j)] = value;
        }


        // get a value based on a position in 3d world space
        bool getMapValue (const reVector3Df& point) const
        {
            const reVector3Df local = point - center;
            const reVector3Df localXZ = Vec3Utilities::setYtoZero(local);

            const float hxs = xSize/2;
            const float hzs = zSize/2;

            const float x = localXZ.x;
            const float z = localXZ.z;

            const bool out = (x > +hxs) || (x < -hxs) || (z > +hzs) || (z < -hzs);

            if (out) 
            {
                return outsideValue;
            }
            else
            {
                const float r = (float) resolution; // prevent VC7.1 warning
                const int i = (int) remapInterval (x, -hxs, hxs, 0.0f, r);
                const int j = (int) remapInterval (z, -hzs, hzs, 0.0f, r);
                return getMapBit (i, j);
            }
        }


        void xxxDrawMap (void)
        {
            const float xs = xSize/(float)resolution;
            const float zs = zSize/(float)resolution;
            const reVector3Df alongRow (xs, 0, 0);
            const reVector3Df nextRow (-xSize, 0, zs);
            reVector3Df g ((xSize - xs) / -2, 0, (zSize - zs) / -2);
            g += center;
            for (int j = 0; j < resolution; j++)
            {
                for (int i = 0; i < resolution; i++)
                {
                    if (getMapBit (i, j))
                    {
                        // spikes
                        // const reVector3Df spikeTop (0, 5.0f, 0);
                        // drawLine (g, g+spikeTop, Color::gWhite);

                        // squares
                        const float rockHeight = 0;
                        const reVector3Df v1 (+xs/2, rockHeight, +zs/2);
                        const reVector3Df v2 (+xs/2, rockHeight, -zs/2);
                        const reVector3Df v3 (-xs/2, rockHeight, -zs/2);
                        const reVector3Df v4 (-xs/2, rockHeight, +zs/2);
                        // const reVector3Df redRockColor (0.6f, 0.1f, 0.0f);
                        const Color orangeRockColor (0.5f, 0.2f, 0.0f);
                        drawQuadrangle (g+v1, g+v2, g+v3, g+v4, orangeRockColor);

                        // pyramids
                        // const reVector3Df top (0, xs/2, 0);
                        // const reVector3Df redRockColor (0.6f, 0.1f, 0.0f);
                        // const reVector3Df orangeRockColor (0.5f, 0.2f, 0.0f);
                        // drawTriangle (g+v1, g+v2, g+top, redRockColor);
                        // drawTriangle (g+v2, g+v3, g+top, orangeRockColor);
                        // drawTriangle (g+v3, g+v4, g+top, redRockColor);
                        // drawTriangle (g+v4, g+v1, g+top, orangeRockColor);
                    } 
                    g += alongRow;
                }
                g += nextRow;
            }
        }


        float minSpacing (void) const
        {
            return minXXX (xSize, zSize) / (float)resolution;
        }

        // used to detect if vehicle body is on any obstacles
        bool scanLocalXZRectangle (const AbstractLocalSpace& localSpace,
                                   float xMin, float xMax,
                                   float zMin, float zMax) const
        {
            const float spacing = minSpacing() / 2;

            for (float x = xMin; x < xMax; x += spacing)
            {
                for (float z = zMin; z < zMax; z += spacing)
                {
                    const reVector3Df sample (x, 0, z);
                    const reVector3Df global = localSpace.globalizePosition (sample);
                    if (getMapValue (global)) return true;
                }
            }
            return false;
        }

        // Scans along a ray (directed line segment) on the XZ plane, sampling
        // the map for a "true" cell.  Returns the index of the first sample
        // that gets a "hit", or zero if no hits found.
        int scanXZray (const reVector3Df& origin,
                       const reVector3Df& sampleSpacing,
                       const int sampleCount) const
        {
            reVector3Df samplePoint (origin);

            for (int i = 1; i <= sampleCount; i++)
            {
                samplePoint += sampleSpacing;
                if (getMapValue (samplePoint)) return i;
            }

            return 0;
        }


        int cellwidth (void) const {return resolution;}  // xxx cwr
        int cellheight (void) const {return resolution;}  // xxx cwr
        bool isPassable (const reVector3Df& point) const {return ! getMapValue (point);}


        reVector3Df center;
        float xSize;
        float zSize;
        int resolution;

        bool outsideValue;

    private:

        int mapAddress (int i, int j) const {return i + (j * resolution);}

        std::vector<bool> map;
    };
    #endif


    typedef PolylineSegmentedPathwaySegmentRadii GCRoute;



    // ----------------------------------------------------------------------------


    class MapDriver : public CFpsVehicle
    {
    public:

        // constructor
        MapDriver () : map (makeMap ()), path (makePath ())
        {
            reset ();

            // to compute mean time between collisions
            sumOfCollisionFreeTimes = 0;
            countOfCollisionFreeTimes = 0;

            // keep track for reliability statistics
            collisionLastTime = false;
            timeOfLastCollision = OpenSteerDemo::clock.getTotalSimulationTime ();

            // keep track of average speed
            totalDistance = 0;
            totalTime = 0;

            // keep track of path following failure rate
            pathFollowTime = 0;
            pathFollowOffTime = 0;

            // innitialize counters for various performance data
            stuckCount = 0;
            stuckCycleCount = 0;
            stuckOffPathCount = 0;
            lapsStarted = 0;
            lapsFinished = 0;
            hintGivenCount = 0;
            hintTakenCount = 0;

            // follow the path "upstream or downstream" (+1/-1)
            pathFollowDirection = 1;

            // use curved prediction and incremental steering:
            curvedSteering = true;
            incrementalSteering = true;

            // 10 seconds with 200 points along the trail
            setTrailParameters (10, 200);
        }

        // destructor
        ~MapDriver ()
        {
            delete (map);
            delete (path);
        }

        // reset state
        void reset (void)
        {
            // reset the underlying vehicle class
            CFpsVehicle::reset ();

            // initially stopped
            setSpeed (0);

            // Assume top speed is 20 meters per second (44.7 miles per hour).
            // This value will eventually be supplied by a higher level module.
            setMaxSpeed (20);

            // steering force is clipped to this magnitude
            setMaxForce (maxSpeed () * 0.4f);

            // vehicle is 2 meters wide and 3 meters long
            halfWidth = 1.0f;
            halfLength = 1.5f;

            // init dynamically controlled radius
            adjustVehicleRadiusForSpeed ();

            // not previously avoiding
            annotateAvoid = reVector3Df::ZERO;

            // prevent long streaks due to teleportation 
            clearTrailHistory ();

            // first pass at detecting "stuck" state
            stuck = false;

            // QQQ need to clean up this hack
            qqqLastNearestObstacle = reVector3Df::ZERO;

            // master look ahead (prediction) time
            baseLookAheadTime = 3;

            if (demoSelect == 2)
            {
                lapsStarted++;
                const float s = worldSize;
                const float d = (float) pathFollowDirection;
                setPosition (reVector3Df (s * d * 0.6f, 0, s * -0.4f));
				regenerateOrthonormalBasisUF (reVector3Df::NEGATIVE_UNIT_X * d);
            }

            // reset bookeeping to detect stuck cycles
            resetStuckCycleDetection ();

            // assume no previous steering
            currentSteering = reVector3Df::ZERO;

            // assume normal running state
            dtZero = false;

            // QQQ temporary global QQQoaJustScraping
            QQQoaJustScraping = false;

            // state saved for speedometer
    //      annoteMaxRelSpeed = annoteMaxRelSpeedCurve = annoteMaxRelSpeedPath = 0;
    //      annoteMaxRelSpeed = annoteMaxRelSpeedCurve = annoteMaxRelSpeedPath = 1;
        }


        // per frame simulation update
        void update (const float currentTime, const float elapsedTime)
        {
            // take note when current dt is zero (as in paused) for stat counters
            dtZero = (elapsedTime == 0);

            // pretend we are bigger when going fast
            adjustVehicleRadiusForSpeed ();

            // state saved for speedometer
    //      annoteMaxRelSpeed = annoteMaxRelSpeedCurve = annoteMaxRelSpeedPath = 0;
            annoteMaxRelSpeed = annoteMaxRelSpeedCurve = annoteMaxRelSpeedPath = 1;

            // determine combined steering
            reVector3Df steering;
            const bool offPath = !bodyInsidePath ();
            if (stuck || offPath || detectImminentCollision ())
            {
                // bring vehicle to a stop if we are stuck (newly or previously
                // stuck, because off path or collision seemed imminent)
                // (QQQ combine with stuckCycleCount code at end of this function?)
    //          applyBrakingForce (curvedSteering ? 3 : 2, elapsedTime); // QQQ
                ApplyBrakingForce ((curvedSteering?3.0f:2.0f), elapsedTime); // QQQ
                // count "off path" events
                if (offPath && !stuck && (demoSelect == 2)) 
					stuckOffPathCount++;
                stuck = true;

                // QQQ trying to prevent "creep" during emergency stops
                ResetSmoothedAcceleration ();
                currentSteering = reVector3Df::ZERO;
            }
            else
            {
                // determine steering for obstacle avoidance (save for annotation)
                const reVector3Df avoid = annotateAvoid = 
                    steerToAvoidObstaclesOnMap (lookAheadTimeOA (),
                                                *map,
                                                hintForObstacleAvoidance ());
                const bool needToAvoid = avoid != reVector3Df::ZERO;

                // any obstacles to avoid?
                if (needToAvoid)
                {
                    // slow down and turn to avoid the obstacles
                    const float targetSpeed =((curvedSteering && QQQoaJustScraping)
                                              ? maxSpeedForCurvature () : 0);
                    annoteMaxRelSpeed = targetSpeed / maxSpeed ();
                    const float avoidWeight = 3 + (3 * GetRelativeSpeed ()); // ad hoc
                    steering = avoid * avoidWeight;
                    steering += steerForTargetSpeed (targetSpeed);
                }
                else
                {
                    // otherwise speed up and...
                    steering = steerForTargetSpeed (maxSpeedForCurvature ());

                    // wander for demo 1
                    if (demoSelect == 1)
                    {
                        const reVector3Df wander = steerForWander (elapsedTime);
                        const reVector3Df flat = Vec3Utilities::setYtoZero(wander);
                        const reVector3Df weighted = flat.truncateLength (maxForce()) * 6;
                        const reVector3Df a = position() + reVector3Df (0, 0.2f, 0);
                        annotationLine (a, a + (weighted * 0.3f), Color::gWhite);
                        steering += weighted;
                    }

                    // follow the path in demo 2
                    if (demoSelect == 2)
                    {
                        const reVector3Df pf = steerToFollowPath (pathFollowDirection,
                                                           lookAheadTimePF (),
                                                           *path);
                        if (pf != reVector3Df::ZERO)
                        {
                            // steer to remain on path
                            if (pf.dotProduct (forward()) < 0)
                                steering = pf;
                            else
                                steering = pf + steering;
                        }
                        else
                        {
                            // path aligment: when neither obstacle avoidance nor
                            // path following is required, align with path segment
                            const reVector3Df pathHeading = mapPointAndDirectionToTangent( *path, position(), pathFollowDirection ); // path->tangentAt (position (), pathFollowDirection);
                            {
                                const reVector3Df b = (position () +
                                                (up () * 0.2f) +
                                                (forward () * halfLength * 1.4f));
                                const float l = 2;
                                annotationLine (b, b + (forward ()  * l), Color::gCyan);
                                annotationLine (b, b + (pathHeading * l), Color::gCyan);
                            }
                            steering += (steerTowardHeading(pathHeading) *
                                         ( isNearWaypoint( *path, position() ) /* path->nearWaypoint (position () ) */ ?
                                          0.5f : 0.1f));
                        }
                    }
                }
            }

            if (!stuck)
            {
                // convert from absolute to incremental steering signal
                if (incrementalSteering)
                    steering = convertAbsoluteToIncrementalSteering (steering,
                                                                     elapsedTime);
                // enforce minimum turning radius
                steering = adjustSteeringForMinimumTurningRadius (steering);
            }

            // apply selected steering force to vehicle, record data
            ApplySteeringForce (steering, elapsedTime);
            collectReliabilityStatistics (currentTime, elapsedTime);

            // detect getting stuck in cycles -- we are moving but not
            // making progress down the route (annotate smoothedPosition)
            if (demoSelect == 2)
            {
                const bool circles = weAreGoingInCircles ();
                if (circles && !stuck) stuckCycleCount++;
                if (circles) stuck = true;
                annotationCircleOrDisk (0.5, up(), GetSmoothedPosition (), Color::gWhite, 12, circles, false);
            }

            // annotation
            perFrameAnnotation ();
            recordTrailVertex (currentTime, position());
        }


    //  // QQQ 5-8-04 random experiment, currently unused
    //  //
    //  // reduce lateral steering at low speeds
    //  //
    //  reVector3Df reduceTurningAtLowSpeeds (const reVector3Df& rawSteering)
    //  {
    //      const reVector3Df thrust = rawSteering.parallelComponent (forward ());
    //      const reVector3Df lateral = rawSteering.perpendicularComponent (forward ());
    //      // const float adjust = RelativeSpeed ();
    //      // const float adjust = square (RelativeSpeed ());
    //      const float adjust = square (square (RelativeSpeed ()));
    //      return thrust + (lateral * adjust);
    //  }


        void adjustVehicleRadiusForSpeed (void)
        {
            const float minRadius = sqrtXXX(square(halfWidth)+square(halfLength));
            const float safetyMargin = (curvedSteering ?
                                        interpolate (GetRelativeSpeed(), 0.0f, 1.5f) :
                                        0.0f);
            setRadius (minRadius + safetyMargin);
        }


        void collectReliabilityStatistics (const float currentTime,
                                           const float elapsedTime)
        {
            // detect obstacle avoidance failure and keep statistics
            collisionDetected = map->scanLocalXZRectangle (*this,
                                                           -halfWidth, halfWidth,
                                                           -halfLength,halfLength);

            // record stats to compute mean time between collisions
            const float timeSinceLastCollision = currentTime - timeOfLastCollision;
            if (collisionDetected &&
                !collisionLastTime &&
                (timeSinceLastCollision > 1))
            {
                std::ostringstream message;
                message << "collision after "<<timeSinceLastCollision<<" seconds";
                OpenSteerDemo::printMessage (message);
                sumOfCollisionFreeTimes += timeSinceLastCollision;
                countOfCollisionFreeTimes++;
                timeOfLastCollision = currentTime;
            }
            collisionLastTime = collisionDetected;

            // keep track of average speed
            totalDistance += speed () * elapsedTime;
            totalTime += elapsedTime;

            // keep track of path following failure rate
            // QQQ for now, duplicating this code from the draw method:
            // if we are following a path but are off the path,
            // draw a red line to where we should be
            if (demoSelect == 2)
            {
                pathFollowTime += elapsedTime;
                if (! bodyInsidePath ()) pathFollowOffTime += elapsedTime;
            }
        }


        reVector3Df hintForObstacleAvoidance (void)
        {
            // used only when path following, return zero ("no hint") otherwise
            if (demoSelect != 2) return reVector3Df::ZERO;

            // are we heading roughly parallel to the current path segment?
            const reVector3Df p = position ();
            const reVector3Df pathHeading = mapPointAndDirectionToTangent( *path, p, pathFollowDirection ); // path->tangentAt (p, pathFollowDirection);
            if (pathHeading.dotProduct (forward ()) < 0.8f)
            {
                // if not, the "hint" is to turn to align with path heading
                const reVector3Df s = side () * halfWidth;
                const float f = halfLength * 2;
                annotationLine (p + s, p + s + (forward () * f), Color::gBlack);
                annotationLine (p - s, p - s + (forward () * f), Color::gBlack);
                annotationLine (p, p + (pathHeading * 5), Color::gMagenta);
                return pathHeading;
            }
            else
            {
                // when there is a valid nearest obstacle position
                const reVector3Df obstacle = qqqLastNearestObstacle;
                const reVector3Df o = obstacle + (up () * 0.1f);
                if (obstacle != reVector3Df::ZERO)
                {
                    // get offset, distance from obstacle to its image on path
                    float outside;
                    const reVector3Df onPath = mapPointToPointOnCenterLineAndOutside( *path, obstacle, outside );// path->mapPointToPath (obstacle, outside);
                    const reVector3Df offset = onPath - obstacle;
                    const float offsetDistance = offset.length();

                    // when the obstacle is inside the path tube
                    if (outside < 0)
                    {
                        // when near the outer edge of a sufficiently wide tube
                        // const int segmentIndex = path->indexOfNearestSegment (onPath);
                        // const float segmentRadius = path->segmentRadius( segmentIndex );
                        float const segmentRadius = mapPointToRadius( *path, onPath );
                        const float w = halfWidth * 6;
                        const bool nearEdge = offsetDistance > w;
                        const bool wideEnough = segmentRadius > (w * 2);
                        if (nearEdge && wideEnough)
                        {
                            const float obstacleDistance = (obstacle - p).length();
                            const float range = speed () * lookAheadTimeOA ();
                            const float farThreshold = range * 0.8f;
                            const bool usableHint = obstacleDistance>farThreshold;
                            if (usableHint)
                            {
								const reVector3Df q = p + (Vec3Utilities::normalise(offset) * 5);
                                annotationLine (p, q, Color::gMagenta);
                                annotationCircleOrDisk (0.4f, up(), o, Color::gWhite, 12, false, false);
                                return offset;
                            }
                        }
                    }
                    annotationCircleOrDisk (0.4f, up(), o, Color::gBlack, 12,false,false);
                }
            }
            // otherwise, no hint
            return reVector3Df::ZERO;
        }


        // like steerToAvoidObstacles, but based on a BinaryTerrainMap indicating
        // the possitions of impassible regions
        //
        reVector3Df steerToAvoidObstaclesOnMap (const float minTimeToCollision,
                                         const TerrainMap& map)
        {
            return steerToAvoidObstaclesOnMap (minTimeToCollision,
                                               map,
                                               reVector3Df::ZERO); // no steer hint
        }


        // given a map of obstacles (currently a global, binary map) steer so as
        // to avoid collisions within the next minTimeToCollision seconds.
        //
        reVector3Df steerToAvoidObstaclesOnMap (const float minTimeToCollision,
                                         const TerrainMap& map,
                                         const reVector3Df& steerHint)
        {
            const float spacing = map.minSpacing() / 2;
            const float maxSide = radius();
            const float maxForward = minTimeToCollision * speed();
            const int maxSamples = (int) (maxForward / spacing);
            const reVector3Df step = forward () * spacing;
            const reVector3Df fOffset = position ();
            reVector3Df sOffset;
            float s = spacing / 2;

            const int infinity = 9999; // qqq
            int nearestL = infinity;
            int nearestR = infinity;
            int nearestWL = infinity;
            int nearestWR = infinity;
            reVector3Df nearestO;
            wingDrawFlagL = false;
            wingDrawFlagR = false;

            const bool hintGiven = steerHint != reVector3Df::ZERO;
            if (hintGiven && !dtZero) hintGivenCount++;
            if (hintGiven) annotationCircleOrDisk (halfWidth * 0.9f, up(),
                                                   position () + (up () * 0.2f),
                                                   Color::gWhite, 12, false, false);

            // QQQ temporary global QQQoaJustScraping
            QQQoaJustScraping = true;

            const float signedRadius = 1 / nonZeroCurvatureQQQ ();
            const reVector3Df localCenterOfCurvature = side () * signedRadius;
            const reVector3Df center = position () + localCenterOfCurvature;
            const float sign = signedRadius < 0 ? 1.0f : -1.0f;
            const float arcRadius = signedRadius * -sign;
            const float twoPi = 2 * OPENSTEER_M_PI;
            const float circumference = twoPi * arcRadius;
            const float rawLength = speed() * minTimeToCollision * sign;
            const float fracLimit = 1.0f / 6.0f;
            const float distLimit = circumference * fracLimit;
            const float arcLength = arcLengthLimit (rawLength, distLimit);
            const float arcAngle = twoPi * arcLength / circumference;

            // XXX temp annotation to show limit on arc angle
            if (curvedSteering)
            {
                if ((speed() * minTimeToCollision) > (circumference * fracLimit))
                {
                    const float q = twoPi * fracLimit;
                    const reVector3Df fooz = position () - center;
                    const reVector3Df booz = fooz.rotateAboutGlobalY (sign * q);
                    annotationLine (center, center + fooz, Color::gRed);
                    annotationLine (center, center + booz, Color::gRed);
                }
            }

            // assert loops will terminate
        assert (spacing > 0);

            // scan corridor straight ahead of vehicle,
            // keep track of nearest obstacle on left and right sides
            while (s < maxSide)
            {
                sOffset = side() * s;
                s += spacing;
                const reVector3Df lOffset = fOffset + sOffset;
                const reVector3Df rOffset = fOffset - sOffset;

                reVector3Df lObsPos, rObsPos;

                const int L = (curvedSteering ? 
                               (int) (scanObstacleMap (lOffset,
                                                       center,
                                                       arcAngle,
                                                       maxSamples,
                                                       0,
                                                       Color::gYellow,
                                                       Color::gRed,
                                                       lObsPos)
                                      / spacing) :
                               map.scanXZray (lOffset, step, maxSamples));
                const int R = (curvedSteering ? 
                               (int) (scanObstacleMap (rOffset,
                                                        center,
                                                       arcAngle,
                                                       maxSamples,
                                                       0,
                                                       Color::gYellow,
                                                       Color::gRed,
                                                       rObsPos)
                                      / spacing) :
                               map.scanXZray (rOffset, step, maxSamples));

                if ((L > 0) && (L < nearestL))
                {
                    nearestL = L;
                    if (L < nearestR) nearestO = ((curvedSteering) ?
                                                  lObsPos :
                                                  lOffset + ((float)L * step));
                }
                if ((R > 0) && (R < nearestR))
                {
                    nearestR = R;
                    if (R < nearestL) nearestO = ((curvedSteering) ?
                                                  rObsPos :
                                                  rOffset + ((float)R * step));
                }

                if (!curvedSteering)
                {
                    annotateAvoidObstaclesOnMap (lOffset, L, step);
                    annotateAvoidObstaclesOnMap (rOffset, R, step);
                }

                if (curvedSteering)
                {
                    // QQQ temporary global QQQoaJustScraping
                    const bool outermost = s >= maxSide;
                    const bool eitherSide = (L > 0) || (R > 0);
                    if (!outermost && eitherSide) QQQoaJustScraping = false;
                }
            }
            qqqLastNearestObstacle = nearestO;

            // scan "wings"
            {
                const int wingScans = 4;
                // see duplicated code at: QQQ draw sensing "wings"
                // QQQ should be a parameter of this method
                const reVector3Df wingWidth = side() * wingSlope () * maxForward;

                const Color beforeColor (0.75f, 0.9f, 0.0f);  // for annotation
                const Color afterColor  (0.9f,  0.5f, 0.0f);  // for annotation

                for (int i=1; i<=wingScans; i++)
                {
                    const float fraction = (float)i / (float)wingScans;
                    const reVector3Df endside = sOffset + (wingWidth * fraction);
                    const reVector3Df corridorFront = forward() * maxForward;

                    // "loop" from -1 to 1
                    for (int j = -1; j < 2; j+=2)
                    {
                        float k = (float)j; // prevent VC7.1 warning
                        const reVector3Df start = fOffset + (sOffset * k);
                        const reVector3Df end = fOffset + corridorFront + (endside * k);
                        const reVector3Df ray = end - start;
                        const float rayLength = ray.length();
                        const reVector3Df step = ray * spacing / rayLength;
                        const int raySamples = (int) (rayLength / spacing);
                        const float endRadius =
                            wingSlope () * maxForward * fraction *
                            (signedRadius < 0 ? 1 : -1) * (j==1?1:-1);
                        reVector3Df ignore;
                        const int scan = (curvedSteering ?
                                          (int) (scanObstacleMap (start,
                                                                  center,
                                                                  arcAngle,
                                                                  raySamples,
                                                                  endRadius,
                                                                  beforeColor,
                                                                  afterColor,
                                                                  ignore)
                                                 / spacing) :
                                          map.scanXZray (start, step, raySamples));

                        if (!curvedSteering)
                            annotateAvoidObstaclesOnMap (start,scan,step);

                        if (j==1) 
                        {
                            if ((scan > 0) && (scan < nearestWL)) nearestWL = scan;
                        }
                        else
                        {
                            if ((scan > 0) && (scan < nearestWR)) nearestWR = scan;
                        }
                    }
                }
                wingDrawFlagL = nearestWL != infinity;
                wingDrawFlagR = nearestWR != infinity;
            }

            // for annotation
            savedNearestWR = (float) nearestWR;
            savedNearestR  = (float) nearestR;
            savedNearestL  = (float) nearestL;
            savedNearestWL = (float) nearestWL;

            // flags for compound conditions, used below
            const bool obstacleFreeC  = nearestL==infinity && nearestR==infinity;
            const bool obstacleFreeL  = nearestL==infinity && nearestWL==infinity;
            const bool obstacleFreeR  = nearestR==infinity && nearestWR==infinity;
            const bool obstacleFreeWL = nearestWL==infinity;
            const bool obstacleFreeWR = nearestWR==infinity;
            const bool obstacleFreeW  = obstacleFreeWL && obstacleFreeWR;

            // when doing curved steering and we have already detected "just
            // scarping" but neither wing is free, recind the "just scarping"
            // QQQ temporary global QQQoaJustScraping
            const bool JS = curvedSteering && QQQoaJustScraping;
            const bool cancelJS = !obstacleFreeWL && !obstacleFreeWR;
            if (JS && cancelJS) QQQoaJustScraping = false;


            // ----------------------------------------------------------
            // now we have measured everything, decide which way to steer
            // ----------------------------------------------------------


            // no obstacles found on path, return zero steering
            if (obstacleFreeC)
            {
                qqqLastNearestObstacle = reVector3Df::ZERO;
                annotationNoteOAClauseName ("obstacleFreeC");

                // qqq  this may be in the wrong place (what would be the right
                // qqq  place?!) but I'm trying to say "even if the path is
                // qqq  clear, don't go too fast when driving between obstacles
                if (obstacleFreeWL || obstacleFreeWR || GetRelativeSpeed () < 0.7f)
                    return reVector3Df::ZERO;
                else
                    return -forward ();
            }

            // if the nearest obstacle is way out there, take hint if any
    //      if (hintGiven && (minXXX (nearestL, nearestR) > (maxSamples * 0.8f)))
            if (hintGiven && (minXXX ((float)nearestL, (float)nearestR) >
                              (maxSamples * 0.8f)))
            {
                annotationNoteOAClauseName ("nearest obstacle is way out there");
                annotationHintWasTaken ();
                if (steerHint.dotProduct(side())>0) return side();else return -side();
            }

            // QQQ experiment 3-9-04
            //
            // since there are obstacles ahead, if we are already near
            // maximum Curvature, we MUST turn in opposite direction
            //
            // are we turning more sharply than the minimum turning radius?
            // (code from adjustSteeringForMinimumTurningRadius)
            const float maxCurvature = 1 / (minimumTurningRadius () * 1.2f);
            if (absXXX (GetCurvature ()) > maxCurvature)
            {
                annotationNoteOAClauseName ("min turn radius");
                annotationCircleOrDisk (minimumTurningRadius () * 1.2f, up(),
					center, Color::MultColor(Color::gBlue, 0.8f), 40, false, false);
                return side () * sign;
            }

            // if either side is obstacle-free, turn in that direction
            if (obstacleFreeL || obstacleFreeR)
                annotationNoteOAClauseName ("obstacle-free side");

            if (obstacleFreeL) return side();
            if (obstacleFreeR) return -side();

            // if wings are clear, turn away from nearest obstacle straight ahead
            if (obstacleFreeW)
            {
                annotationNoteOAClauseName ("obstacleFreeW");
                // distance to obs on L and R side of corridor roughtly the same
                const bool same = absXXX (nearestL - nearestR) < 5; // within 5
                // if they are about the same and a hint is given, use hint
                if (same && hintGiven)
                {
                    annotationHintWasTaken ();
                    if (steerHint.dotProduct(side())>0) return side();else return -side();
                }
                else
                {
                    // otherwise steer toward the less cluttered side
                    if (nearestL > nearestR) return side(); else return -side();
                }
            }

            // if the two wings are about equally clear and a steering hint is
            // provided, use it
            const bool equallyClear = absXXX (nearestWL-nearestWR) < 2; // within 2
            if (equallyClear && hintGiven)
            {
                annotationNoteOAClauseName ("equallyClear");
                annotationHintWasTaken ();
                if (steerHint.dotProduct(side()) > 0) return side(); else return -side();
            }

            // turn towards the side whose "wing" region is less cluttered
            // (the wing whose nearest obstacle is furthest away)
            annotationNoteOAClauseName ("wing less cluttered");
            if (nearestWL > nearestWR) return side(); else return -side();
        }



        // QQQ reconsider calling sequence
        // called when steerToAvoidObstaclesOnMap decides steering is required
        // (default action is to do nothing, layered classes can overload it)
        // virtual void annotateAvoidObstaclesOnMap (const reVector3Df& scanOrigin,
        //                                           int scanIndex,
        //                                           const reVector3Df& scanStep)
        // {
        // }
        void annotateAvoidObstaclesOnMap (const reVector3Df& scanOrigin,
                                          int scanIndex,
                                          const reVector3Df& scanStep)
        {
            if (scanIndex > 0)
            {
                const reVector3Df hit = scanOrigin + (scanStep * (float) scanIndex);
                annotationLine (scanOrigin, hit, Color (0.7f, 0.3f, 0.3f));
            }
        }


        void annotationNoteOAClauseName (const char* clauseName)
        {
            OPENSTEER_UNUSED_PARAMETER(clauseName);
            
            // does noting now, idea was that it might draw 2d text near vehicle
            // with this state information
            //

            // print version:
            //
            // if (!dtZero) std::cout << clauseName << std::endl;

            // was had been in caller:
            //
            //if (!dtZero)
            //{
            //    const int WR = nearestWR; debugPrint (WR);
            //    const int R  = nearestR;  debugPrint (R);
            //    const int L  = nearestL;  debugPrint (L);
            //    const int WL = nearestWL; debugPrint (WL);
            //} 
        }


        void annotationHintWasTaken (void)
        {
            if (!dtZero) hintTakenCount++;

            const float r = halfWidth * 0.9f;
            const reVector3Df ff = forward () * r;
            const reVector3Df ss = side () * r;
            const reVector3Df pp = position () + (up () * 0.2f);
            annotationLine (pp + ff + ss, pp - ff + ss, Color::gWhite);
            annotationLine (pp - ff - ss, pp - ff + ss, Color::gWhite);
            annotationLine (pp - ff - ss, pp + ff - ss, Color::gWhite);
            annotationLine (pp + ff + ss, pp + ff - ss, Color::gWhite);

            //OpenSteerDemo::clock.setPausedState (true);
        }


        // scan across the obstacle map along a given arc
        // (possibly with radius adjustment ramp)
        // returns approximate distance to first obstacle found
        //
        // QQQ 1: this calling sequence does not allow for zero Curvature case
        // QQQ 2: in library version of this, "map" should be a parameter
        // QQQ 3: instead of passing in colors, call virtual annotation function?
        // QQQ 4: need flag saying to continue after a hit, for annotation
        // QQQ 5: I needed to return both distance-to and position-of the first
        //        obstacle. I added returnObstaclePosition but maybe this should
        //        return a "scan results object" with a flag for obstacle found,
        //        plus distant and position if so.
        //
        float scanObstacleMap (const reVector3Df& start,
                               const reVector3Df& center,
                               const float arcAngle,
                               const int segments,
                               const float endRadiusChange,
                               const Color& beforeColor,
                               const Color& afterColor,
                               reVector3Df& returnObstaclePosition)
        {
            // "spoke" is initially the vector from center to start,
            // which is then rotated step by step around center
            reVector3Df spoke = start - center;
            // determine the angular step per segment
            const float step = arcAngle / segments;
            // store distance to, and position of first obstacle
            float obstacleDistance = 0;
            returnObstaclePosition = reVector3Df::ZERO;
            // for spiral "ramps" of changing radius
            const float startRadius = (endRadiusChange == 0) ? 0 : spoke.length(); 

            // traverse each segment along arc
            float sin=0, cos=0;
            reVector3Df oldPoint = start;
            bool obstacleFound = false;
            for (int i = 0; i < segments; i++)
            {
                // rotate "spoke" to next step around circle
                // (sin and cos values get filled in on first call)
                spoke = spoke.rotateAboutGlobalY (step, sin, cos);

                // for spiral "ramps" of changing radius
                const float adjust = ((endRadiusChange == 0) ?
                                      1.0f :
                                      interpolate ((float)(i+1) / (float)segments,
                                                   1.0f,
                                                   (maxXXX (0,
                                                            (startRadius +
                                                             endRadiusChange))
                                                    / startRadius)));

                // construct new scan point: center point, offset by rotated
                // spoke (possibly adjusting the radius if endRadiusChange!=0)
                const reVector3Df newPoint = center + (spoke * adjust);

                // once an obstacle if found "our work here is done" -- continue
                // to loop only for the sake of annotation (make that optional?)
                if (obstacleFound)
                {
                    annotationLine (oldPoint, newPoint, afterColor);
                }
                else
                {
                    // no obstacle found on this scan so far,
                    // scan map along current segment (a chord of the arc)
                    const reVector3Df offset = newPoint - oldPoint;
                    const float d2 = offset.length() * 2;

                    // when obstacle found: set flag, save distance and position
                    if (! map->isPassable (newPoint))
                    {
                        obstacleFound = true;
                        obstacleDistance = d2 * 0.5f * (i+1);
                        returnObstaclePosition = newPoint;
                    }
                    annotationLine (oldPoint, newPoint, beforeColor);
                }
                // save new point for next time around loop
                oldPoint = newPoint;
            }
            // return distance to first obstacle (or zero if none found)
            return obstacleDistance;
        }


        bool detectImminentCollision (void)
        {
            // QQQ  this should be integrated into steerToAvoidObstaclesOnMap
            // QQQ  since it shares so much infrastructure
            // QQQ  less so after changes on 3-16-04
            bool returnFlag = false;
            const float spacing = map->minSpacing() / 2;
            const float maxSide = halfWidth + spacing;
            const float minDistance = curvedSteering ? 2.0f : 2.5f; // meters
            const float predictTime = curvedSteering ? .75f : 1.3f; // seconds
            const float maxForward =
                speed () * combinedLookAheadTime (predictTime, minDistance);
            const reVector3Df step = forward () * spacing;
            float s = curvedSteering ? (spacing / 4) : (spacing / 2);

            const float signedRadius = 1 / nonZeroCurvatureQQQ ();
            const reVector3Df localCenterOfCurvature = side () * signedRadius;
            const reVector3Df center = position () + localCenterOfCurvature;
            const float sign = signedRadius < 0 ? 1.0f : -1.0f;
            const float arcRadius = signedRadius * -sign;
            const float twoPi = 2 * OPENSTEER_M_PI;
            const float circumference = twoPi * arcRadius;
            const reVector3Df qqqLift (0, 0.2f, 0);
            reVector3Df ignore;

            // scan region ahead of vehicle
            while (s < maxSide)
            {
                const reVector3Df sOffset = side() * s;
                const reVector3Df lOffset = position () + sOffset;
                const reVector3Df rOffset = position () - sOffset;
                const float bevel = 0.3f;
                const float fraction = s / maxSide;
                const float scanDist = (halfLength +
                                        interpolate (fraction,
                                                     maxForward,
                                                     maxForward * bevel));
                const float angle = (scanDist * twoPi * sign) / circumference;
                const int samples = (int) (scanDist / spacing);
                const int L = (curvedSteering ?
                               (int) (scanObstacleMap (lOffset + qqqLift,
                                                       center,
                                                       angle,
                                                       samples,
                                                       0,
                                                       Color::gMagenta,
                                                       Color::gCyan,
                                                       ignore)
                                      / spacing) :
                               map->scanXZray (lOffset, step, samples));
                const int R = (curvedSteering ?
                               (int) (scanObstacleMap (rOffset + qqqLift,
                                                       center,
                                                       angle,
                                                       samples,
                                                       0,
                                                       Color::gMagenta,
                                                       Color::gCyan,
                                                       ignore)
                                      / spacing) :
                               map->scanXZray (rOffset, step, samples));

                returnFlag = returnFlag || (L > 0);
                returnFlag = returnFlag || (R > 0);

                // annotation
                if (! curvedSteering)
                {
                    const reVector3Df d (step * (float) samples);
                    annotationLine (lOffset, lOffset + d, Color::gWhite);
                    annotationLine (rOffset, rOffset + d, Color::gWhite);
                }

                // increment sideways displacement of scan line
                s += spacing;
            }
            return returnFlag;
        }


        // see comments at CFpsVehicle::predictFuturePosition, in this instance
        // I just need the future position (not a LocalSpace), so I'll keep the
        // calling sequence and just conditionalize its body
        //
        // this should be const, but easier for now to ignore that

        reVector3Df predictFuturePosition (const float predictionTime) const
        {
            if (curvedSteering)
            {
                // QQQ this chunk of code is repeated in far too many places,
                // QQQ it has to be moved inside some utility
                // QQQ 
                // QQQ and now, worse, I rearranged it to try the "limit arc
                // QQQ angle" trick
                const float signedRadius = 1 / nonZeroCurvatureQQQ ();
                const reVector3Df localCenterOfCurvature = side () * signedRadius;
                const reVector3Df center = position () + localCenterOfCurvature;
                const float sign = signedRadius < 0 ? 1.0f : -1.0f;
                const float arcRadius = signedRadius * -sign;
                const float twoPi = 2 * OPENSTEER_M_PI;
                const float circumference = twoPi * arcRadius;
                const float rawLength = speed() * predictionTime * sign;
                const float arcLength = arcLengthLimit (rawLength,
                                                        circumference * 0.25f);
                const float arcAngle = twoPi * arcLength / circumference;

                const reVector3Df spoke = position () - center;
                const reVector3Df newSpoke = spoke.rotateAboutGlobalY (arcAngle);
                const reVector3Df prediction = newSpoke + center;

                // QQQ unify with annotatePathFollowing
                const Color futurePositionColor (0.5f, 0.5f, 0.6f);
                annotationXZArc (position (), center, arcLength, 20, 
                                 futurePositionColor);
                return prediction;
            }
            else
            {
                return position() + (velocity() * predictionTime);
            }
        }


        // QQQ experimental fix for arcLength limit in predictFuturePosition
        // QQQ and steerToAvoidObstaclesOnMap.
        //
        // args are the intended arc length (signed!), and the limit which is
        // a given (positive!) fraction of the arc's (circle's) circumference
        //

        float arcLengthLimit (const float length, const float limit) const
        {
            if (length > 0)
                return minXXX (length, limit);
            else
                return -minXXX (-length, limit);
        }


        // this is a version of the one in SteerLibrary.h modified for "slow when
        // heading off path".  I put it here because the changes were not
        // compatible with Pedestrians.cpp.  It needs to be merged back after
        // things settle down.
        //
        // its been modified in other ways too (such as "reduce the offset if
        // facing in the wrong direction" and "increase the target offset to
        // compensate the fold back") plus I changed the type of "path" from
        // Pathway to GCRoute to use methods like indexOfNearestSegment and
        // dotSegmentUnitTangents
        //
        // and now its been modified again for Curvature-based prediction
        //
        reVector3Df steerToFollowPath (const int direction,
                                const float predictionTime,
                                GCRoute& path)
        {
            if (curvedSteering)
                return steerToFollowPathCurve (direction, predictionTime, path);
            else
                return steerToFollowPathLinear (direction, predictionTime, path);
        }


        reVector3Df steerToFollowPathLinear (const int direction,
                                      const float predictionTime,
                                      GCRoute& path)
        {
            // our goal will be offset from our path distance by this amount
            const float pathDistanceOffset = direction * predictionTime * speed();

            // predict our future position
            const reVector3Df futurePosition = predictFuturePosition (predictionTime);

            // measure distance along path of our current and predicted positions
            const float nowPathDistance =
                path.mapPointToPathDistance (position ());

            // are we facing in the correction direction?
            const reVector3Df pathHeading = mapPointToTangent( path, position() ) * static_cast< float >( direction );// path.tangentAt(position()) * (float)direction;
            const bool correctDirection = pathHeading.dotProduct (forward ()) > 0;

            // find the point on the path nearest the predicted future position
            // XXX need to improve calling sequence, maybe change to return a
            // XXX special path-defined object which includes two Vec3s and a 
            // XXX bool (onPath,tangent (ignored), withinPath)
            float futureOutside;
            const reVector3Df onPath = mapPointToPointOnCenterLineAndOutside( path, futurePosition, futureOutside ); // path.mapPointToPath (futurePosition,futureOutside);

            // determine if we are currently inside the path tube
            float nowOutside;
            const reVector3Df nowOnPath = mapPointToPointOnCenterLineAndOutside( path, position(), nowOutside );  // path.mapPointToPath (position (), nowOutside);

            // no steering is required if our present and future positions are
            // inside the path tube and we are facing in the correct direction
            const float m = -radius ();
            const bool whollyInside = (futureOutside < m) && (nowOutside < m);
            if (whollyInside && correctDirection)
            {
                // all is well, return zero steering
                return reVector3Df::ZERO;
            }
            else
            {
                // otherwise we need to steer towards a target point obtained
                // by adding pathDistanceOffset to our current path position
                // (reduce the offset if facing in the wrong direction)
                const float targetPathDistance = (nowPathDistance + 
                                                  (pathDistanceOffset *
                                                   (correctDirection ? 1 : 0.1f)));
                reVector3Df target = path.mapPathDistanceToPoint (targetPathDistance);


                // if we are on one segment and target is on the next segment and
                // the dot of the tangents of the two segments is negative --
                // increase the target offset to compensate the fold back
                const int ip =  static_cast< int >( mapPointToSegmentIndex( path, position() ) ); // path.indexOfNearestSegment (position ());
                const int it =  static_cast< int >( mapPointToSegmentIndex( path, target ) ); // path.indexOfNearestSegment (target);
                // Because polyline paths have a constant tangent along a segment
                // just set the distance along the segment to @c 0.0f.
                reVector3Df const ipTangent = path.mapSegmentDistanceToTangent( ip, 0.0f );
                // Because polyline paths have a constant tangent along a segment
                // just set the distance along the segment to @c 0.0f.
                reVector3Df const itTangent = path.mapSegmentDistanceToTangent( it, 0.0f );
                if (((ip + direction) == it) &&
                    ( /* path.dotSegmentUnitTangents (it, ip) */  itTangent.dotProduct( ipTangent ) < -0.1f ) )
                {
                    const float newTargetPathDistance =
                        nowPathDistance + (pathDistanceOffset * 2);
                    target = path.mapPathDistanceToPoint (newTargetPathDistance);
                }

                annotatePathFollowing (futurePosition,onPath,target,futureOutside);

                // if we are currently outside head directly in
                // (QQQ new, experimental, makes it turn in more sharply)
                if (nowOutside > 0) return steerForSeek (nowOnPath);

                // steering to seek target on path
                const reVector3Df seek = steerForSeek (target).truncateLength(maxForce());

                // return that seek steering -- except when we are heading off
                // the path (currently on path and future position is off path)
                // in which case we put on the brakes.
                if ((nowOutside < 0) && (futureOutside > 0))
                    return (seek.perpendicularComponent (forward ()) -
                            (forward () * maxForce ()));
                else
                    return seek;
            }
        }


        // Path following case for curved prediction and incremental steering
        // (called from steerToFollowPath for the curvedSteering case)
        //
        // QQQ this does not handle the case when we AND futurePosition
        // QQQ are outside, say when approach the path from far away
        //
        reVector3Df steerToFollowPathCurve (const int direction,
                                     const float predictionTime,
                                     GCRoute& path)
        {
            // predict our future position (based on current Curvature and speed)
            const reVector3Df futurePosition = predictFuturePosition (predictionTime);
            // find the point on the path nearest the predicted future position
            float futureOutside;
            const reVector3Df onPath =  mapPointToPointOnCenterLineAndOutside( path, futurePosition, futureOutside ); // path.mapPointToPath (futurePosition,futureOutside);
            const reVector3Df pathHeading =  mapPointAndDirectionToTangent( path, onPath, direction ); // path.tangentAt (onPath, direction);
            const reVector3Df rawBraking = forward () * maxForce () * -1;
            const reVector3Df braking = ((futureOutside < 0) ? reVector3Df::ZERO : rawBraking);
            //qqq experimental wrong-way-fixer
            float nowOutside;
            reVector3Df nowTangent;
            const reVector3Df p = position ();
            const reVector3Df nowOnPath = path.mapPointToPath (p, nowTangent, nowOutside);
            nowTangent *= (float)direction;
            const float alignedness = nowTangent.dotProduct (forward ());

            // facing the wrong way?
            if (alignedness < 0)
            {
                annotationLine (p, p + (nowTangent * 10), Color::gCyan);

                // if nearly anti-parallel
                if (alignedness < -0.707f)
                {
                    const reVector3Df towardCenter = nowOnPath - p;
                    const reVector3Df turn = (towardCenter.dotProduct (side ()) > 0 ?
                                       side () * maxForce () :
                                       side () * maxForce () * -1);
                    return (turn + rawBraking);
                }
                else
                {
                    return (steerTowardHeading(pathHeading).
                            perpendicularComponent(forward()) + braking);
                }
            }

            // is the predicted future position(+radius+margin) inside the path?
            if (futureOutside < -(radius () + 1.0f)) //QQQ
            {
                // then no steering is required
                return reVector3Df::ZERO;
            }
            else
            {
                // otherwise determine corrective steering (including braking)
                annotationLine (futurePosition, futurePosition+pathHeading, Color::gRed);
                annotatePathFollowing (futurePosition, onPath,
                                       position(), futureOutside);

                // two cases, if entering a turn (a waypoint between path segments)
                if ( /* path.nearWaypoint (onPath) */ isNearWaypoint( path, onPath )  && (futureOutside > 0))
                {
                    // steer to align with next path segment
                    annotationCircleOrDisk (0.5f, up(), futurePosition,
                                            Color::gRed, 8, false, false);
                    return steerTowardHeading (pathHeading) + braking;
                }
                else
                {
                    // otherwise steer away from the side of the path we
                    // are heading for
                    const reVector3Df pathSide = localRotateForwardToSide (pathHeading);
                    const reVector3Df towardFP = futurePosition - onPath;
                    const float whichSide = (pathSide.dotProduct(towardFP)<0)?1.0f :-1.0f;
                    return (side () * maxForce () * whichSide) + braking;
                }
            }
        }


        void perFrameAnnotation (void)
        {
            const reVector3Df p = position();

            // draw the circular collision boundary
            annotationCircleOrDisk (radius(), up(), p, Color::gBlack, 32, false, false);

            // draw forward sensing corridor and wings ( for non-curved case)
            if (!curvedSteering)
            {
                const float corLength = speed() * lookAheadTimeOA ();
                if (corLength > halfLength)
                {
                    const reVector3Df corFront = forward() * corLength;
                    const reVector3Df corBack = reVector3Df::ZERO; // (was bbFront)
                    const reVector3Df corSide  = side() * radius();
                    const reVector3Df c1 = p + corSide + corBack;
                    const reVector3Df c2 = p + corSide + corFront;
                    const reVector3Df c3 = p - corSide + corFront;
                    const reVector3Df c4 = p - corSide + corBack;
                    const Color color = ((annotateAvoid!=reVector3Df::ZERO)?Color::gRed : Color::gYellow);
                    annotationLine (c1, c2, color);
                    annotationLine (c2, c3, color);
                    annotationLine (c3, c4, color);

                    // draw sensing "wings"
                    const reVector3Df wingWidth = side () * wingSlope () * corLength;
                    const reVector3Df wingTipL = c2 + wingWidth;
                    const reVector3Df wingTipR = c3 - wingWidth;
                    const Color wingColor (Color::gOrange);
                    if (wingDrawFlagL) annotationLine (c2, wingTipL, wingColor);
                    if (wingDrawFlagL) annotationLine (c1, wingTipL, wingColor);
                    if (wingDrawFlagR) annotationLine (c3, wingTipR, wingColor);
                    if (wingDrawFlagR) annotationLine (c4, wingTipR, wingColor);
                }
            }

            // annotate steering acceleration
            const reVector3Df above = position () + reVector3Df (0, 0.2f, 0);
            const reVector3Df accel = GetSmoothedAcceleration() * 5 / maxForce();
            const Color aColor (0.4f, 0.4f, 0.8f);
            annotationLine (above, above + accel, aColor);
        }

        // draw vehicle's body and annotation
        void draw (void)
        {
            // for now: draw as a 2d bounding box on the ground
            Color                     bodyColor( Color::gBlack );
            if (stuck)               bodyColor = Color::gYellow;
            if (! bodyInsidePath ()) bodyColor = Color::gOrange;
            if (collisionDetected)   bodyColor = Color::gRed;

            // draw vehicle's bounding box on gound plane (its "shadow")
            const reVector3Df p = position();
            const reVector3Df bbSide = side() * halfWidth;
            const reVector3Df bbFront = forward() * halfLength;
            const reVector3Df bbHeight (0, 0.1f, 0);
            drawQuadrangle (p - bbFront + bbSide + bbHeight,
                            p + bbFront + bbSide + bbHeight,
                            p + bbFront - bbSide + bbHeight,
                            p - bbFront - bbSide + bbHeight,
                            bodyColor);

            // annotate trail
            const Color darkGreen (0, 0.6f, 0);
            drawTrail (darkGreen, Color::gBlack);
        }


        // called when steerToFollowPath decides steering is required
        void annotatePathFollowing (const reVector3Df& future,
                                    const reVector3Df& onPath,
                                    const reVector3Df& target,
                                    const float outside)
        {
			const Color toTargetColor (Color::MultColor(Color::gGreen, 0.6f));
            const Color insidePathColor (Color::MultColor(Color::gCyan, 0.6f));
            const Color outsidePathColor (Color::MultColor(Color::gBlue, 0.6f));
            const Color futurePositionColor (0.5f, 0.5f, 0.6f);

            // draw line from our position to our predicted future position
            if (!curvedSteering)
                annotationLine (position(), future, futurePositionColor);

            // draw line from our position to our steering target on the path
            annotationLine (position(), target, toTargetColor);

            // draw a two-toned line between the future test point and its
            // projection onto the path, the change from dark to light color
            // indicates the boundary of the tube.

            const float o = outside + radius () + (curvedSteering ? 1.0f : 0.0f);
			reVector3Df tmp = onPath - future;
			tmp.normalise();
            const reVector3Df boundaryOffset = (tmp * o);

            const reVector3Df onPathBoundary = future + boundaryOffset;
            annotationLine (onPath, onPathBoundary, insidePathColor);
            annotationLine (onPathBoundary, future, outsidePathColor);
        }


        void drawMap (void)
        {
    #ifdef OLDTERRAINMAP
            const float xs = map->xSize/(float)map->resolution;
            const float zs = map->zSize/(float)map->resolution;
            const reVector3Df alongRow (xs, 0, 0);
            const reVector3Df nextRow (-map->xSize, 0, zs);
            reVector3Df g ((map->xSize - xs) / -2, 0, (map->zSize - zs) / -2);
            g += map->center;
            for (int j = 0; j < map->resolution; j++)
            {
                for (int i = 0; i < map->resolution; i++)
                {
                    if (map->getMapBit (i, j))
                    {
                        // spikes
                        // const reVector3Df spikeTop (0, 5.0f, 0);
                        // drawLine (g, g+spikeTop, Color::gWhite);

                        // squares
                        const float rockHeight = 0;
                        const reVector3Df v1 (+xs/2, rockHeight, +zs/2);
                        const reVector3Df v2 (+xs/2, rockHeight, -zs/2);
                        const reVector3Df v3 (-xs/2, rockHeight, -zs/2);
                        const reVector3Df v4 (-xs/2, rockHeight, +zs/2);
                        // const reVector3Df redRockColor (0.6f, 0.1f, 0.0f);
                        const Color orangeRockColor (0.5f, 0.2f, 0.0f);
                        drawQuadrangle (g+v1, g+v2, g+v3, g+v4, orangeRockColor);

                        // pyramids
                        // const reVector3Df top (0, xs/2, 0);
                        // const reVector3Df redRockColor (0.6f, 0.1f, 0.0f);
                        // const reVector3Df orangeRockColor (0.5f, 0.2f, 0.0f);
                        // drawTriangle (g+v1, g+v2, g+top, redRockColor);
                        // drawTriangle (g+v2, g+v3, g+top, orangeRockColor);
                        // drawTriangle (g+v3, g+v4, g+top, redRockColor);
                        // drawTriangle (g+v4, g+v1, g+top, orangeRockColor);
                    } 
                    g += alongRow;
                }
                g += nextRow;
            }
    #else
    #endif
        }

        /**
         * draw the GCRoute as a series of circles and "wide lines"
         * (QQQ this should probably be a method of Path (or a
         * closely-related utility function) in which case should pass
         * color in, certainly shouldn't be recomputing it each draw)
         * @todo Add a <code>reVector3Df const* points() const</code> member function to
         *       SegmentedPath, etc. to allow for faster point access?
         */
        void drawPath (void)
        {
            const Color pathColor (0, 0.5f, 0.5f);
            const Color sandColor (0.8f, 0.7f, 0.5f);
            const Color color = interpolate (0.1f, sandColor, pathColor);

            const reVector3Df down (0, -0.1f, 0);
            for ( u32 i = 1; i < path->pointCount(); ++i )
            {
                const reVector3Df endPoint0 = path->point( i ) + down;
                const reVector3Df endPoint1 = path->point( i - 1 ) + down;

                const float legWidth = path->segmentRadius( i - 1 );

                drawXZWideLine (endPoint0, endPoint1, color, legWidth * 2);
                drawLine (path->point( i ), path->point( i - 1 ), pathColor);
                drawXZDisk (legWidth, endPoint0, color, 24);
                drawXZDisk (legWidth, endPoint1, color, 24);

            }
        }


        GCRoute* makePath (void)
        {
            // a few constants based on world size
            const float m = worldSize * 0.4f; // main diamond size
            const float n = worldSize / 8;    // notch size
            const float o = worldSize * 2;    // outside of the sand

            // construction vectors
            const reVector3Df p (0,   0, m);
            const reVector3Df q (0,   0, m-n);
            const reVector3Df r (-m,  0, 0);
            const reVector3Df s (2*n, 0, 0);
            const reVector3Df t (o,   0, 0);
            const reVector3Df u (-o,  0, 0);
            const reVector3Df v (n,   0, 0);
            const reVector3Df w (0, 0, 0);


            // path vertices
            const reVector3Df a (t-p);
            const reVector3Df b (s+v-p);
            const reVector3Df c (s-q);
            const reVector3Df d (s+q);
            const reVector3Df e (s-v+p);
            const reVector3Df f (p-w);
            const reVector3Df g (r-w);
            const reVector3Df h (-p-w);
            const reVector3Df i (u-p);

            // return Path object
            const int pathPointCount = 9;
            const reVector3Df pathPoints[pathPointCount] = {a, b, c, d, e, f, g, h, i};
            const float k = 10.0f;
            const float pathRadii[pathPointCount] = {k, k, k, k, k, k, k, k, k};
            return new GCRoute (pathPointCount, pathPoints, pathRadii, false);
        }


        TerrainMap* makeMap (void)
        {
    #ifdef OLDTERRAINMAP
            return new TerrainMap (reVector3Df::ZERO,
                                   worldSize,
                                   worldSize,
                                   (int)worldSize + 1);
    #else
            return new TerrainMap (worldSize, worldSize, 1);
    #endif
        }


        bool handleExitFromMap (void)
        {
            if (demoSelect == 2)
            {
                // for path following, do wrap-around (teleport) and make new map
                const float px = position ().x;
                const float fx = forward ().x;
                const float ws = worldSize * 0.51f; // slightly past edge
                if (((fx > 0) && (px > ws)) || ((fx < 0) && (px < -ws)))
                {
                    // bump counters
                    lapsStarted++;
                    lapsFinished++;

                    const reVector3Df camOffsetBefore =
                        OpenSteerDemo::camera.position() - position ();

                    // set position on other side of the map (set new X coordinate)
                    setPosition ((((px < 0) ? 1 : -1) *
                                  ((worldSize * 0.5f) +
                                   (speed() * lookAheadTimePF ()))),
                                 position ().y,
                                 position ().z);

                    // reset bookeeping to detect stuck cycles
                    resetStuckCycleDetection ();

                    // new camera position and aimpoint to compensate for teleport
                    OpenSteerDemo::camera.target = position ();
                    OpenSteerDemo::camera.setPosition (position () + camOffsetBefore);

                    // make camera jump immediately to new position
                    OpenSteerDemo::camera.doNotSmoothNextMove ();

                    // prevent long streaks due to teleportation 
                    clearTrailHistory ();

                    return true; 
                }
            }
            else
            {
                // for the non-path-following demos:
                // reset simulation if the vehicle drives through the fence
                if (position().length() > worldDiag) reset();
            }
            return false;
        }


        float wingSlope (void)
        {
            return interpolate (GetRelativeSpeed (),
                                (curvedSteering ? 0.3f : 0.35f),
                                0.06f);
        }


        void resetStuckCycleDetection (void)
        {
            ResetSmoothedPosition (position () + (forward () * -80)); // qqq
        }


        // QQQ just a stop gap, not quite right
        // (say for example we were going around a circle with radius > 10)
        bool weAreGoingInCircles (void)
        {
            const reVector3Df offset = GetSmoothedPosition () - position ();
            return offset.length () < 10;
        }


        float lookAheadTimeOA (void) const
        {
            const float minTime = (baseLookAheadTime *
                                   (curvedSteering ?
                                    interpolate (GetRelativeSpeed(), 0.4f, 0.7f) :
                                    0.66f));
            return combinedLookAheadTime (minTime, 3);
        }

        float lookAheadTimePF (void) const
        {
            return combinedLookAheadTime (baseLookAheadTime, 3);
        }

        // QQQ maybe move to CFpsVehicle ?
        // compute a "look ahead time" with two components, one based on
        // minimum time to (say) a collision and one based on minimum distance
        // arg 1 is "seconds into the future", arg 2 is "meters ahead"
        float combinedLookAheadTime (float minTime, float minDistance) const
        {
            if (speed () == 0) return 0;
            return maxXXX (minTime, minDistance / speed ());
        }


        // is vehicle body inside the path?
        // (actually tests if all four corners of the bounbding box are inside)
        //
        bool bodyInsidePath (void)
        {
            if (demoSelect == 2)
            {
                const reVector3Df bbSide = side () * halfWidth;
                const reVector3Df bbFront = forward () * halfLength;
                return ( /* path->isInsidePath (position () - bbFront + bbSide) */ isInsidePathway( *path, position () - bbFront + bbSide ) &&
                         /* path->isInsidePath (position () + bbFront + bbSide) */ isInsidePathway( *path, position () + bbFront + bbSide ) &&
                         /* path->isInsidePath (position () + bbFront - bbSide) */ isInsidePathway( *path, position () + bbFront - bbSide ) &&
                         /* path->isInsidePath (position () - bbFront - bbSide) */ isInsidePathway( *path, position () - bbFront - bbSide ) );
            }
            return true;
        }


        reVector3Df convertAbsoluteToIncrementalSteering (const reVector3Df& absolute,
                                                   const float elapsedTime)
        {
            const reVector3Df curved = convertLinearToCurvedSpaceGlobal (absolute);
            blendIntoAccumulator (elapsedTime * 8.0f, curved, currentSteering);
            {
                // annotation
                const reVector3Df u (0, 0.5, 0);
                const reVector3Df p = position ();
                annotationLine (p + u, p + u + absolute, Color::gRed);
                annotationLine (p + u, p + u + curved, Color::gYellow);
                annotationLine (p + u*2, p + u*2 + currentSteering, Color::gGreen);
            }
            return currentSteering;
        }


        // QQQ new utility 2-25-04 -- may replace inline code elsewhere
        //
        // Given a location in this vehicle's linear local space, convert it into
        // the curved space defined by the vehicle's current path Curvature.  For
        // example, forward() gets mapped on a point 1 unit along the circle
        // centered on the current center of Curvature and passing through the
        // vehicle's position().
        //
        reVector3Df convertLinearToCurvedSpaceGlobal (const reVector3Df& linear)
        {
            const reVector3Df trimmedLinear = linear.truncateLength (maxForce ());

            // ---------- this block imported from steerToAvoidObstaclesOnMap
            const float signedRadius = 1 / (nonZeroCurvatureQQQ() /*QQQ*/ * 1);
            const reVector3Df localCenterOfCurvature = side () * signedRadius;
            const reVector3Df center = position () + localCenterOfCurvature;
            const float sign = signedRadius < 0 ? 1.0f : -1.0f;
            const float arcLength = trimmedLinear.dotProduct (forward ());
            //
            const float arcRadius = signedRadius * -sign;
            const float twoPi = 2 * OPENSTEER_M_PI;
            const float circumference = twoPi * arcRadius;
            const float arcAngle = twoPi * arcLength / circumference;
            // ---------- this block imported from steerToAvoidObstaclesOnMap

            // ---------- this block imported from scanObstacleMap
            // vector from center of Curvature to position of vehicle
            const reVector3Df initialSpoke = position () - center;
            // rotate by signed arc angle
            const reVector3Df spoke = initialSpoke.rotateAboutGlobalY (arcAngle * sign);
            // ---------- this block imported from scanObstacleMap

            const reVector3Df fromCenter = Vec3Utilities::normalise(-localCenterOfCurvature);
            const float dRadius = trimmedLinear.dotProduct (fromCenter);
            const float radiusChangeFactor = (dRadius + arcRadius) / arcRadius;
            const reVector3Df resultLocation = center + (spoke * radiusChangeFactor);
            {
                const reVector3Df center = position () + localCenterOfCurvature;
                annotationXZArc (position (), center, speed () * sign * -3,
                                 20, Color::gWhite);
            }
            // return the vector from vehicle position to the coimputed location
            // of the curved image of the original linear offset
            return resultLocation - position ();
        }


        // approximate value for the Polaris Ranger 6x6: 16 feet, 5 meters
        float minimumTurningRadius () const {return 5.0f;}


        reVector3Df adjustSteeringForMinimumTurningRadius (const reVector3Df& steering)
        {
            const float maxCurvature = 1 / (minimumTurningRadius () * 1.1f);

            // are we turning more sharply than the minimum turning radius?
            if (absXXX (GetCurvature()) > maxCurvature)
            {
                // remove the tangential (non-thrust) component of the steering
                // force, replace it with a force pointing away from the center
                // of Curvature, causing us to "widen out" easing off from the
                // minimum turing radius
                const float signedRadius = 1 / nonZeroCurvatureQQQ ();
                const float sign = signedRadius < 0 ? 1.0f : -1.0f;
                const reVector3Df thrust = steering.parallelComponent (forward ());
                const reVector3Df trimmed = thrust.truncateLength (maxForce ());
                const reVector3Df widenOut = side () * maxForce () * sign;
                {
                    // annotation
                    const reVector3Df localCenterOfCurvature = side () * signedRadius;
                    const reVector3Df center = position () + localCenterOfCurvature;
                    annotationCircleOrDisk (minimumTurningRadius (), up(),
                                            center, Color::gBlue, 40, false, false);
                }
                return trimmed + widenOut;
            }

            // otherwise just return unmodified input
            return steering;
        }


        // QQQ This is to work around the bug that scanObstacleMap's current
        // QQQ arguments preclude the driving straight [Curvature()==0] case.
        // QQQ This routine returns the current vehicle path Curvature, unless it
        // QQQ is *very* close to zero, in which case a small positive number is
        // QQQ returned (corresponding to a radius of 100,000 meters).  
        // QQQ
        // QQQ Presumably it would be better to get rid of this routine and
        // QQQ redesign the arguments of scanObstacleMap
        //
        float nonZeroCurvatureQQQ (void) const
        {
            const float c = GetCurvature();
            const float minCurvature = 1.0f / 100000.0f; // 100,000 meter radius
            const bool tooSmall = (c < minCurvature) && (c > -minCurvature);
            return (tooSmall ? minCurvature: c);
        }


        // QQQ ad hoc speed limitation based on path orientation...
        // QQQ should be renamed since it is based on more than Curvature
        //
        float maxSpeedForCurvature ()
        {
            float maxRelativeSpeed = 1;

            if (curvedSteering)
            {
                // compute an ad hoc "relative Curvature"
                const float absC = absXXX (GetCurvature());
                const float maxC = 1 / minimumTurningRadius ();
                const float relativeCurvature = sqrtXXX (clip (absC/maxC, 0, 1));

                // map from full throttle when straight to 10% at max Curvature
                const float curveSpeed = interpolate (relativeCurvature,1.0f,0.1f);
                annoteMaxRelSpeedCurve = curveSpeed;

                if (demoSelect != 2)
                {
                    maxRelativeSpeed = curveSpeed;
                }
                else
                {
                    // heading (unit tangent) of the path segment of interest
                    const reVector3Df pathHeading =  mapPointAndDirectionToTangent( *path, position(), pathFollowDirection ); // path->tangentAt (position (), pathFollowDirection);
                    // measure how parallel we are to the path
                    const float parallelness = pathHeading.dotProduct (forward ());

                    // determine relative speed for this heading
                    const float mw = 0.2f;
                    const float headingSpeed = ((parallelness < 0) ? mw :
                                                interpolate (parallelness,
                                                             mw, 1.0f));
                    maxRelativeSpeed = minXXX (curveSpeed, headingSpeed);
                    annoteMaxRelSpeedPath = headingSpeed;
                }
            }
            annoteMaxRelSpeed = maxRelativeSpeed;
            return maxSpeed () * maxRelativeSpeed;
        }


        // xxx library candidate
        // xxx assumes (but does not check or enforce) heading is unit length
        //
        reVector3Df steerTowardHeading (const reVector3Df& desiredGlobalHeading)
        {
            const reVector3Df headingError = desiredGlobalHeading - forward ();
            return Vec3Utilities::normalise(headingError) * maxForce ();
        }


        // XXX this should eventually be in a library, make it a first
        // XXX class annotation queue, tie in with drawXZArc
        void annotationXZArc (const reVector3Df& start,
                              const reVector3Df& center,
                              const float arcLength,
                              const int segments,
                              const Color& color) const
        {
            // "spoke" is initially the vector from center to start,
            // it is then rotated around its tail
            reVector3Df spoke = start - center;

            // determine the angular step per segment
            const float radius = spoke.length ();
            const float twoPi = 2 * OPENSTEER_M_PI;
            const float circumference = twoPi * radius;
            const float arcAngle = twoPi * arcLength / circumference;
            const float step = arcAngle / segments;

            // draw each segment along arc
            float sin=0, cos=0;
            for (int i = 0; i < segments; i++)
            {
                const reVector3Df old = spoke + center;

                // rotate point to next step around circle
                spoke = spoke.rotateAboutGlobalY (step, sin, cos);

                annotationLine (spoke + center, old, color);
            }
        }

        // map of obstacles
        TerrainMap* map;

        // route for path following (waypoints and legs)
        GCRoute* path;

        // follow the path "upstream or downstream" (+1/-1)
        int pathFollowDirection;

        // master look ahead (prediction) time
        float baseLookAheadTime;

        // vehicle dimentions in meters
        float halfWidth;
        float halfLength;

        // keep track of failure rate (when vehicle is on top of obstacle)
        bool collisionDetected;
        bool collisionLastTime;
        float timeOfLastCollision;
        float sumOfCollisionFreeTimes;
        int countOfCollisionFreeTimes;

        // keep track of average speed
        float totalDistance;
        float totalTime;

        // keep track of path following failure rate
        // (these are probably obsolete now, replaced by stuckOffPathCount)
        float pathFollowTime;
        float pathFollowOffTime;

        // take note when current dt is zero (as in paused) for stat counters
        bool dtZero;

        // state saved for annotation
        reVector3Df annotateAvoid;
        bool wingDrawFlagL, wingDrawFlagR;

        // QQQ first pass at detecting "stuck" state
        bool stuck;
        int stuckCount;
        int stuckCycleCount;
        int stuckOffPathCount;

        reVector3Df qqqLastNearestObstacle;

        int lapsStarted;
        int lapsFinished;

        // QQQ temporary global QQQoaJustScraping
        // QQQ replace this global flag with a cleaner mechanism
        bool QQQoaJustScraping;

        int hintGivenCount;
        int hintTakenCount;

        // for "Curvature-based incremental steering" -- contains the current
        // steering into which new incremental steering is blended
        reVector3Df currentSteering;

        // use curved prediction and incremental steering:
        bool curvedSteering;
        bool incrementalSteering;

        // save obstacle avoidance stats for annotation
        // (nearest obstacle in each of the four zones)
        static float savedNearestWR, savedNearestR, savedNearestL, savedNearestWL;

        float annoteMaxRelSpeed, annoteMaxRelSpeedCurve, annoteMaxRelSpeedPath;

        // which of the three demo modes is selected
        static int demoSelect;

        // size of the world (the map actually)
        static float worldSize;
        static float worldDiag;
    };


    // define map size (and compute its half diagonal)
    float MapDriver::worldSize = 200;
    float MapDriver::worldDiag = sqrtXXX (square (worldSize) / 2);

    // 0 = obstacle avoidance and speed control
    // 1 = wander, obstacle avoidance and speed control
    // 2 = path following, obstacle avoidance and speed control
    // int MapDriver::demoSelect = 0;
    int MapDriver::demoSelect = 2;

    float MapDriver::savedNearestWR = 0;
    float MapDriver::savedNearestR = 0;
    float MapDriver::savedNearestL = 0;
    float MapDriver::savedNearestWL = 0;


    // ----------------------------------------------------------------------------
    // PlugIn for OpenSteerDemo


    class MapDrivePlugIn : public PlugIn
    {
    public:

        const char* name (void) {return "Driving through map based obstacles";}

        float selectionOrderSortKey (void) {return 0.07f;}

        // be more "nice" to avoid a compiler warning
        virtual ~MapDrivePlugIn() {}

        void open (void)
        {
            // make new MapDriver
            vehicle = new MapDriver ();
            vehicles.push_back (vehicle);
            OpenSteerDemo::selectedVehicle = vehicle;

            // marks as obstacles map cells adjacent to the path
            usePathFences = true; 

            // scatter random rock clumps over map
            useRandomRocks = true;

            // init OpenSteerDemo camera
            initCamDist = 30;
            initCamElev = 15;
            OpenSteerDemo::init2dCamera (*vehicle, initCamDist, initCamElev);
            // "look straight down at vehicle" camera mode parameters
            OpenSteerDemo::camera.lookdownDistance = 50;
            // "static" camera mode parameters
            OpenSteerDemo::camera.fixedPosition.set (145, 145, 145);
            OpenSteerDemo::camera.fixedTarget.set (40, 0, 40);
			OpenSteerDemo::camera.fixedUp = reVector3Df::UNIT_Y;

            // reset this plugin
            reset ();
        }


        void update (const float currentTime, const float elapsedTime)
        {
            // update simulation of test vehicle
            vehicle->update (currentTime, elapsedTime);

            // when vehicle drives outside the world
            if (vehicle->handleExitFromMap ()) regenerateMap ();

            // QQQ first pass at detecting "stuck" state
            if (vehicle->stuck && (vehicle->GetRelativeSpeed () < 0.001f))
            {
                vehicle->stuckCount++;
                reset();
            }
        }


        void redraw (const float currentTime, const float elapsedTime)
        {
            // update camera, tracking test vehicle
            OpenSteerDemo::updateCamera (currentTime, elapsedTime, *vehicle);

            // draw "ground plane"  (make it 4x map size)
            const float s = MapDriver::worldSize * 2;
            const float u = -0.2f;
            drawQuadrangle (reVector3Df (+s, u, +s),
                            reVector3Df (+s, u, -s),
                            reVector3Df (-s, u, -s),
                            reVector3Df (-s, u, +s),
                            Color (0.8f, 0.7f, 0.5f)); // "sand"

            // draw map and path
            vehicle->drawMap ();
            if (vehicle->demoSelect == 2) vehicle->drawPath ();

            // draw test vehicle
            vehicle->draw ();

            // QQQ mark origin to help spot artifacts
            const float tick = 2;
            drawLine (reVector3Df (tick, 0, 0), reVector3Df (-tick, 0, 0), Color::gGreen);
            drawLine (reVector3Df (0, 0, tick), reVector3Df (0, 0, -tick), Color::gGreen);

            // compute conversion factor miles-per-hour to meters-per-second
            const float metersPerMile = 1609.344f;
            const float secondsPerHour = 3600;
            const float MPSperMPH = metersPerMile / secondsPerHour;

            // display status in the upper left corner of the window
            std::ostringstream status;
            status << "Speed: "
                   << (int) vehicle->speed () << " mps ("
                   << (int) (vehicle->speed () / MPSperMPH) << " mph)"
                   << ", average: "
                   << std::setprecision (1) << std::setiosflags (std::ios::fixed)
                   << vehicle->totalDistance / vehicle->totalTime

                   << " mps\n\n";
            status << "collisions avoided for "
                   << (int)(OpenSteerDemo::clock.getTotalSimulationTime () -
                            vehicle->timeOfLastCollision)
                   << " seconds";
            if (vehicle->countOfCollisionFreeTimes > 0)
            {
                status << "\nmean time between collisions: "
                       << (int) (vehicle->sumOfCollisionFreeTimes /
                                 vehicle->countOfCollisionFreeTimes)
                       << " ("
                       << (int)vehicle->sumOfCollisionFreeTimes
                       << "/"
                       << (int)vehicle->countOfCollisionFreeTimes
                       << ")";
            }

            status << "\n\nStuck count: " << vehicle->stuckCount << " (" 
                   << vehicle->stuckCycleCount << " cycles, "
                   << vehicle->stuckOffPathCount << " off path)";
            status << "\n\n[F1] ";
            if (1 == vehicle->demoSelect) status << "wander, ";
            if (2 == vehicle->demoSelect) status << "follow path, ";
            status << "avoid obstacle";

            if (2 == vehicle->demoSelect)
            {
                status << "\n[F2] path following direction: ";
                if (vehicle->pathFollowDirection>0)status<<"+1";else status<<"-1";
                status << "\n[F3] path fence: ";
                if (usePathFences) status << "on"; else status << "off";
            }

            status << "\n[F4] rocks: ";
            if (useRandomRocks) status << "on"; else status << "off";
            status << "\n[F5] prediction: ";
            if (vehicle->curvedSteering)
                status << "curved"; else status << "linear";
            if (2 == vehicle->demoSelect)
            {
                status << "\n\nLap " << vehicle->lapsStarted
                       << " (completed: "
                       << ((vehicle->lapsStarted < 2) ? 0 :
                           (int) (100 * ((float) vehicle->lapsFinished /
                                         (float) (vehicle->lapsStarted - 1))))
                       << "%)";

                status << "\nHints given: " << vehicle->hintGivenCount
                       << ", taken: " << vehicle->hintTakenCount;
            }
            status << "\n";
            qqqRange ("WR ", vehicle->savedNearestWR, status);
            qqqRange ("R  ", vehicle->savedNearestR,  status);
            qqqRange ("L  ", vehicle->savedNearestL,  status);
            qqqRange ("WL ", vehicle->savedNearestWL, status);
            status << std::ends;
            const float h = drawGetWindowHeight ();
            const reVector3Df screenLocation (10, h-50, 0);
            const Color color (0.15f, 0.15f, 0.5f);
            draw2dTextAt2dLocation (status, screenLocation, color, drawGetWindowWidth(), drawGetWindowHeight());

            {
                const float v = 5;
                const float m = 10;
                const float w = drawGetWindowWidth ();
                const float f = w - (2 * m);
                const float s = vehicle->GetRelativeSpeed ();

                // limit tick mark
                const float l = vehicle->annoteMaxRelSpeed;
                draw2dLine (reVector3Df (m+(f*l), v-3, 0), reVector3Df (m+(f*l),v+3, 0), Color::gBlack, drawGetWindowWidth(), drawGetWindowHeight());
                // two "inverse speedometers" showing limits due to Curvature and
                // path alignment
                if (l!=0)
                {
                    const float c = vehicle->annoteMaxRelSpeedCurve;
                    const float p = vehicle->annoteMaxRelSpeedPath;
                    draw2dLine (reVector3Df(m+(f*c), v+1, 0), reVector3Df(w-m, v+1, 0), Color::gRed, drawGetWindowWidth(), drawGetWindowHeight());
                    draw2dLine (reVector3Df(m+(f*p), v-2, 0), reVector3Df(w-m, v-1, 0), Color::gGreen, drawGetWindowWidth(), drawGetWindowHeight());
                }
                // speedometer: horizontal line with length proportional to speed
                draw2dLine (reVector3Df (m, v, 0), reVector3Df (m + (f * s), v, 0), Color::gWhite, drawGetWindowWidth(), drawGetWindowHeight());
                // min and max tick marks
                draw2dLine (reVector3Df (m,       v, 0), reVector3Df (m,      v-2, 0), Color::gWhite, drawGetWindowWidth(), drawGetWindowHeight());
                draw2dLine (reVector3Df (w-m,     v, 0), reVector3Df (w-m,    v-2, 0), Color::gWhite, drawGetWindowWidth(), drawGetWindowHeight());
            }
        }

        void qqqRange (char* string, float range, std::ostringstream& status)
        {
            status << "\n" << string;
            if (range == 9999.0f) status << "--"; else status << (int) range;
        }

        void close (void)
        {
            vehicles.clear ();
            delete (vehicle);
        }

        void reset (void)
        {
            regenerateMap();

            // reset vehicle
            vehicle->reset ();

            // make camera jump immediately to new position
            OpenSteerDemo::camera.doNotSmoothNextMove ();

            // reset camera position
            OpenSteerDemo::position2dCamera (*vehicle, initCamDist, initCamElev);
        }

        void handleFunctionKeys (int keyNumber)
        {
            switch (keyNumber)
            {
            case 1: selectNextDemo (); break;
            case 2: reversePathFollowDirection (); break;
            case 3: togglePathFences (); break;
            case 4: toggleRandomRocks (); break;
            case 5: toggleCurvedSteering (); break;

            case 6: // QQQ draw an enclosed "pen" of obstacles to test cycle-stuck
                {
                    const float m = MapDriver::worldSize * 0.4f; // main diamond size
                    const float n = MapDriver::worldSize / 8;    // notch size
                    const reVector3Df q (0,   0, m-n);
                    const reVector3Df s (2*n, 0, 0);
                    const reVector3Df c (s-q);
                    const reVector3Df d (s+q);
                    const int pathPointCount = 2;
                    const float pathRadii[pathPointCount] = {10, 10};
                    const reVector3Df pathPoints[pathPointCount] = {c, d};
                    GCRoute r (pathPointCount, pathPoints, pathRadii, false);
                    drawPathFencesOnMap (*vehicle->map, r);
                    break;
                }
            }
        }

        void printMiniHelpForFunctionKeys (void)
        {
            std::ostringstream message;
            message << "Function keys handled by ";
            message << '"' << name() << '"' << ':' << std::ends;
            OpenSteerDemo::printMessage (message);
            OpenSteerDemo::printMessage ("  F1     select next driving demo.");
            OpenSteerDemo::printMessage ("  F2     reverse path following direction.");
            OpenSteerDemo::printMessage ("  F3     toggle path fences.");
            OpenSteerDemo::printMessage ("  F4     toggle random rock clumps.");
            OpenSteerDemo::printMessage ("  F5     toggle curved prediction.");
            OpenSteerDemo::printMessage ("");
        }

        void reversePathFollowDirection (void)
        {
            int& pfd = vehicle->pathFollowDirection;
            pfd = (pfd > 0) ? -1 : +1;
        }

        void togglePathFences (void)
        {
            usePathFences = ! usePathFences;
            reset ();
        }

        void toggleRandomRocks (void)
        {
            useRandomRocks = ! useRandomRocks;
            reset ();
        }

        void toggleCurvedSteering (void)
        {
            vehicle->curvedSteering = ! vehicle->curvedSteering;
            vehicle->incrementalSteering = ! vehicle->incrementalSteering;
            reset ();
        }

        void selectNextDemo (void)
        {
            std::ostringstream message;
            message << name() << ": ";
            switch (++vehicle->demoSelect)
            {
            default:
                vehicle->demoSelect = 0; // wrap-around, falls through to case 0:
            case 0:
                message << "obstacle avoidance and speed control";
                reset ();
                break;
            case 1: 
                message << "wander, obstacle avoidance and speed control";
                reset ();
                break;
            case 2: 
                message << "path following, obstacle avoidance and speed control";
                reset ();
                break;
            }
            message << std::ends;
            OpenSteerDemo::printMessage (message);
        }

        // random utility, worth moving to Utilities.h?
        int irandom2 (int min, int max)
        {
            return (int) frandom2 ((float) min, (float) max);
        }

	void regenerateMap (void)
	{
	    // regenerate map: clear and add random "rocks"
	    vehicle->map->clear();
	    drawRandomClumpsOfRocksOnMap (*vehicle->map);
	    clearCenterOfMap (*vehicle->map);

	    // draw fences for first two demo modes
	    if (vehicle->demoSelect < 2) drawBoundaryFencesOnMap (*vehicle->map);

	    // randomize path widths
	    if (vehicle->demoSelect == 2)
	    {
		const u32 count = vehicle->path->segmentCount();
		const bool upstream = vehicle->pathFollowDirection > 0;
		const u32 entryIndex = upstream ? 0 : count-1;
		const u32 exitIndex  = upstream ? count-1 : 0;
		const float lastExitRadius = vehicle->path->segmentRadius( exitIndex );
		for (u32 i = 0; i < count; i++)
		{
		    vehicle->path->setSegmentRadius( i, frandom2 (4, 19) );
		}
		vehicle->path->setSegmentRadius( entryIndex, lastExitRadius );
	    }

	    // mark path-boundary map cells as obstacles
	    // (when in path following demo and appropriate mode is set)
	    if (usePathFences && (vehicle->demoSelect == 2))
		drawPathFencesOnMap (*vehicle->map, *vehicle->path);
	}

        void drawRandomClumpsOfRocksOnMap (TerrainMap& map)
        {
            if (useRandomRocks)
            {
                const int spread = 4;
                const int r = map.cellwidth();
                const int k = irandom2 (50, 150);

                for (int p=0; p<k; p++)
                {
                    const int i = irandom2 (0, r - spread);
                    const int j = irandom2 (0, r - spread);
                    const int c = irandom2 (0, 10);

                    for (int q=0; q<c; q++)
                    {
                        const int m = irandom2 (0, spread);
                        const int n = irandom2 (0, spread);
    #ifdef OLDTERRAINMAP
                        map.setMapBit (i+m, j+n, 1);
    #else
                        map.setType (i+m, j+n, CellData::OBSTACLE);
    #endif
                    }
                }
            }
        }


        void drawBoundaryFencesOnMap (TerrainMap& map)
        {
            // QQQ it would make more sense to do this with a "draw line
            // QQQ on map" primitive, may need that for other things too

            const int cw = map.cellwidth();
            const int ch = map.cellheight();

            const int r = cw - 1;
            const int a = cw >> 3;
            const int b = cw - a;
            const int o = cw >> 4;
            const int p = (cw - o) >> 1;
            const int q = (cw + o) >> 1;

            for (int i = 0; i < cw; i++)
            {
                for (int j = 0; j < ch; j++)
                {
                    const bool c = i>a && i<b && (i<p || i>q);
                    if (i==0 || j==0 || i==r || j==r || (c && (i==j || i+j==r))) 
    #ifdef OLDTERRAINMAP
                        map.setMapBit (i, j, 1);
    #else
                        map.setType (i, j, CellData::IMPASSABLE);
    #endif
                }
            }
        }


        void clearCenterOfMap (TerrainMap& map)
        {
            const int o = map.cellwidth() >> 4;
            const int p = (map.cellwidth() - o) >> 1;
            const int q = (map.cellwidth() + o) >> 1;
            for (int i = p; i <= q; i++)
                for (int j = p; j <= q; j++)
    #ifdef OLDTERRAINMAP
                    map.setMapBit (i, j, 0);
    #else
                    map.setType (i, j, CellData::CLEAR);
    #endif
        }


        void drawPathFencesOnMap (TerrainMap& map, GCRoute& path)
        {
    #ifdef OLDTERRAINMAP
            const float xs = map.xSize / (float)map.resolution;
            const float zs = map.zSize / (float)map.resolution;
            const reVector3Df alongRow (xs, 0, 0);
            const reVector3Df nextRow (-map.xSize, 0, zs);
            reVector3Df g ((map.xSize - xs) / -2, 0, (map.zSize - zs) / -2);
            for (int j = 0; j < map.resolution; j++)
            {
                for (int i = 0; i < map.resolution; i++)
                {
                    const float outside = mapPointToOutside( path, g ); // path.howFarOutsidePath (g);
                    const float wallThickness = 1.0f;

                    // set map cells adjacent to the outside edge of the path
                    if ((outside > 0) && (outside < wallThickness))
                        map.setMapBit (i, j, true);

                    // clear all other off-path map cells 
                    if (outside > wallThickness) map.setMapBit (i, j, false);

                    g += alongRow;
                }
                g += nextRow;
            }
    #else
    #endif
        }

        const Core::AI::AVGroup& allVehicles (void) {return (const AVGroup&) vehicles;}

        MapDriver* vehicle;
        std::vector<MapDriver*> vehicles; // for allVehicles

        float initCamDist, initCamElev;

        bool usePathFences;
        bool useRandomRocks;
    };


    MapDrivePlugIn gMapDrivePlugIn;


    // ----------------------------------------------------------------------------

} // anonymous namespace
