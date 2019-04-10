#ifdef COMPILE_ODE

#include "CFpsSoliderPlayer.h"
#include "CEuler.h"
#include "Prefab/OgreOde_Prefab.h"
#include "CPhysicsStrategy_Ode.h"
#include "CTextOutput.h"

using namespace Core::Physics;

enum QueryFlags
{
	ANY_QUERY_MASK					= 1<<0,
	ZOMBIE_QUERY_MASK				= 1<<1,
	GEOMETRY_QUERY_MASK				= 1<<2,
	VEHICLE_QUERY_MASK				= 1<<3,
	STATIC_GEOMETRY_QUERY_MASK		= 1<<4
};

CFpsSoliderPlayer::CFpsSoliderPlayer(String Name, Physics::IPhysicsStrategy* PhysicsStrategy) 
	: CPlayer(Name, PhysicsStrategy)
{
	m_SceneManager = CGameManager::Instance()->GetSceneManager();

	// Setup basic node structure to handle 3rd person cameras
	m_PlayerNode = m_SceneManager->getRootSceneNode()->createChildSceneNode(Name);
	m_SightNode = m_PlayerNode->createChildSceneNode (Name + "_sight", reVector3Df(0, 0, 0));
	m_CameraNode = m_PlayerNode->createChildSceneNode (Name + "_camera", reVector3Df(0, 0, 0));

	//Entity *ent = m_SceneManager->createEntity("Soldier", "Soldier.mesh");
	//m_PlayerNode->attachObject(ent);

	//m_PlayerNode->setScale(.005, .005, .005);
	m_PlayerNode->setPosition(reVector3Df(10, 5, 10));
	//m_PlayerNode->rotate(Quaternion(Degree(-90), reVector3Df::UNIT_X));


	m_Sight = (Overlay*)OverlayManager::getSingleton().getByName("Soldier/Target");    
	m_Sight->show();

	//m_Gun = m_SceneManager->createEntity("gun", "gun.mesh");
	//m_Gun->setCastShadows(false);

	//m_GunNode = m_PlayerNode->createChildSceneNode("gun", reVector3Df(0.3, -0.15, -1.1));
	//m_GunNode->attachObject(m_Gun);

	m_AnimationSpeed = 1.0;
	//CreateRagDoll();
	m_ShotTime = 0.0;

	// Physics
	SetupPhysics();

}

CFpsSoliderPlayer::~CFpsSoliderPlayer()
{
	CGameManager::Instance()->GetSceneManager()->destroySceneNode(Name);
}

