#if 0
/** TutorialApplication.cpp */

/** Include example application headerfile */
#include "ExampleApplication.h"
//#include "OgreOde\OgreOde_Core.h"
#include "OgreOde_Core.h"

class TutorialFrameListener : public ExampleFrameListener
{
protected:
  OgreOde::World* mWorld;
  OgreOde::StepHandler* mStepper;

public:
    TutorialFrameListener(RenderWindow* win, Camera* cam, OgreOde::World* _world, OgreOde::StepHandler* _stepper)
        : ExampleFrameListener(win, cam), mWorld(_world), mStepper(_stepper)
    {
    }

	//bool frameStarted(const FrameEvent& evt)
	bool frameEnded(const FrameEvent& evt)
	{
		Real time = evt.timeSinceLastFrame;
		if ( mStepper->step(time) )
			mWorld->synchronise();
		return ExampleFrameListener::frameEnded(evt);
	}
};

/** Class TutorialApplication */
class TutorialApplication : public ExampleApplication
{
protected:

public:
	TutorialApplication(void)
	{
	}

	~TutorialApplication(void)
	{
	}

protected:
	void createScene(void)
	{

		/** Create ambient light */
		mSceneMgr->setAmbientLight(ColourValue(1, 1, 1));

		/** Create point light, set its type and position */
		Light *light = mSceneMgr->createLight("Light1");
		light->setType(Light::LT_POINT);
		light->setPosition(Vector3(0, 150, 250));

		/** set the diffuse and specular colour */
		light->setDiffuseColour(1.0, 1.0, 1.0);
		light->setSpecularColour(1.0, 1.0, 1.0);

		/** Add physics to Ogre */
		mWorld = new OgreOde::World(mSceneMgr);
		mWorld->setGravity(Ogre::Vector3(0, -9.80665, 0));
		mWorld->setCFM(10e-5);
		mWorld->setERP(0.8);
		mWorld->setAutoSleep(true);
		mWorld->setAutoSleepAverageSamplesCount(10);
		mWorld->setContactCorrectionVelocity(1.0);
		mSpace = mWorld->getDefaultSpace();

		const Ogre::Real _time_step = 0.5;
		const Ogre::Real time_scale = Ogre::Real(1.7);
		const Ogre::Real max_frame_time = Ogre::Real(1.0 / 4);

		mStepper = new OgreOde::StepHandler(mWorld, OgreOde::StepHandler::QuickStep, _time_step, max_frame_time, time_scale);

		mGround = new OgreOde::InfinitePlaneGeometry(Plane(Ogre::Vector3(0,1,0),0), mWorld, mWorld->getDefaultSpace());
		
		//load meshed
		int i = 0;
		StaticGeometry *s;
		s = mSceneMgr->createStaticGeometry("StaticFloor");
		s->setRegionDimensions(Ogre::Vector3(160.0, 100.0, 160.0));

		//set the region origin so the center is at 0 world
		s->setOrigin(Ogre::Vector3::ZERO);

		for(Real z = -80.0; z <= 80.0; z += 20.0)
		{
			for(Real x = -80.0; x <= 80.0; x += 20.0)
			{
				String name = String("Ground") + StringConverter::toString(i++);
				Entity *entity = mSceneMgr->createEntity(name, "plane.mesh");
				entity->setQueryFlags(1<<4);
				entity->setUserObject(mGround);
				entity->setCastShadows(false);
				s->addEntity(entity, Ogre::Vector3(x,0,z));
			}
		}
		s->build();

		mEntity = mSceneMgr->createEntity("crate", "crate.mesh");
		mEntity->setQueryFlags(1<<2);
		mNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("crate");
		mNode->attachObject(mEntity);
		//mEntity->setNormaliseNormals(true);
		mEntity->setCastShadows(true);

		//create a body for our crate
		//and "register" it in our world and the scene node
		mBody = new OgreOde::Body(mWorld);
		mNode->attachObject(mBody);

		//set size and mass of the box
		Vector3 size(10.0,10.0,10.0);
		OgreOde::BoxMass mMass(0.5, size);
		mMass.setDensity(5.0, size);
		mGeom = (OgreOde::Geometry*) new OgreOde::BoxGeometry(size, mWorld, mSpace);
		mNode->setScale(size.x * 0.1, size.y * 0.1, size.z * 0.1);
		mBody->setMass(mMass);
		mGeom->setBody(mBody);
		mEntity->setUserObject(mGeom);

		//set the orientation and position of the crate
		mBody->setOrientation(Quaternion(Radian(5.0), Ogre::Vector3(0,0,0)));
		mBody->setPosition(Vector3(0,120,-20));
	}

	//create framelistener
	void createFrameListener(void)
	{
		mFrameListener = new TutorialFrameListener(mWindow, mCamera, mWorld, mStepper);
		mRoot->addFrameListener(mFrameListener);
	}

private:
	OgreOde::World			*mWorld;
	OgreOde::Space			*mSpace;
	OgreOde::StepHandler	*mStepper;

	OgreOde::InfinitePlaneGeometry	*mGround;
	OgreOde::Body					*mBody;
	OgreOde::Geometry				*mGeom;
	OgreOde::BoxMass				*mMass;
	Ogre::SceneNode					*mNode;
	Ogre::Entity					*mEntity;
};

#define WIN32_LEAN_AND_MEAN
#include "windows.h"

int main(int argc, char **argv)
{
	/** Create application object */
	TutorialApplication app;

	try
	{
		app.go();
	}
	catch(Exception e)
	{
		fprintf(stderr, "An exception has occured: %s\n", e.getFullDescription().c_str());
	}

	return 0;
}
#endif