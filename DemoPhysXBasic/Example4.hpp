#include "ExampleApplication.h"
#include "NxOgre.h"
#include "NxOgreRender/NxOgreOGRE3D.h"
 
class Example4Listener : public ExampleFrameListener
{ 
public:
    Example4Listener(RenderWindow *win, Camera *cam) 
        : ExampleFrameListener(win, cam)
    {
        mTimeController = NxOgre::TimeController::getSingleton();
    }
 
    bool frameStarted(const FrameEvent& evt)
    {
        mTimeController->advance(evt.timeSinceLastFrame);
        return ExampleFrameListener::frameStarted(evt);
    }
 
protected:
    NxOgre::TimeController* mTimeController;
};
 
class Example4 : public ExampleApplication
{
protected:
    NxOgre::World*            mWorld;
    NxOgre::Scene*            mScene;
    NxOgre::TimeController*            mTimeController;
    OGRE3DRenderSystem*        mRenderSystem;
 
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
 
        // Create floor plane (BloodyMess)
        mScene->createSceneGeometry(new NxOgre::PlaneGeometry(0, NxOgre::Vec3(0, 1, 0)), NxOgre::Matrix44(Matrix44_Identity));
 
        // Create floor plane (Ogre)
        MovablePlane *plane = new MovablePlane("Plane");
        plane->d = 0;
        plane->normal = Vector3::UNIT_Y;
        Ogre::MeshManager::getSingleton().createPlane("PlaneMesh", 
            ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, 
            *plane, 120, 120, 1, 1, true, 1, 3, 3, Vector3::UNIT_Z);
        Entity *planeEnt = mSceneMgr->createEntity("PlaneEntity", "PlaneMesh");
        planeEnt->setMaterialName("PlaneMat");
 
        Ogre::SceneNode* mPlaneNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
        mPlaneNode->attachObject(planeEnt);
 
        // Open archive and load the meshes
        NxOgre::ResourceSystem::getSingleton()->openArchive("media", "file:media");
        NxOgre::Mesh* convexMesh = NxOgre::MeshManager::getSingleton()->load("media:Barrel.nxs");
        NxOgre::Mesh* triangleMesh = NxOgre::MeshManager::getSingleton()->load("media:Stairs.nxs");
 
        // Create the Convex and the TriangleGeometry
        NxOgre::Convex* convex = new NxOgre::Convex(convexMesh);
        NxOgre::TriangleGeometry* triangleGeometry = new NxOgre::TriangleGeometry(triangleMesh);
 
        // Create the OGRE3DBody with the Convex as its shape
        OGRE3DBody* convexBody = mRenderSystem->createBody(convex, NxOgre::Vec3(0, 30, 0), "Barrel.mesh");
        convexBody->setGlobalOrientation(NxOgre::Matrix33(NxOgre::Quat(0, 45, 0, 45)));
 
        // Create the SceneGeometry
        mScene->createSceneGeometry(triangleGeometry, NxOgre::Matrix44(NxOgre::Vec3(0, 5, 0)));
 
        // Create the Entity and Scenenode to visualize the SceneGeometry
        Ogre::Entity* triangleEntity = mSceneMgr->createEntity("triangleEntity", "Stairs.mesh");
        Ogre::SceneNode* triangleNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
        triangleNode->attachObject(triangleEntity);
        triangleNode->setPosition(Vector3(0, 5, 0));        
    }
 
    // Create a new frame listener
    void createFrameListener()
    {
        mFrameListener = new Example4Listener(mWindow, mCamera);
        mRoot->addFrameListener(mFrameListener);
    }
};
