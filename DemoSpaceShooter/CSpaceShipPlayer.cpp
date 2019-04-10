#include "CSpaceShipPlayer.h"

CSpaceShipPlayer::CSpaceShipPlayer(String Name, Physics::IPhysicsStrategy* PhysicsStrategy) 
	: CPlayer(Name, PhysicsStrategy)
{
	SceneManager* m_SceneManager = CGameManager::Instance()->GetSceneManager();

	// Setup basic node structure to handle 3rd person cameras
	m_PlayerNode = m_SceneManager->getRootSceneNode()->createChildSceneNode(Name);
	m_SightNode = m_PlayerNode->createChildSceneNode (Name + "_sight", reVector3Df(0, 0, 100));
	m_CameraNode = m_PlayerNode->createChildSceneNode (Name + "_camera", reVector3Df(0, 50, -100));

	// Add ship
    Entity *ent = m_SceneManager->createEntity("razor", "razor.mesh");
	m_PlayerNode->attachObject(ent);

	// Setup particles for the ship
	ParticleSystem* pThrusters = m_SceneManager->createParticleSystem("ParticleSys1", 200);

	pThrusters->setMaterialName("Flare");
    pThrusters->setDefaultDimensions( 25, 25 );

    ParticleEmitter *pEmit1 = pThrusters->addEmitter("Point");
    ParticleEmitter *pEmit2 = pThrusters->addEmitter("Point");

    // Thruster 1
    pEmit1->setAngle( Degree(3) );
    pEmit1->setTimeToLive( 0.2 );
    pEmit1->setEmissionRate( 70 );

    pEmit1->setParticleVelocity( 50 );

    pEmit1->setDirection(-reVector3Df::UNIT_Z);
    pEmit1->setColour(ColourValue::White, ColourValue::Red);        

    // Thruster 2
    pEmit2->setAngle( Degree(3) );
    pEmit2->setTimeToLive( 0.2 );
    pEmit2->setEmissionRate( 70 );

    pEmit2->setParticleVelocity( 50 );

    pEmit2->setDirection( -reVector3Df::UNIT_Z );
    pEmit2->setColour( ColourValue::White, ColourValue::Red );

    // Set the position of the thrusters
    pEmit1->setPosition( reVector3Df( 5.7f, 0.0f, 0.0f ) );
    pEmit2->setPosition( reVector3Df( -18.0f, 0.0f, 0.0f ) );

    m_PlayerNode->createChildSceneNode(reVector3Df(0.0f, 6.5f, -67.0f))->attachObject(pThrusters);
}

CSpaceShipPlayer::~CSpaceShipPlayer()
{
	CGameManager::Instance()->GetSceneManager()->destroySceneNode(Name);
}

void CSpaceShipPlayer::Update(f32 elapsedTime, OIS::Keyboard* Input, OIS::Mouse* Mouse)
{
#define DEFAULT_FORWARD m_PlayerNode->translate(m_PlayerNode->getOrientation() * reVector3Df(0, 0, 100 * elapsedTime));
#define DEFAULT_DOWN	m_PlayerNode->translate(m_PlayerNode->getOrientation() * reVector3Df(0, -100 * elapsedTime, 0));
#define DEFAULT_UP		m_PlayerNode->translate(m_PlayerNode->getOrientation() * reVector3Df(0, 100 * elapsedTime, 0));
#define DEFAULT_BACK	m_PlayerNode->translate(m_PlayerNode->getOrientation() * reVector3Df(0, 0, -50 * elapsedTime));

	static reVector3Df Trajectory(0.0f);
	bool isNewTrajectory = false;

	// Default controls
	if(m_Camera->GetMode() == ECM_SIDE)
	{
		if(Input->isKeyDown(OIS::KC_W)) 
		{
			DEFAULT_UP
		}
		if(Input->isKeyDown(OIS::KC_S)) 
		{
			DEFAULT_DOWN
		}
		if(Input->isKeyDown(OIS::KC_A)) 
		{
			DEFAULT_BACK
		}
		if(Input->isKeyDown(OIS::KC_D))
		{
			DEFAULT_FORWARD
		}
	}
	else
	{
		if(Input->isKeyDown(OIS::KC_W)) 
		{
			isNewTrajectory = true;
			Trajectory = m_PlayerNode->getOrientation() * reVector3Df(0, 0, 100 * elapsedTime);
			m_PlayerNode->translate(Trajectory);
		}
		if(Input->isKeyDown(OIS::KC_S)) 
		{
			isNewTrajectory = true;
			Trajectory = m_PlayerNode->getOrientation() * reVector3Df(0, 0, -50 * elapsedTime);
			m_PlayerNode->translate(Trajectory);
		}
		if(Input->isKeyDown(OIS::KC_A)) 
		{
			m_PlayerNode->yaw(Radian(2 * elapsedTime));
		}
		if(Input->isKeyDown(OIS::KC_D))
		{
			m_PlayerNode->yaw(Radian(-2 * elapsedTime));
		}
		if(isNewTrajectory == false)
			m_PlayerNode->translate(Trajectory);
	}
#undef DEFAULT_FORWARD 
#undef DEFAULT_DOWN	
#undef DEFAULT_UP		
#undef DEFAULT_BACK	
	
}

void CSpaceShipPlayer::SetupCollision(reVector3Df& Size)
{

}

void CSpaceShipPlayer::OnHit(CWeapon* Weapon, const reVector3Df* Direction, void* physics)
{
}