//// SimpleVehicle
////
//// A steerable point mass with a velocity-aligned local coordinate system.
//// SimpleVehicle is useful for developing prototype vehicles in OpenSteerDemo,
//// it is the base class for vehicles in the PlugIns supplied with OpenSteer.
//// Note that SimpleVehicle is intended only as sample code.  Your application
//// can use the OpenSteer library without using SimpleVehicle, as long as you
//// implement the AbstractVehicle protocol.
////
//// SimpleVehicle makes use of the "mixin" concept from OOP.  To quote
//// "Mixin-Based Programming in C++" a clear and helpful paper by Yannis
//// Smaragdakis and Don Batory (http://citeseer.nj.nec.com/503808.html):
////
////     ...The idea is simple: we would like to specify an extension without
////     predetermining what exactly it can extend. This is equivalent to
////     specifying a subclass while leaving its superclass as a parameter to be
////     determined later. The benefit is that a single class can be used to
////     express an incremental extension, valid for a variety of classes...
//// 
//// In OpenSteer, vehicles are defined by an interface: an abstract base class
//// called AbstractVehicle.  Implementations of that interface, and related
//// functionality (like steering behaviors and vehicle physics) are provided as
//// template-based mixin classes.  The intent of this design is to allow you to
//// reuse OpenSteer code with your application's own base class.
//
//#ifndef OPENSTEER_SIMPLEVEHICLE_H
//#define OPENSTEER_SIMPLEVEHICLE_H
//
//
//#include "AbstractVehicle.h"
//#include "SteerLibrary.h"
//#include "Annotation.h"
//#include "Vec3Utilities.h"
//
//namespace Core
//{
//namespace AI
//{
//
//    // SimpleVehicle_1 adds concrete LocalSpace methods to AbstractVehicle
//    typedef LocalSpaceMixin<AbstractVehicle> SimpleVehicle_1;
//
//
//    // SimpleVehicle_2 adds concrete annotation methods to SimpleVehicle_1
//    typedef AnnotationMixin<SimpleVehicle_1> SimpleVehicle_2;
//
//
//    // SimpleVehicle_3 adds concrete steering methods to SimpleVehicle_2
//    typedef SteerLibraryMixin<SimpleVehicle_2> SimpleVehicle_3;
//
//
//    // SimpleVehicle adds concrete vehicle methods to SimpleVehicle_3
//    class SimpleVehicle : public SimpleVehicle_3
//    {
//    public:
//
//        // constructor
//        SimpleVehicle ();
//
//        // destructor
//        ~SimpleVehicle ();
//
//        // reset vehicle state
//        void reset (void)
//        {
//            // reset LocalSpace state
//            resetLocalSpace ();
//
//            // reset SteerLibraryMixin state
//            // (XXX this seems really fragile, needs to be redesigned XXX)
//            SimpleVehicle_3::reset ();
//
//            setMass (1);          // mass (defaults to 1 so acceleration=force)
//            setSpeed (0);         // speed along Forward direction.
//
//            setRadius (0.5f);     // size of bounding sphere
//
//            setMaxForce (0.1f);   // steering force is clipped to this magnitude
//            setMaxSpeed (1.0f);   // velocity is clipped to this magnitude
//
//            // reset bookkeeping to do running averages of these quanities
//            resetSmoothedPosition ();
//            resetSmoothedCurvature ();
//            resetSmoothedAcceleration ();
//        }
//
//        // get/set mass
//        float mass (void) const {return _mass;}
//        float setMass (float m) {return _mass = m;}
//
//        // get velocity of vehicle
//        reVector3Df velocity (void) const {return forward() * _speed;}
//
//        // get/set speed of vehicle  (may be faster than taking mag of velocity)
//        float speed (void) const {return _speed;}
//        float setSpeed (float s) {return _speed = s;}
//
//        // size of bounding sphere, for obstacle avoidance, etc.
//        float radius (void) const {return _radius;}
//        float setRadius (float m) {return _radius = m;}
//
//        // get/set maxForce
//        float maxForce (void) const {return _maxForce;}
//        float setMaxForce (float mf) {return _maxForce = mf;}
//
//        // get/set maxSpeed
//        float maxSpeed (void) const {return _maxSpeed;}
//        float setMaxSpeed (float ms) {return _maxSpeed = ms;}
//
//        // ratio of speed to max possible speed (0 slowest, 1 fastest)
//        float RelativeSpeed (void) const {return speed () / maxSpeed ();}
//
//
//        // apply a given steering force to our momentum,
//        // adjusting our orientation to maintain velocity-alignment.
//        void ApplySteeringForce (const reVector3Df& force, const float deltaTime);
//
//        // the default version: keep FORWARD parallel to velocity, change
//        // UP as little as possible.
//        virtual void regenerateLocalSpace (const reVector3Df& newVelocity,
//                                           const float elapsedTime);
//
//        // alternate version: keep FORWARD parallel to velocity, adjust UP
//        // according to a no-basis-in-reality "banking" behavior, something
//        // like what birds and airplanes do.  (XXX experimental cwr 6-5-03)
//        void regenerateLocalSpaceForBanking (const reVector3Df& newVelocity,
//                                             const float elapsedTime);
//
//        // adjust the steering force passed to ApplySteeringForce.
//        // allows a specific vehicle class to redefine this adjustment.
//        // default is to disallow backward-facing steering at low speed.
//        // xxx experimental 8-20-02
//        virtual reVector3Df adjustRawSteeringForce (const reVector3Df& force,
//                                             const float deltaTime);
//
//        // apply a given braking force (for a given dt) to our momentum.
//        // xxx experimental 9-6-02
//        void applyBrakingForce (const float rate, const float deltaTime);
//
//        // predict position of this vehicle at some time in the future
//        // (assumes velocity remains constant)
//        reVector3Df predictFuturePosition (const float predictionTime) const;
//
//        // get instantaneous Curvature (since last update)
//        float Curvature (void) const {return _curvature;}
//
//        // get/reset smoothedCurvature, smoothedAcceleration and smoothedPosition
//        float smoothedCurvature (void) {return _smoothedCurvature;}
//        float resetSmoothedCurvature (float value = 0)
//        {
//            _lastForward = reVector3Df::ZERO;
//            _lastPosition = reVector3Df::ZERO;
//            return _smoothedCurvature = _curvature = value;
//        }
//        reVector3Df smoothedAcceleration (void) {return _smoothedAcceleration;}
//        reVector3Df resetSmoothedAcceleration (const reVector3Df& value = reVector3Df::ZERO)
//        {
//            return _smoothedAcceleration = value;
//        }
//        reVector3Df smoothedPosition (void) {return _smoothedPosition;}
//        reVector3Df resetSmoothedPosition (const reVector3Df& value = reVector3Df::ZERO)
//        {
//            return _smoothedPosition = value;
//        }
//
//        // give each vehicle a unique number
//        int serialNumber;
//        static int serialNumberCounter;
//
//        // draw lines from vehicle's position showing its velocity and acceleration
//        void annotationVelocityAcceleration (float maxLengthA, float maxLengthV);
//        void annotationVelocityAcceleration (float maxLength)
//            {annotationVelocityAcceleration (maxLength, maxLength);}
//        void annotationVelocityAcceleration (void)
//            {annotationVelocityAcceleration (3, 3);}
//
//        // set a random "2D" heading: set local Up to global Y, then effectively
//        // rotate about it by a random angle (pick random forward, derive side).
//        void RandomizeHeadingOnXZPlane (void)
//        {
//			setUp (reVector3Df::UNIT_Y);
//            setForward (Vec3Utilities::RandomUnitVectorOnXZPlane ());
//            setSide (localRotateForwardToSide (forward()));
//        }
//
//    private:
//
//        float _mass;       // mass (defaults to unity so acceleration=force)
//
//        float _radius;     // size of bounding sphere, for obstacle avoidance, etc.
//
//        float _speed;      // speed along Forward direction.  Because local space
//                           // is velocity-aligned, velocity = Forward * Speed
//
//        float _maxForce;   // the maximum steering force this vehicle can apply
//                           // (steering force is clipped to this magnitude)
//
//        float _maxSpeed;   // the maximum speed this vehicle is allowed to move
//                           // (velocity is clipped to this magnitude)
//
//        float _curvature;
//        reVector3Df _lastForward;
//        reVector3Df _lastPosition;
//        reVector3Df _smoothedPosition;
//        float _smoothedCurvature;
//        reVector3Df _smoothedAcceleration;
//
//        // measure path Curvature (1/turning-radius), maintain smoothed version
//        void measurePathCurvature (const float elapsedTime);
//    };
//
//
//}
//}
//    
//#endif // OPENSTEER_SIMPLEVEHICLE_H
