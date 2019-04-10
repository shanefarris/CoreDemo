#ifdef COMPILE_ODE

#include "CVehiclePlayer.h"
#include "CEuler.h"
#include "Prefab/OgreOde_Prefab.h"
#include "CPhysicsStrategy_Ode.h"

Entity* PlayerEntity = 0;
Entity* entTurret = 0;
Entity* entBarrels = 0;

SceneNode* TurretNode = 0;
SceneNode* BarrelsNode = 0;
//SceneNode* CollisionNode = 0;

OgreOde_Prefab::Vehicle* vehicle = 0;

CEuler Euler(Degree(0));

CVehiclePlayer::CVehiclePlayer(String Name, Physics::IPhysicsStrategy* PhysicsStrategy) 
	: CPlayer(Name, PhysicsStrategy)
{
	SceneManager* m_SceneManager = CGameManager::Instance()->GetSceneManager();

	//std::string file = "tank.ogreode";
	//vehicle = (OgreOde_Prefab::Vehicle*)m_PhysicsStrategy->LoadPrefabVehicle(file, "Tank");
	//vehicle->setPosition(reVector3Df(0, 50, 0));

	Core::Physics::CPhysicsStrategy_Ode* strategy = 
		static_cast<Core::Physics::CPhysicsStrategy_Ode*>(m_PhysicsStrategy);
	OgreOde::World* world = strategy->GetWorld();

	PlayerEntity = m_SceneManager->createEntity("Tank", "NewTank_MainBody.mesh");
	vehicle = new OgreOde_Prefab::Vehicle(Name, world, 0);
	vehicle->setEntity(PlayerEntity);
	vehicle->getEntity()->setCastShadows(true);
	vehicle->getTransNode()->attachObject(vehicle->getEntity());

	// Setup basic node structure to handle 3rd person cameras
	m_PlayerNode = vehicle->getTransNode(); //m_SceneManager->getRootSceneNode()->createChildSceneNode(Name);
	m_SightNode = m_PlayerNode->createChildSceneNode (Name + "_sight", reVector3Df(0, 0, 0));
	m_CameraNode = m_PlayerNode->createChildSceneNode (Name + "_camera", reVector3Df(0, 0, 0));

	// Main body
	//PlayerEntity = m_SceneManager->createEntity("Tank", "NewTank_MainBody.mesh");
	//m_PlayerNode->attachObject(PlayerEntity);
	//m_PlayerNode->setPosition(reVector3Df(0, 50, 0));

	// Create turrent
	entTurret = m_SceneManager->createEntity("TankTurrent", "NewTank_Turret.mesh");
	//TurretNode = m_PlayerNode->createChildSceneNode(reVector3Df(0, 1.875414, 0));
	TurretNode = m_SceneManager->getRootSceneNode()->createChildSceneNode(reVector3Df(0, 2.0, 0));
	TurretNode->attachObject(entTurret);
	TurretNode->setOrientation(Euler);

	// Create the barrels
	//entBarrels = m_SceneManager->createEntity("TankBarrels", "NewTank_Barrels.mesh");
	//BarrelsNode = TurretNode->createChildSceneNode(reVector3Df(-1.0896e-007, 0.0, 0.491442));
	//BarrelsNode->attachObject(entBarrels);
	//BarrelsNode->setOrientation(Euler);

	// Physics
	SetupCollision();
	vehicle->setPosition(reVector3Df(0, 10, 0));
}

CVehiclePlayer::~CVehiclePlayer()
{
	CGameManager::Instance()->GetSceneManager()->destroySceneNode(Name);
}

void CVehiclePlayer::Update(f32 elapsedTime, OIS::Keyboard* Keyboard, OIS::Mouse* Mouse)
{
	vehicle->setInputs(Keyboard->isKeyDown(OIS::KC_A),
                       Keyboard->isKeyDown(OIS::KC_D),
                       Keyboard->isKeyDown(OIS::KC_W),
                       Keyboard->isKeyDown(OIS::KC_S));
	vehicle->update(elapsedTime);

	const OIS::MouseState &ms = Mouse->getMouseState();
	Radian m_RotX = Degree(-ms.X.rel * 0.13);
	Radian m_RotY = Degree(-ms.Y.rel * 0.13);
	Euler.Yaw(Radian(ms.X.rel * 0.001));

	TurretNode->setOrientation(Euler);

	Matrix3 matrix;
	m_PlayerNode->getOrientation().ToRotationMatrix(matrix);
	reVector3Df Direction = matrix.GetColumn(2);

	m_Camera->GetCamera()->yaw(m_RotX);
	m_Camera->GetCamera()->pitch(m_RotY);
	m_Camera->Update(elapsedTime, 
					m_Camera->GetCamera()->getPosition(),
					(Direction * -20) + vehicle->getPosition()  + reVector3Df(0, 10, 0));
}

