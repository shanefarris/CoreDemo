#ifndef __METHODS_H__
#define __METHODS_H__

#include "CoreEngine.h"

using namespace Core;
using namespace Core::AI;
using namespace Core::GUI;
using namespace Core::Physics;
using namespace Core::Effects;

class EditorHelper
{
public:
	static u32 counter;

	static void SetupPhysics(IPhysicsStrategy*& physics)
	{
		// setup physics
		physics = CPhysicsManager::Instance()->CreateStrategy("physics");
		physics->InitWorld();
	}

	static void CreateFloorPlane(IPhysicsStrategy* physics)
	{
		SceneManager* sm = CGameManager::Instance()->GetSceneManager();

		// Create floor plane
		if(physics)
			physics->AddInfinitePlane(NULL);

		// Create floor plane (Ogre)
		MovablePlane *plane = new MovablePlane("Plane");
		plane->d = 0;
		plane->normal = Vector3::UNIT_Y;
		Ogre::MeshManager::getSingleton().createPlane("PlaneMesh", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, *plane, 500, 500, 1, 1, true, 1, 3, 3, Vector3::UNIT_Z);
		Entity *planeEnt = sm->createEntity("PlaneEntity", "PlaneMesh");
		planeEnt->setMaterialName("Demo/Floor");

		SceneNode* mPlaneNode = sm->getRootSceneNode()->createChildSceneNode("PlaneNode");
		mPlaneNode->attachObject(planeEnt);
	}

	static void CreateWeapon(IPhysicsStrategy* physics)
	{
		CWeapon* weapon = new CWeapon(physics);
		weapon->Name = "some_weapon";
		weapon->MeshName = "m4a1_1.mesh";
		weapon->OverlayName = "Soldier/Target";
		weapon->MeshScale = 0.1f;
		weapon->Load();

		CProjectile* projectile = new CProjectile();
		projectile->Damage = 90;

		CMagazine* mag = new CMagazine();
		mag->Capacity = 15;
		mag->Desc = "15 round clip";
		mag->Name = "5.56mm";
		mag->ProjectileName = "5.56mm";
		mag->SetProjectile(projectile);

		weapon->SetMagazine(mag);

		CItemsManager::Instance()->AddWeapon(weapon);
		CItemsManager::Instance()->PlaceWeapon("some_weapon", reVector3Df(0, 20, 0));
	}

	static ManualObject* Draw3D_Line(const reVector3Df& from, const reVector3Df& to, const ColourValue& Color, String& matName)
	{
		CGameManager* gm = CGameManager::Instance();
		SceneManager* sm = gm->GetSceneManager();

		ManualObject* myManualObject =  sm->createManualObject("obj_" + StringConverter::toString(counter)); 
		SceneNode* myManualObjectNode = sm->getRootSceneNode()->createChildSceneNode("node_" + StringConverter::toString(counter)); 
		 
		if(matName == "")
			matName = "mat" + StringConverter::toString(counter);

		MaterialPtr mat = MaterialManager::getSingleton().create(matName, ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME); 
		mat->setReceiveShadows(false); 
		mat->getTechnique(0)->setLightingEnabled(true); 
		mat->getTechnique(0)->getPass(0)->setDiffuse(Color); 
		mat->getTechnique(0)->getPass(0)->setAmbient(Color); 
		mat->getTechnique(0)->getPass(0)->setSelfIllumination(Color); 
		//mat->dispose();  // dispose pointer, not the material
		 
		myManualObject->begin(mat->getName(), Ogre::RenderOperation::OT_LINE_LIST); 
		myManualObject->position(from); 
		myManualObject->position(to); 
		// etc 
		myManualObject->end(); 
		 
		myManualObjectNode->attachObject(myManualObject);
		counter++;
		return myManualObject;
	}

	static IGameObject* AddObstacleBox(const reVector3Df& Position)
	{
		IGameObject* obj = CGameObjectManager::Instance()->CreateObject("CRATE_1");

		if(obj)
		{
			obj->Node->setPosition(Position);
			CGameObjectManager::Instance()->AddBoxObstacle(obj);
		}
		return obj;
	}

	static IGameObject* AddObstacleSphere(const reVector3Df& Position)
	{
		IGameObject* obj = CGameObjectManager::Instance()->CreateObject("CRATE_1");

		if(obj)
		{
			obj->Node->setPosition(Position);
			CGameObjectManager::Instance()->AddSphereObstacle(obj);
		}
		return obj;
	}

	static void AddPathSphere(const reVector3Df& Position)
	{
		IGameObject* obj = CGameObjectManager::Instance()->CreateObject("SPHERE_1");
		if(obj)
			obj->Node->setPosition(Position);
	}

};

#endif // __METHODS_H__