#ifdef COMPILE_ODE

#include "CFpsPlayer.h"
#include "Prefab/OgreOde_Prefab.h"
#include "CPhysicsStrategy_Ode.h"

Entity* mEntity = 0;
SceneNode* modelNode = 0;
OgreOde::Body* dollFeetBody = 0;
OgreOde::Body* dollTorsoBody = 0;

//OgreOde::Body* _torso = 0;
//OgreOde::Body* _feet = 0;
int _ninja_rotate = 0;
int _ninja_thrust = 0;

CFpsPlayer::CFpsPlayer(const char* Name, Physics::IPhysicsStrategy* PhysicsStrategy) 
	: CPlayer(Name, PhysicsStrategy)
{
	CGameManager::Instance()->AddResourceArchive(".\\Soldier_files.zip", "soldier");
	SceneManager* m_SceneManager = CGameManager::Instance()->GetSceneManager();

	// Setup basic node structure to handle 3rd person cameras
	m_PlayerNode = m_SceneManager->getRootSceneNode()->createChildSceneNode(Name);
	m_SightNode = m_PlayerNode->createChildSceneNode (String(Name) + "_sight", reVector3Df(0, 0, 100));
	m_CameraNode = m_PlayerNode->createChildSceneNode (String(Name) + "_camera", reVector3Df(0, 50, -100));

	// Create the entity
	mEntity = m_SceneManager->createEntity("soldier", "soldier.mesh");

	// Create the scene node
	m_PlayerNode->setPosition(reVector3Df(0.0f, 0.0f, 25.0f));
	m_PlayerNode->setScale(0.5f, 0.5f, 0.5f);
	//m_PlayerNode->attachObject(mEntity);

	modelNode = m_PlayerNode->createChildSceneNode("model");
	modelNode->attachObject(mEntity);

	// Setup physics
	SetupPhysics(m_SceneManager);
}

CFpsPlayer::~CFpsPlayer()
{
	CGameManager::Instance()->GetSceneManager()->destroySceneNode(Name);
}

void CFpsPlayer::Update(f32 elapsedTime, OIS::Keyboard* Keyboard, OIS::Mouse* Mouse)
{
   //_ninja_rotate = 0;
   //if (Keyboard->isKeyDown(OIS::KC_J))
   // _ninja_rotate += -1;
   //if (Keyboard->isKeyDown(OIS::KC_L))
   // _ninja_rotate += 1;

   //_ninja_thrust = 0;
   //if (Keyboard->isKeyDown(OIS::KC_I))
   // _ninja_thrust += -1;
   //if (Keyboard->isKeyDown(OIS::KC_K))
   // _ninja_thrust += 1; 

   //if (_ninja_rotate == 0)
   //{
   // _feet->wake();
   // _feet->setAngularVelocity(Vector3(_feet->getAngularVelocity().x,0,0));
   //}
   //else
   //{
   // _feet->wake();
   // Quaternion q1 = _torso->getOrientation();
   // Quaternion q2(Degree(-4*_ninja_rotate),Ogre::Vector3::UNIT_Y);
   // _torso->setOrientation(q1*q2);
   //}

   //if (_ninja_thrust == 0)
   //{
   // _feet->wake();
   // _feet->setLinearVelocity(Vector3(0,_feet->getLinearVelocity().y,0));
   // _feet->setAngularVelocity(Vector3(0,_feet->getAngularVelocity().y,0));
   //}
   //else
   //{
   // float speed = 30;
   // _feet->wake();
   // Quaternion q = _torso->getOrientation();
   // _feet->setAngularVelocity(q*Ogre::Vector3(speed*_ninja_thrust*cos(1.0),_feet->getAngularVelocity().y,0));
   //}

   //Quaternion q = _torso->getOrientation();         
   //Vector3 x = q.xAxis();
   //Vector3 y = q.yAxis();
   //Vector3 z = q.zAxis();
   //_torso->wake();
   //_torso->setOrientation(Quaternion(x,Vector3::UNIT_Y,z));
   //_torso->setAngularVelocity(Vector3(0,0,0)); 

	//m_Direction = m_Camera->GetCamera()->getOrientation() * reVector3Df::NEGATIVE_UNIT_Z;
	//m_Direction.normalise();
	//m_Movement = reVector3Df::ZERO;

	//if(Keyboard->isKeyDown(OIS::KC_W)) 
	//{
	//	Quaternion q = dollTorsoBody->getOrientation();
	//	dollFeetBody->wake();
	//	dollFeetBody->setAngularVelocity(q * reVector3Df(10 * cos(1.0f), 0, 10 * sin(1.0f)));
	//}
	//else if(Keyboard->isKeyDown(OIS::KC_A))
	//{
	//	Quaternion q1 = dollTorsoBody->getOrientation();
	//	Quaternion q2(Degree(-4), reVector3Df::UNIT_Y);
	//	dollTorsoBody->setOrientation(q1 * q2);
	//}
	//else
	//{
	//	dollFeetBody->setAngularVelocity(reVector3Df(0, 0, 0));
	//	dollFeetBody->setLinearVelocity(Vector3(0, dollFeetBody->getLinearVelocity().y, 0));			
	//}
	//
	//Quaternion q = dollTorsoBody->getOrientation();
	//Vector3 x = q.xAxis();
	//Vector3 y = q.yAxis();
	//Vector3 z = q.zAxis();

	//dollTorsoBody->wake();
	//dollTorsoBody->setOrientation(Quaternion(x, reVector3Df::UNIT_Y, z));


	// ********************
	_ninja_rotate = 0;
   if (Keyboard->isKeyDown(OIS::KC_J))
    _ninja_rotate += -1;
   if (Keyboard->isKeyDown(OIS::KC_L))
    _ninja_rotate += 1;

   _ninja_thrust = 0;
   if (Keyboard->isKeyDown(OIS::KC_I))
    _ninja_thrust += -1;
   if (Keyboard->isKeyDown(OIS::KC_K))
    _ninja_thrust += 1; 

   if (_ninja_rotate == 0)
   {
    dollFeetBody->wake();
    dollFeetBody->setAngularVelocity(Vector3(dollFeetBody->getAngularVelocity().x,0,0));
   }
   else
   {
    dollFeetBody->wake();
    Quaternion q1 = dollTorsoBody->getOrientation();
    Quaternion q2(Degree(-4*_ninja_rotate),Ogre::Vector3::UNIT_Y);
    dollTorsoBody->setOrientation(q1*q2);
   }

   if (_ninja_thrust == 0)
   {
    dollFeetBody->wake();
    dollFeetBody->setLinearVelocity(Vector3(0,dollFeetBody->getLinearVelocity().y,0));
    dollFeetBody->setAngularVelocity(Vector3(0,dollFeetBody->getAngularVelocity().y,0));
   }
   else
   {
    float speed = 30;
    dollFeetBody->wake();
    Quaternion q = dollTorsoBody->getOrientation();
    dollFeetBody->setAngularVelocity(q*Ogre::Vector3(speed*_ninja_thrust*cos(1.0),dollFeetBody->getAngularVelocity().y,0));
   }

   Quaternion q = dollTorsoBody->getOrientation();         
   Vector3 x = q.xAxis();
   Vector3 y = q.yAxis();
   Vector3 z = q.zAxis();
   dollTorsoBody->wake();
   dollTorsoBody->setOrientation(Quaternion(x,Vector3::UNIT_Y,z));
   dollTorsoBody->setAngularVelocity(Vector3(0,0,0)); 
	
}