void CFpsSoliderPlayer::Update(f32 elapsedTime, OIS::Keyboard* Keyboard, OIS::Mouse* Mouse)
{
	// Update our shot time
	m_ShotTime -= elapsedTime;

	reVector3Df Direction = m_PlayerNode->getOrientation() * reVector3Df::NEGATIVE_UNIT_Z;
	Direction.normalise();
	reVector3Df Movement = reVector3Df::ZERO;

	if(Keyboard->isKeyDown(OIS::KC_W)) 
	{
		Movement += Direction;
	}
	if(Keyboard->isKeyDown(OIS::KC_S)) 
	{
		Movement -= Direction;
	}
	if(Keyboard->isKeyDown(OIS::KC_A)) 
	{
		Movement.z += Direction.x * -1;
		Movement.x += Direction.z;
	}
	if(Keyboard->isKeyDown(OIS::KC_D))
	{
		Movement.x += Direction.z * -1;
		Movement.z += Direction.x;
	}
	m_PlayerNode->translate(Movement);

	// mouse
	const OIS::MouseState &ms = Mouse->getMouseState();
	Radian m_RotX = Degree(-ms.X.rel * 0.13);
	Radian m_RotY = Degree(-ms.Y.rel * 0.13);

	CORE_TEXT("position", "position:" + StringConverter::toString(m_PlayerNode->_getDerivedPosition()));
	CORE_TEXT("direction", "direction:" + StringConverter::toString(Direction));
	if(Movement != reVector3Df::ZERO)
		CORE_TEXT("movement", "movement:" + StringConverter::toString(Movement));

	m_PlayerNode->yaw(m_RotX, Node::TransformSpace::TS_WORLD);
	m_Camera->GetCamera()->yaw(m_RotX);
	m_Camera->GetCamera()->pitch(m_RotY);
	m_Camera->Update(elapsedTime, 
					m_Camera->GetCamera()->getPosition(),
					(Direction) + m_PlayerNode->getPosition()  + reVector3Df(0, 0, 0));

	//m_GunNode->setOrientation(m_PlayerNode->getOrientation());
	//m_GunNode->setPosition(m_PlayerNode->getPosition() + 
	//					  (m_PlayerNode->getOrientation() * 
	//					  reVector3Df(0.3, -0.15, -1.1)));

	try
	{
	// Picking shots
	CPhysicsStrategy_Ode* physics = (CPhysicsStrategy_Ode*)m_PhysicsStrategy;
	if (m_ShotTime <= 0.0f /* Is the shot time reset */
		&& Mouse->getMouseState().buttonDown(OIS::MB_Left))
	{
		// Pick middle of the screen
		Ray pickRay = m_Camera->GetCamera()->getCameraToViewportRay(0.5, 0.5);
		m_GunRay->setDefinition(pickRay.getOrigin(), pickRay.getDirection());

		m_ShotTime = 0.2;

		m_RayQuery->setRay (pickRay);
		const RaySceneQueryResult& result = m_RayQuery->execute();
		if (!result.empty())
		{
			RaySceneQueryResult::const_iterator i = result.begin();
	
			m_RayQuery->setSortByDistance (true, 1);//only one hit
			m_GunRay->enable ();
			while((i != result.end()))
			{

				//_last_ragdoll = 5.0;

				OgreOde_Prefab::Ragdoll * const ragdoll = static_cast<OgreOde_Prefab::Ragdoll *> (i->movable);
				if(ragdoll)
				{
					if (!ragdoll->isPhysical())
					{
						//if(ragdoll->getName() != "solider")
						//{
							if(ragdoll->getAnimationState("Walk")->getEnabled())
							{
								ragdoll->getAnimationState("Walk")->setEnabled(false);

								// Create the ragdoll
								ragdoll->takePhysicalControl(physics->GetWorld(), physics->GetWorld()->getDefaultSpace(), false);
								ragdoll->setSelfCollisions(true);
							}
						//}
					}

					OgreOde::Body *hit_body;
					Ogre::Vector3 hit_point;

					bool is_hit = false;
					if (ragdoll->pickRagdoll(m_GunRay, hit_body, hit_point))
					{
						if (hit_body)
						{
							hit_body->addForceAt(pickRay.getDirection() * 250000, hit_point);
							is_hit = true;
							ragdoll->_timeSincePhysical = 0.0f;
						}
					}

					if (!is_hit&& !ragdoll->isPhysical())
					{
						ragdoll->releasePhysicalControl();
						ragdoll->getAnimationState("Walk")->setEnabled(true);
					}

					++i;
				}
			}	// if results.	
		}
		m_GunRay->disable ();
    }
	}catch(...)
	{
		int i = 0;
	}
}

void CFpsSoliderPlayer::SetupPhysics()
{
	CPhysicsStrategy_Ode* physics = (CPhysicsStrategy_Ode*)m_PhysicsStrategy;

	m_GunRay = new OgreOde::RayGeometry(1000.0, physics->GetWorld(), physics->GetWorld()->getDefaultSpace());
	m_RayQuery = m_SceneManager->createRayQuery(Ray());
	m_RayQuery->setQueryMask (ZOMBIE_QUERY_MASK);
	m_RayQuery->setQueryTypeMask(SceneManager::ENTITY_TYPE_MASK);
}

void CFpsSoliderPlayer::OnHit(CWeapon* Weapon, const reVector3Df* Direction, void* physics)
{
}

#endif