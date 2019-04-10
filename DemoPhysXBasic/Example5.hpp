// Demos the framework integration
#include "ExampleApplication.h"
#include "NxOgre.h"
#include "CPhysicsStrategy_PhysX.h"
#include "CPhysicsManager.h"
 
using namespace Core::Physics;

class Example5Listener : public ExampleFrameListener
{ 
public:
    Example5Listener(RenderWindow *win, Camera *cam, IPhysicsStrategy* p) 
        : ExampleFrameListener(win, cam)
    {
		physics = p;
    }
 
    bool frameStarted(const FrameEvent& evt)
    {
		if(physics)
			physics->Update(evt.timeSinceLastFrame);
        return ExampleFrameListener::frameStarted(evt);
    }
 
protected:
	IPhysicsStrategy* physics;
    NxOgre::TimeController* mTimeController;
};
 
class Example5 : public ExampleApplication
{
protected:
	IPhysicsStrategy* physics;
	 
    void createScene()
    {
        // Set ambient light
        mSceneMgr->setAmbientLight(ColourValue(0.5f, 0.5f, 0.5f));
 
        // Create a light
        Light* l = mSceneMgr->createLight("MainLight");
        l->setPosition(20, 80, 50);
 
        // Position the camera
        mCamera->setPosition(0, 20, 80);
        mCamera->lookAt(0, 20, 0);
 
        // Create the physics strategy
        physics = CPhysicsManager::Instance()->CreateStrategy("physics", EPS_PHYSX);
		physics->InitWorld();
 
        // Add objects
		Entity *ent = mSceneMgr->createEntity("cube", "cube.1m.mesh");
		Ogre::SceneNode* node = mSceneMgr->getRootSceneNode()->createChildSceneNode();
		node->attachObject(ent);
		node->setPosition(0, 40, 0);

		// Add phyiscs
		physics->AddCube(node, ent, 0, 0, 0);
        //mCube = mRenderSystem->createBody(new NxOgre::Box(1, 1, 1), NxOgre::Vec3(0, 40, 0), "cube.1m.mesh");
        //mCubeTwo = mRenderSystem->createBody(new NxOgre::Box(1, 1, 1), NxOgre::Vec3(20, 45, 0), "cube.1m.mesh");
        //mCubeTwo->addForce(NxOgre::Vec3(-800, -200, 0), NxOgre::Enums::ForceMode_Force);
 
        // Create floor plane (BloodyMess)
        physics->AddInfinitePlane(NULL);
 
        // Create floor plane (Ogre)
        MovablePlane *plane = new MovablePlane("Plane");
        plane->d = 0;
        plane->normal = Vector3::UNIT_Y;
        Ogre::MeshManager::getSingleton().createPlane("PlaneMesh", 
            ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, 
            *plane, 120, 120, 1, 1, true, 1, 3, 3, Vector3::UNIT_Z);
        Entity *planeEnt = mSceneMgr->createEntity("PlaneEntity", "PlaneMesh");
        planeEnt->setMaterialName("Examples/GrassFloor");
 
        Ogre::SceneNode* mPlaneNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
        mPlaneNode->attachObject(planeEnt);
    }
 
    // Create a new frame listener
    void createFrameListener()
    {
        mFrameListener = new Example5Listener(mWindow, mCamera, physics);
        mRoot->addFrameListener(mFrameListener);
    }
};