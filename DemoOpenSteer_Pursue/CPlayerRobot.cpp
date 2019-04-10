#include "CPlayerRobot.h"
#include "Prefab/OgreOde_Prefab.h"
#include "CPhysicsStrategy_Ode.h"
#include "CFpsVehicle.h"

using namespace Core::AI;
using namespace Core::Physics;

CPlayerRobot::CPlayerRobot(const char* Name, Physics::IPhysicsStrategy* PhysicsStrategy) : CPlayer(Name, PhysicsStrategy)
{
	SceneManager* m_SceneManager = CGameManager::Instance()->GetSceneManager();

	// Setup basic node structure to handle 3rd person cameras
	m_PlayerNode = m_SceneManager->getRootSceneNode()->createChildSceneNode(Name);
	m_SightNode = NULL;
	m_CameraNode = NULL;

	// Create the entity
	m_Entity = m_SceneManager->createEntity(Name, "robot.mesh");

	// Create the scene node
	m_PlayerNode->setPosition(reVector3Df(0.0f, 0.0f, 0.0f));
	m_PlayerNode->setScale(0.2f, 0.2f, 0.2f);
	m_PlayerNode->attachObject(m_Entity);

	// Setup AI
	m_AbstractVehicle = new CFpsVehicle();
	m_AbstractVehicle->setUp(reVector3Df(0, 0, -1));
	m_AbstractVehicle->setPosition(m_PlayerNode->getPosition());
    m_AbstractVehicle->setSpeed (1.0f);         // speed along Forward direction.
    m_AbstractVehicle->setMaxForce (2.0f);     // steering force is clipped to this magnitude
    m_AbstractVehicle->setMaxSpeed (5.0f);     // velocity is clipped to this magnitude
	m_AbstractVehicle->setRadius (5.0f);
}

CPlayerRobot::~CPlayerRobot()
{
	CGameManager::Instance()->GetSceneManager()->destroySceneNode(Name);
}

void CPlayerRobot::Update(f32 elapsedTime, OIS::Keyboard* Keyboard, OIS::Mouse* Mouse)
{
	// Do we need to update our animation
	if(m_AnimState != m_CurrAnimState)
	{
		if(m_AnimState == EAS_WALK)
		{
			m_AnimationState = m_Entity->getAnimationState("Walk");
			m_AnimationState->setLoop(true);
			m_AnimationState->setEnabled(true);
		}
		else if(m_AnimState == EAS_NONE)
		{
			m_AnimationState->setEnabled(false);
			delete m_AnimationState;
		}
		m_CurrAnimState = m_AnimState;
	}

	if(m_AnimationState)
		m_AnimationState->addTime(elapsedTime * 0.4f);
}

void CPlayerRobot::SetupPhysics(SceneManager* sm)
{

}

void CPlayerRobot::update(const float currentTime, const float elapsedTime)
{
	Update(elapsedTime, 0, 0);
}

void CPlayerRobot::OnHit(CWeapon* Weapon, const reVector3Df* Direction, void* physics)
{
}