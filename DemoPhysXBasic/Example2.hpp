// Shows debugging
#include "ExampleApplication.h"
#include "NxOgre.h"
#include "NxOgreRender/NxOgreOGRE3D.h"
 
class Example2Listener : public ExampleFrameListener
{ 
public:
    Example2Listener(RenderWindow *win, Camera *cam, NxOgre::VisualDebugger* visualDebugger, Ogre::SceneNode* node) 
        : ExampleFrameListener(win, cam)
    {
        mTimeController = NxOgre::TimeController::getSingleton();

		// added
        mVisualDebugger = visualDebugger;
        mVisualDebuggerNode = node;
    }
 
    bool frameStarted(const FrameEvent& evt)
    {
        mTimeController->advance(evt.timeSinceLastFrame);

		// added
        mVisualDebugger->draw();
        mVisualDebuggerNode->needUpdate();
 
        return ExampleFrameListener::frameStarted(evt);
    }
 
protected:
    NxOgre::TimeController*        mTimeController;

	// added
    NxOgre::VisualDebugger*        mVisualDebugger;
    Ogre::SceneNode*            mVisualDebuggerNode;
};
 
class Example2 : public ExampleApplication
{
protected:
    NxOgre::World*            mWorld;
    NxOgre::Scene*            mScene;
    NxOgre::TimeController*        mTimeController;
    OGRE3DRenderSystem*        mRenderSystem;
 
    OGRE3DBody*            mCube;
    OGRE3DBody*            mCubeTwo;

	// To add debugging
	NxOgre::VisualDebugger*    mVisualDebugger;
	OGRE3DRenderable*			mVisualDebuggerRenderable;
	Ogre::SceneNode*			mVisualDebuggerNode;
 
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
 
        // Create the world
        mWorld = NxOgre::World::createWorld();
		
		// added for debugging
		mVisualDebugger = mWorld->getVisualDebugger();		
		mVisualDebuggerRenderable = new OGRE3DRenderable(NxOgre::Enums::RenderableType_VisualDebugger);
		mVisualDebugger->setRenderable(mVisualDebuggerRenderable);
		mVisualDebuggerNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
		mVisualDebuggerNode->attachObject(mVisualDebuggerRenderable);
		mVisualDebugger->setVisualisationMode(NxOgre::Enums::VisualDebugger_ShowAll);
 
        // Create scene description
        NxOgre::SceneDescription sceneDesc;
        sceneDesc.mGravity = NxOgre::Vec3(0, -9.8f, 0);
        sceneDesc.mName = "DemoScene";
 
        // Create scene
        mScene = mWorld->createScene(sceneDesc);
 
        // Set some physical scene values
        mScene->getMaterial(0)->setStaticFriction(0.5);
        mScene->getMaterial(0)->setDynamicFriction(0.5);
        mScene->getMaterial(0)->setRestitution(0.1);
 
        // Create render system
        mRenderSystem = new OGRE3DRenderSystem(mScene);
 
        //Create time controller
        mTimeController = NxOgre::TimeController::getSingleton();
 
        // Add objects
        mCube = mRenderSystem->createBody(new NxOgre::Box(1, 1, 1), NxOgre::Vec3(0, 40, 0), "cube.1m.mesh");
        mCubeTwo = mRenderSystem->createBody(new NxOgre::Box(1, 1, 1), NxOgre::Vec3(20, 45, 0), "cube.1m.mesh");
        mCubeTwo->addForce(NxOgre::Vec3(-800, -200, 0), NxOgre::Enums::ForceMode_Force);
 
        // Create floor plane (BloodyMess)
        mScene->createSceneGeometry(new NxOgre::PlaneGeometry(0, NxOgre::Vec3(0, 1, 0)), Matrix44_Identity);
 
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
        mFrameListener = new Example2Listener(mWindow, mCamera, mVisualDebugger, mVisualDebuggerNode);
		mRoot->addFrameListener(mFrameListener);
    }
};
