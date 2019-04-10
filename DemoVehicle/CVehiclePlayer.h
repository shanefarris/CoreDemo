#ifdef COMPILE_ODE

#ifndef __CSPACESHIPPLAYER__
#define __CSPACESHIPPLAYER__

#include "Defines.h"
#include "CPlayer.h"

#include <OIS/OIS.h>

using namespace Core;

namespace Core
{
	class CWeapon;
}

class CVehiclePlayer : public CPlayer
{
public:
	CVehiclePlayer(String Name, Physics::IPhysicsStrategy* PhysicsStrategy);
	~CVehiclePlayer();
	   
	void OnHit(CWeapon* Weapon, const reVector3Df* Direction, void* physics = NULL);
	void OnDamage(u32 damage);
	void Update(f32 elapsedTime, OIS::Keyboard* Keyboard, OIS::Mouse* Mouse = 0);

private:
	bool SetupCollision();
	bool AddWheel(bool isRight, bool isSteer, const reVector3Df& Position);

};

#endif // __CSPACESHIPPLAYER__

#endif