void CFpsPlayer::SetupPhysics(SceneManager* sm)
{
	Core::Physics::CPhysicsStrategy_Ode* strategy = 
		static_cast<Core::Physics::CPhysicsStrategy_Ode*>(m_PhysicsStrategy);
	OgreOde::World* world = strategy->GetWorld();

	// Getting AABB
	AxisAlignedBox aab = mEntity->getBoundingBox(); 
	reVector3Df min = aab.getMinimum() * m_PlayerNode->getScale();
	reVector3Df max = aab.getMaximum() * m_PlayerNode->getScale();
	reVector3Df center = aab.getCenter() * m_PlayerNode->getScale();
	reVector3Df size(fabs(max.x - min.x), fabs(max.y - min.y), fabs(max.z - min.z));
	float radius = (size.x > size.z) ? size.z / 2.0f : size.x / 2.0f;

	// Create a new space
	OgreOde::SimpleSpace* dollSpace = new OgreOde::SimpleSpace(world, world->getDefaultSpace());
	dollSpace->setInternalCollisions(false);

	// create sphere
	dollFeetBody = new OgreOde::Body(world, "feet");
	dollFeetBody->setMass(OgreOde::SphereMass(70 * 2.5, radius)); 
	OgreOde::SphereGeometry* feetGeom = new OgreOde::SphereGeometry(radius, world, 0);
	OgreOde::TransformGeometry* feetTrans = new OgreOde::TransformGeometry(world, dollSpace); 
	modelNode->translate(reVector3Df(0, -radius / m_PlayerNode->getScale().y, 0));
	feetTrans->setBody(dollFeetBody); 
	feetTrans->setEncapsulatedGeometry(feetGeom);
	m_PlayerNode->attachObject(dollFeetBody);

	// Create the Capsule (torso)
	dollTorsoBody = new OgreOde::Body(world, "torso"); 
	dollTorsoBody->setMass(OgreOde::CapsuleMass(70 * 2.5, radius, reVector3Df::UNIT_Y, radius)); 
	dollTorsoBody->setAffectedByGravity(false);
	//dollTorsoBody->setDamping(0, 50000);
	OgreOde::TransformGeometry* torsoTrans = new OgreOde::TransformGeometry(world, dollSpace); 
	OgreOde::CapsuleGeometry* torsoGeom = new OgreOde::CapsuleGeometry(
		radius, size.y - 4 * radius, world, dollSpace); 
	torsoGeom->setPosition(reVector3Df(0, size.y - ((size.y - 4 * radius) / 2 + 2 * radius), 0)); //can't find a good way to explain this
	torsoGeom->setOrientation(Quaternion(Degree(90), reVector3Df::UNIT_X));
	torsoTrans->setBody(dollTorsoBody); 
	torsoTrans->setEncapsulatedGeometry(torsoGeom);
	m_PlayerNode->attachObject(dollTorsoBody);

	// Create the Joint
	OgreOde::HingeJoint* joint = new OgreOde::HingeJoint(world);
	joint->attach(dollTorsoBody, dollFeetBody);
	joint->setAxis(reVector3Df::UNIT_X);	//set the rotation axis
}

#endif