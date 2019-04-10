/*
SimpleScenesApplication.h
------------------------
Defintion of the class responsible for running the OgreOde
demo scenes. Wrangles input and the various different test
scenes into a single application. Extends the ExampleApplication
and ExampleFrameListener classes.
*/
#ifndef _SIMPLESCENESAPPLICATION_H_
#define _SIMPLESCENESAPPLICATION_H_

// Include the OgreOde interface which includes Ogre itself
#include "OgreOde_Core.h"
#include "ExampleApplication.h"

#if !(OGRE_VERSION_MINOR < 4)
//using namespace OIS;
#endif //OGRE_VERSION not heihort

// Our framework class for the different tests
#include "SimpleScenes.h"

class SimpleScenesApplication;

/*
A frame listener that does default processing before deferring to the main application
*/
class SimpleScenesFrameListener:public ExampleFrameListener
{
public:
	// Standard example constructor with one additional parameter
	SimpleScenesFrameListener(SimpleScenesApplication* demo,RenderWindow* win,Camera* cam) : 
      ExampleFrameListener(win,cam),
      _demo(demo)
	{
		mMoveSpeed = 15.0;
	}
	~SimpleScenesFrameListener(){}

	bool frameStarted(const FrameEvent& evt);
	bool frameEnded(const FrameEvent& evt);

protected:
	// Keep track of the application that created us
	SimpleScenesApplication* _demo;
};

/*
The test application, based on the Ogre example application for consistency
*/
class SimpleScenesApplication: public ExampleApplication
{
	// The frame listener gets access to some special things
	friend class SimpleScenesFrameListener;

public:
	// Standard constructor/destructor
    SimpleScenesApplication() : 
      ExampleApplication()
	{
		_test = 0;
		_plane = 0;
		_stepper = 0;	
		_world = 0;

		_time_elapsed = 0.0;
		_time_step = SimpleScenes::STEP_RATE;
		_looking = _chasing = false;
		_paused = false;
	}
    ~SimpleScenesApplication();

protected:
	// Override stuff from the base class
    void createScene(void);	
    void createFrameListener(void)
    {
        mFrameListener= new SimpleScenesFrameListener(this,mWindow,mCamera);
        mRoot->addFrameListener(mFrameListener);
    }
    void chooseSceneManager(void)
    {
        mSceneMgr = mRoot->createSceneManager( ST_GENERIC, "basicsm" );
    }

	// Add the standard resources, plus our own pack
	void setupResources(void)
    {
		ExampleApplication::setupResources(); 
		ResourceGroupManager *rsm = ResourceGroupManager::getSingletonPtr();
		StringVector groups = rsm->getResourceGroups();   

		FileInfoListPtr finfo =  ResourceGroupManager::getSingleton().findResourceFileInfo (
			"Bootstrap", "axes.mesh");

		const bool isSDK =  (!finfo->empty()) && 
			StringUtil::startsWith (finfo->begin()->archive->getName(), "./media/packs/ogrecore.zip", true);
 
		{

			const String groupName("OgreOde");    
			if (std::find(groups.begin(), groups.end(), String(groupName)) == groups.end())
			{
				if (isSDK)
				{
					rsm->addResourceLocation("./Media","FileSystem", groupName);
				}
				else
				{
					rsm->addResourceLocation("./Media","FileSystem", groupName);
				}
			}
		}


		/*{
			const String groupName("Kuniverse");    
			if (std::find(groups.begin(), groups.end(), String(groupName)) == groups.end())
			{
				rsm->createResourceGroup(groupName);
				String folderBase;("../../../../../lib/cvs/KUniverse/medias/");
				if (isSDK)
				{
					folderBase = ("../../../../../lib/cvs/KUniverse/medias/");
				}	
				else
				{
					folderBase = "../../../../../../KUniverse/medias/";
				}

				rsm->addResourceLocation(folderBase,"FileSystem", groupName);
				rsm->addResourceLocation(folderBase + "gfx/3d/entities/bship","FileSystem", groupName);
				rsm->addResourceLocation(folderBase + "gfx/3d/entities/bull","FileSystem", groupName);
				rsm->addResourceLocation(folderBase + "gfx/3d/entities/mothership","FileSystem", groupName);
				rsm->addResourceLocation(folderBase + "gfx/3d/entities/tank","FileSystem", groupName);
				rsm->addResourceLocation(folderBase + "gfx/3d/entities/trooper","FileSystem", groupName);
				rsm->addResourceLocation(folderBase + "gfx/3d/buildings","FileSystem", groupName);

				rsm->addResourceLocation(folderBase + "gfx/3d/envir","FileSystem", groupName);
				rsm->addResourceLocation(folderBase + "gfx/3d/envir/fish","FileSystem", groupName);
				rsm->addResourceLocation(folderBase + "gfx/3d/envir/grass","FileSystem", groupName);
				rsm->addResourceLocation(folderBase + "gfx/3d/envir/metal","FileSystem", groupName);
				rsm->addResourceLocation(folderBase + "gfx/3d/envir/palm","FileSystem", groupName);
				rsm->addResourceLocation(folderBase + "gfx/3d/envir/rock","FileSystem", groupName);
				rsm->addResourceLocation(folderBase + "gfx/3d/envir/sky","FileSystem", groupName);
				rsm->addResourceLocation(folderBase + "gfx/3d/envir/water","FileSystem", groupName);
				rsm->addResourceLocation(folderBase + "gfx/3d/envir/clouds","FileSystem", groupName);

				rsm->addResourceLocation(folderBase + "gfx/3d/effects/","FileSystem", groupName);
				rsm->addResourceLocation(folderBase + "gfx/3d/effects/smoke","FileSystem", groupName);

				rsm->addResourceLocation(folderBase + "gfx/gui/","FileSystem", groupName);

				rsm->addResourceLocation(folderBase + "sound/","FileSystem", groupName);
				rsm->addResourceLocation(folderBase + "video","FileSystem", groupName);
				
			}
		}*/


	}

    // The frame listener will call these	

#if (OGRE_VERSION_MINOR < 4)
    void frameStarted(const FrameEvent& evt,InputReader* mInputDevice);
    void frameEnded(const FrameEvent& evt,InputReader* mInputDevice);
#else
    void frameStarted(const FrameEvent& evt, OIS::Keyboard* input, OIS::Mouse* mouse);
    void frameEnded(const FrameEvent& evt, OIS::Keyboard* input, OIS::Mouse* mouse);
#endif //OGRE_VERSION not heihort


	
protected:
	OgreOde::World*					_world;
	OgreOde::StepHandler*			_stepper;
	OgreOde::InfinitePlaneGeometry* _plane;
	SimpleScenes*					_test;

	Real	_delay,_time_elapsed,_time_step;
	Light*	_spot;
	bool	_looking,_chasing,_paused;
};

#endif
