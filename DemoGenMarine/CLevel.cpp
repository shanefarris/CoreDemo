#ifdef COMPILE_ODE

#include "CLevel.h"
#include "CPhysicsManager.h"
#include "CPhysicsStrategy_Ode.h"
#include "OgreOde_Core.h"
#include "Prefab/OgreOde_Prefab.h"
#include "CFpsSoliderPlayer.h"
#include "CGenericCollisionListener_Ode.h"

using namespace Core;
using namespace Core::Physics;

CPhysicsStrategy_Ode* physics = 0;
OgreOde_Prefab::Ragdoll* ragdoll = 0;
f32 _animation_speed = 1.0;

CLevelGeneric::CLevelGeneric()
{
}

CLevelGeneric::~CLevelGeneric()
{
}

void CLevelGeneric::Update(f32 elapsedTime, OIS::Keyboard* Input, OIS::Mouse* Mouse)
{
	// turn to stone to improve fps, better way to do that would be to tweak
	// simulation parameters to be less jitty. better auto sleep
	if (ragdoll->isPhysical() && 
		!ragdoll->isStatic() && 
		ragdoll->isAwake() && 
		ragdoll->_timeSincePhysical > 50.0)
	{
		//radgoll->turnToStone();
		ragdoll->sleep();
	}
    else if (ragdoll->isPhysical())
    {
        if (!ragdoll->isStatic())
        {
			ragdoll->_timeSincePhysical += elapsedTime;
            ragdoll->update();	// RagDoll Activated
        }
    }
    else
    {
        ragdoll->getAnimationState("Walk")->addTime(elapsedTime * 1.0);

        ragdoll->getParentNode ()->translate(
            ragdoll->getParentNode ()->getOrientation() * (Vector3::UNIT_Z * elapsedTime * 2.5));
        
    }

	physics->Update(elapsedTime);	
}

void CLevelGeneric::End()
{
	m_GameManager->RemoveResourceArchive(".\\Media.zip", "OgreOde");
}

bool CLevelGeneric::Load()
{
	m_GameManager->AddResourceArchive(".\\Media.zip", "OgreOde");
	m_GameManager->GetEngineDevice()->setFrameSmoothingPeriod(5.0f);

	physics = (CPhysicsStrategy_Ode*)CPhysicsManager::Instance()->CreateStrategy("physics");
	physics->InitWorld();
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
            //entity->setQueryFlags (STATIC_GEOMETRY_QUERY_MASK);
            //entity->setUserAny(Any(plane));
			entity->setCastShadows(false);
            s->addEntity(entity, reVector3Df(x, 0, z));
		}
    }
    s->build();

	//for(int i = 0; i < 5; i++)
	//{
	//	// Setup ragdoll scene node
	//	SceneNode* node = m_SceneManager->getRootSceneNode()->createChildSceneNode("Zombie_Node" + StringConverter::toString(i));
	//	node->yaw(Degree(rand() % 360));
	//	node->setPosition(0.0,0.0,0);

	//	// Create the ragdoll
	//	ragdoll = (OgreOde_Prefab::Ragdoll*)physics->LoadPrefabRagdoll(node, "zombie.ogreode", "zombie");

	//	// Enable animation on the ragdoll
	//	ragdoll->releasePhysicalControl();
	//	ragdoll->getAnimationState("Walk1")->setEnabled(true);
	//}

	// Setup solider ragdoll
	SceneNode* node = m_SceneManager->getRootSceneNode()->createChildSceneNode("GenMarine_Node" + StringConverter::toString(i));
	node->setPosition(0.0,0.0,0);
	ragdoll = (OgreOde_Prefab::Ragdoll*)physics->LoadPrefabRagdoll(node, "GenMarine.ogreode", "Gen-Marine");
	ragdoll->releasePhysicalControl();
	ragdoll->getAnimationState("Walk")->setEnabled(true);

	// Set default collision listener
	Core::Physics::CGenericCollisionListener_Ode* listener = new Core::Physics::CGenericCollisionListener_Ode();
	physics->SetCollisionListener(listener);

	// setup player
	m_Player = new CFpsSoliderPlayer("Player", physics);
	m_Player->SetCamera(m_GameManager->GetCurrCamera());

	// setup camera
	m_GameManager->GetCurrCamera()->SetMode(ECM_CHASE);
	Camera* cam = m_GameManager->GetCurrCamera()->GetCamera();

	// Setup FrameListener
	m_FrameListenerFramework = new CFpsFrameListener(m_GameManager->GetRenderWindow());
	m_GameManager->GetEngineDevice()->addFrameListener(m_FrameListenerFramework);

	return true;
}

#pragma lib("OgreOde.lib);

#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#include "CGame.h"

int main(int argc, char **argv);
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT argc) { return main(1, &strCmdLine); }

int main(int argc, char **argv)
{
	Core::CGame game;
    try 
	{
		game.AddGameState(new CLevelGeneric());
		game.Run();
    } 
	catch( Ogre::Exception& e ) 
	{
		fprintf(stderr, "An exception has occurred: %s\n", e.what());
    }
    
    return 0;
}

#else

int main()
{
	return 0;
}

#endif
