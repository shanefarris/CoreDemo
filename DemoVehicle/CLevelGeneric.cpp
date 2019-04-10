#ifdef COMPILE_ODE

#include "CLevelGeneric.h"
#include "CPhysicsManager.h"
#include "CPhysicsStrategy_Ode.h"
#include "CVehiclePlayer.h"
#include "CCameraManager.h"

using namespace Core;
using namespace Core::Physics;

CLevelGeneric::CLevelGeneric() : m_Terrain(0)
{
}

CLevelGeneric::~CLevelGeneric()
{
	CPhysicsManager::Instance()->DeleteStrategy("CLevelGeneric");
	delete m_RaySceneQuery;
}

void CLevelGeneric::Update(const f32& deltaTime)
{
	m_PhysicsStrategy->Update(deltaTime);
}

void CLevelGeneric::End()
{
}

bool CLevelGeneric::Load()
{
	m_GameManager->AddResourceArchive(".\\Media.zip", "CLevelGeneric");
	CCamera* camera = m_GameManager->GetCurrCamera();

	SetupPhysics();

    m_SceneManager->setAmbientLight(ColourValue(0.5, 0.5, 0.5));	// Set ambient light
    Light* l = m_SceneManager->createLight("MainLight");			// Create a light
    l->setPosition(20,80,50);

    // Fog NB it's VERY important to set this before calling setWorldGeometry 
    // because the vertex program picked will be different
    ColourValue fadeColour(0.5, 0.5, 1.0);
    m_SceneManager->setFog( FOG_LINEAR, fadeColour, .001, 500, 1000);
    m_GameManager->GetRenderWindow()->getViewport(0)->setBackgroundColour(fadeColour);

    // Load terrain
	Entity* TerrainEntity = m_SceneManager->createEntity("Terrain","Terrain.mesh");
	SceneNode* TerrainNode = m_SceneManager->getRootSceneNode()->createChildSceneNode(TerrainEntity->getName());
	TerrainNode->attachObject(TerrainEntity);
	m_PhysicsStrategy->AddStaticTrimesh(TerrainNode, TerrainEntity, 1, 5);

	// Fill the scene with some razors
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

		m_PhysicsStrategy->AddCube(node, entity, 1.0f, 1.0f, 1.0f);
	}

	// Add palm trees
	SceneNode* nodeTrees = m_SceneManager->getRootSceneNode()->createChildSceneNode("trees", reVector3Df(0, 0, 0));
	Entity* entityTrees = m_SceneManager->createEntity("treesEntity", "PalmTrees.mesh");
	nodeTrees->setScale(5.05282, 5.05285, 5.05285);
	nodeTrees->attachObject(entityTrees);

	// Camera
	CCameraManager::Instance()->CreateCamera(ECM_CHASE);
	CCameraManager::Instance()->GetCamera()->lookAt(reVector3Df::UNIT_Z);

	// setup player
	m_Player = new CVehiclePlayer("Player", m_PhysicsStrategy);
	m_Player->SetCamera(m_GameManager->GetCurrCamera());

	// Setup FrameListener
	m_FrameListenerFramework = new CFpsFrameListener(m_GameManager->GetRenderWindow());
	m_GameManager->GetEngineDevice()->addFrameListener(m_FrameListenerFramework);

	return true;
}

void CLevelGeneric::SetupPhysics()
{
	// Setup physics
	m_PhysicsStrategy = (CPhysicsStrategy_Ode*)CPhysicsManager::Instance()->CreateStrategy("CLevelGeneric", EPS_ODE);
	m_PhysicsStrategy->InitWorld();

	m_CollisionListener = new CGenericCollisionListener_Ode();
	m_PhysicsStrategy->SetCollisionListener(m_CollisionListener);

	m_GameManager->AddResourceArchive(".\\Media.zip", "OgreOde");
	m_PhysicsStrategy->ShowDebug(false);
}

#pragma lib("OgreOde.lib");

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

void main() { }

#endif