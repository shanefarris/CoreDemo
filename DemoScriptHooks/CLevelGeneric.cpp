#include "CLevelGeneric.h"
#include "CPhysicsManager.h"
#include "CGameObjectManager.h"

using namespace Core;

CLevelGeneric::CLevelGeneric()
{
}

CLevelGeneric::~CLevelGeneric()
{
}

void CLevelGeneric::Update(f32 elapsedTime, OIS::Keyboard* Input, OIS::Mouse* Mouse)
{
	CGameObjectManager::Instance()->Update(elapsedTime);
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

	// Set ambient light
	m_SceneManager->setAmbientLight(ColourValue(0.5, 0.5, 0.5));

	// Give it a little ambience with lights
	Light* l;
	l = m_SceneManager->createLight("BlueLight");
	l->setType(Light::LT_SPOTLIGHT);
	l->setPosition(-200,150,-100);
	reVector3Df dir(-l->getPosition());
	dir.normalise();
	l->setDirection(dir);
	l->setDiffuseColour(0.5, 0.5, 1.0);

	l = m_SceneManager->createLight("GreenLight");
	l->setType(Light::LT_SPOTLIGHT);
    l->setPosition(0,150,-100);
	dir = -l->getPosition();
	dir.normalise();
	l->setDirection(dir);
    l->setDiffuseColour(0.5, 1.0, 0.5);

    // Setup the camera
	CCameraManager::Instance()->CreateCamera(ECM_CHASE);
	Camera* camera = CCameraManager::Instance()->GetCamera();
    camera->setPosition(100,20,0);
    camera->lookAt(0,10,0);

	// create some ground
	Plane plane;
	plane.normal = reVector3Df::UNIT_Y;
	plane.d = 100;
	MeshManager::getSingleton().createPlane("Myplane",
		ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane,
		1500,1500,20,20,true,1,60,60,reVector3Df::UNIT_Z);
	Entity* pPlaneEnt = m_SceneManager->createEntity( "plane", "Myplane" );
	//pPlaneEnt->setMaterialName("Examples/Rockwall");
	pPlaneEnt->setCastShadows(false);
	m_SceneManager->getRootSceneNode()->createChildSceneNode(reVector3Df(0, 99, 0))->attachObject(pPlaneEnt);

	// setup player
	//m_GameManager->GetCurrCamera()->SetMode(ECM_CHASE);
	//m_Player = CGameObjectManager::Instance()->CreatePlayer(EPT_SOLIDER1, "Player", NULL);
	//m_Player->SetCamera(m_GameManager->GetCurrCamera());
	//m_Player->GetPlayerNode()->setPosition(0, 0, 0);

	// Setup FrameListener
	m_FrameListenerFramework = new CFpsFrameListener(m_GameManager->GetRenderWindow());
	m_FrameListenerFramework->SetPlayer(0);
	m_GameManager->GetEngineDevice()->addFrameListener(m_FrameListenerFramework);

	return true;
}

#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#include "CGame.h"

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