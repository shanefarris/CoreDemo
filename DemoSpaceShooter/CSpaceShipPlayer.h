#ifndef __CSPACESHIPPLAYER__
#define __CSPACESHIPPLAYER__

#include "Defines.h"
#include "CPlayer.h"

#include <OIS/OIS.h>

using namespace Core;

class CSpaceShipPlayer : public CPlayer
{
public:
	CSpaceShipPlayer(String Name, Physics::IPhysicsStrategy* Strategy);
	~CSpaceShipPlayer();
	    
	void OnHit(CWeapon* Weapon, const reVector3Df* Direction, void* physics = NULL);
	void Update(f32 elapsedTime, OIS::Keyboard* Input, OIS::Mouse* Mouse);

private:
	void SetupCollision(reVector3Df& Size);

};

#endif // __CSPACESHIPPLAYER__
