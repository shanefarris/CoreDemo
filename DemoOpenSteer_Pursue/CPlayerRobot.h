#ifndef __CPLAYERZOMBIE_H__
#define __CPLAYERZOMBIE_H__

#include "Defines.h"
#include "CPlayer.h"

#include <OIS/OIS.h>

using namespace Core;

namespace OgreOde
{
	class RayGeometry;
}
namespace OgreOde_Prefab
{
	class Ragdoll;
}


class CPlayerRobot : public CPlayer
{
public:
	CPlayerRobot(const char* Name, Physics::IPhysicsStrategy* PhysicsStrategy);
	~CPlayerRobot();
	    
	void OnHit(CWeapon* Weapon, const reVector3Df* Direction, void* physics = NULL);
	void update(const float currentTime, const float elapsedTime);
	void Update(f32 elapsedTime, OIS::Keyboard* Keyboard, OIS::Mouse* Mouse = 0);


private:
	Entity* m_Entity;

	void SetupPhysics(SceneManager* sm);
	void CreateRagDoll();

};

#endif // __CPLAYERZOMBIE_H__
