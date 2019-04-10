#ifdef COMPILE_ODE

#ifndef __CSPACESHIPPLAYER__
#define __CSPACESHIPPLAYER__

#include "Defines.h"
#include "CPlayer.h"

#include <OIS/OIS.h>

using namespace Core;

class CFpsPlayer : public CPlayer
{
public:
	CFpsPlayer(String Name, Physics::IPhysicsStrategy* PhysicsStrategy);
	~CFpsPlayer();
	    
	void OnHit(CWeapon* Weapon, const reVector3Df* Direction, void* physics = NULL) {};
	void Update(f32 elapsedTime, OIS::Keyboard* Input, OIS::Mouse* Mouse);

private:
	void SetupPhysics(SceneManager* sm);

};

#endif // __CSPACESHIPPLAYER__

#endif