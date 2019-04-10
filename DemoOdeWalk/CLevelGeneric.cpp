#ifdef COMPILE_ODE

#include "CLevelGeneric.h"
#include "CPhysicsManager.h"
#include "CCameraManager.h"
#include "CPhysicsStrategy_Ode.h"
#include "CGenericCollisionListener_Ode.h"

using namespace Core;
using namespace Core::Physics;

CPhysicsStrategy_Ode* physics = 0;

CLevelGeneric::CLevelGeneric()
{
}

CLevelGeneric::~CLevelGeneric()
{
}

void CLevelGeneric::Update(const f32& elapsedTime)
{
	physics->Update(elapsedTime);
	CCameraManager::Instance()->Update(elapsedTime);
}

void CLevelGeneric::End()
{
	m_GameManager->RemoveResourceArchive(".\\Media.zip", "CLevelGeneric");
}

bool CLevelGeneric::Load()
{
	m_GameManager->AddResourceArchive(".\\Media.zip", "CLevelGeneric");

	m_SceneManager->setShadowTechnique(SHADOWTYPE_TEXTURE_MODULATIVE);
	m_SceneManager->setShadowTextureSize(512);
	m_SceneManager->setShadowColour(ColourValue(0.6, 0.6, 0.6));

	// Setup animation default
	Animation::setDefaultInterpolationMode(Animation::IM_LINEAR);
	Animation::setDefaultRotationInterpolationMode(Animation::RIM_LINEAR);

	// Set ambient light
	m_SceneManager->setAmbientLight(ColourValue(0.5, 0.5, 0.5));

	// Give it a little ambience with lights
	Light* l;
	l = m_SceneManager->createLight("light");
	l->setType(Light::LT_SPOTLIGHT);
	l->setPosition(-200,150,-100);
	reVector3Df dir(-l->getPosition());
	dir.normalise();
	l->setDirection(dir);
	l->setDiffuseColour(0.5, 0.5, 0.5);

    // Position the camera
	Camera* camera = m_GameManager->GetCurrCamera()->GetCamera();
    camera->setPosition(100,20,0);
    camera->lookAt(0,10,0);

	// create some ground
	physics = (CPhysicsStrategy_Ode*)CPhysicsManager::Instance()->CreateStrategy("physics");
	physics->InitWorld();
	CGenericCollisionListener_Ode* listener = new CGenericCollisionListener_Ode();
	physics->SetCollisionListener(listener);
	m_GameManager->AddResourceArchive(".\\Media.zip", "OgreOde");
	physics->ShowDebug(true);

	// Create a default plane to act as the ground
	Plane* p = new Plane(reVector3Df(0, 1, 0), 0);
	s32 num = physics->AddInfinitePlane(p);
	OgreOde::Geometry* plane =  physics->GetGeometry(num);

	// Use a load of meshes to represent the floor
    int i = 0;
    StaticGeometry* s;
    s = m_SceneManager->createStaticGeometry("StaticFloor");
    s->setRegionDimensions(reVector3Df(160.0, 100.0, 160.0));
    // Set the region origin so the center is at 0 world
    s->setOrigin(reVector3Df::ZERO);
	for (Real z = -80.0;z <= 320.0;z += 20.0)
	{
		for (Real x = -80.0;x <= 320.0;x += 20.0)
		{
			String name = String("Plane_") + StringConverter::toString(i++);			
			Entity* entity = m_SceneManager->createEntity(name, "plane.mesh");
			entity->setCastShadows(false);
			entity->setUserAny(Any(plane));
            s->addEntity(entity, reVector3Df(x, 0, z));
		}
    }
    s->build();

	// Fill the scene with some creates
	SceneNode *node;
	Entity *entity;
	for (unsigned int i = 0; i < 5; ++i) 
	{
		node = m_SceneManager->getRootSceneNode()->createChildSceneNode(
			StringConverter::toString(i), 
			reVector3Df(10 * i, 100 * i, 10));
		node->setScale(0.1, 0.1, 0.1);
		entity = m_SceneManager->createEntity(StringConverter::toString(i), "crate.mesh");
		node->attachObject(entity);

		physics->AddCube(node, entity, 1.0f, 1.0f, 0.01f);
	}

	// setup player
	CCameraManager::Instance()->CreateCamera(ECM_FREE);
	m_Player = new CFpsPlayer("player", physics);
	m_Player->SetCamera(m_GameManager->GetCurrCamera());

	// Setup FrameListener
	m_FrameListenerFramework = new CFpsFrameListener(m_GameManager->GetRenderWindow());
	m_GameManager->GetEngineDevice()->addFrameListener(m_FrameListenerFramework);

	return true;
}

#pragma lib("OgreOde.lib");

#include "CGame.h"

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#endif

int main(int argc, char **argv)
{
	Core::CGame app;
    try 
	{
		app.AddGameState(new CLevelGeneric());
		app.Run();
    } 
	catch( Ogre::Exception& e ) 
	{
		fprintf(stderr, "An exception has occurred: %s\n", e.what());
    }
    
    return 0;
}

#else

void main() { }

#endif