bool CVehiclePlayer::SetupCollision()
{
	Core::Physics::CPhysicsStrategy_Ode* strategy = 
		static_cast<Core::Physics::CPhysicsStrategy_Ode*>(m_PhysicsStrategy);
	OgreOde::World* world = strategy->GetWorld();

	{
		OgreOde::EntityInformer ei(PlayerEntity, m_PlayerNode->_getFullTransform());
		const Vector3 box (ei.getSize());
		const Vector3 boxCenter (ei.getCenter());
		OgreOde::Geometry* vGeom = new OgreOde::BoxGeometry(box, world, vehicle->getSpace());
		vGeom->setPosition(boxCenter);
		vehicle->setGeometry(vGeom);					
		vGeom->setUserAny(Ogre::Any(vehicle));	
	}

	vehicle->setTransformGeometry(new OgreOde::TransformGeometry(world, 0));
	vehicle->getTransformGeometry()->setEncapsulatedGeometry(vehicle->getGeometry());
	vehicle->getTransformGeometry()->setBody(vehicle->getBody());

	// set mass
	{
		OgreOde::EntityInformer ei(vehicle->getEntity());
		reVector3Df box = ei.getSize();
		OgreOde::BoxMass box_mass(20.0f, Vector3(box.x, box.y, box.z));
		vehicle->getBody()->setMass(box_mass);
		reVector3Df offset(reVector3Df::ZERO);
		offset.x = 0; 
		offset.y = -1; 
		offset.z = 0; 
		vehicle->setOffset(-offset);
	}

	// Add wheels
	AddWheel(false, true, Vector3(0.899088, -0.626518, 0.771819));
	AddWheel(false, false, Vector3(0.899088, -0.626518, -0.0261833));
	AddWheel(false, false, Vector3(0.899088, -0.626518, -0.787603));
	AddWheel(true, true, Vector3(-0.899088, -0.626518, 0.771819));
	AddWheel(true, false, Vector3(-0.899088, -0.626518, -0.0261833));
	AddWheel(true, false, Vector3(-0.899088, -0.626518, -0.787603));

	vehicle->setSuspension(100, 3.6, 0.01);

	// Add engine
	vehicle->getEngine()->setRevLimit(30.0);
	vehicle->getEngine()->setBrakeForce(15.0);
	vehicle->getEngine()->setTorque(0.5, 10.0);

	vehicle->getSpace()->setInternalCollisions(false);

	// Add collision to turret
	OgreOde::Body* body = new OgreOde::Body(world);
	TurretNode->attachObject(body);

	OgreOde::EntityInformer ei(entTurret, TurretNode->_getFullTransform());
	reVector3Df size = ei.getSize();
	OgreOde::BoxMass mass(0.1, size);
	mass.setDensity(0.1, size);

	OgreOde::Geometry* geom = new OgreOde::BoxGeometry(size, world, vehicle->getSpace()); 
	body->setMass(mass);

	// Tie the collision geometry to the physical body
    geom->setBody(body);
    entTurret->setUserAny(Any(geom));

	OgreOde::HingeJoint* joint = new OgreOde::HingeJoint(world);
	joint->attach(vehicle->getBody(), body);
	joint->setAnchor(body->getPosition());
	joint->setAxis(Vector3::UNIT_Y);

	return true;
}

bool CVehiclePlayer::AddWheel(bool isRight, bool isSteer, const reVector3Df& Position)
{
	OgreOde_Prefab::Vehicle::Wheel *wheel = 0;

	String fileName;
	if(isRight)
		fileName = "Wheel_R_1.mesh";
	else
		fileName = "Wheel_L_1.mesh";

	wheel = vehicle->addWheel(fileName, Position, 0.06f);

	if (wheel)
	{
		if(isSteer)
		{
			wheel->setSteerFactor(1);
			wheel->setSteerLimit(0.75);
			wheel->setSteerForce(8.0);
			wheel->setSteerSpeed(4.0);
		}
		wheel->setPowerFactor(1.0);
		wheel->setBrakeFactor(0.75);
		wheel->setContact(0.9, 1.5, 0.004);
	}
	else
	{
		return false;
	}

	return true;
}

void CVehiclePlayer::OnHit(CWeapon* Weapon, const reVector3Df* Direction, void* physics)
{
}

#endif