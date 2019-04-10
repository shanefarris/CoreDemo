#ifdef COMPILE_ODE

#ifndef __CFPSSOLIDERPLAYER__
#define __CFPSSOLIDERPLAYER__

#include "Defines.h"
#include "CPlayer.h"

#include <OIS/OIS.h>

using namespace Core;

namespace OgreOde
{
	class RayGeometry;
}

class CFpsSoliderPlayer : public CPlayer
{
public:
	CFpsSoliderPlayer(String Name, Physics::IPhysicsStrategy* PhysicsStrategy);
	~CFpsSoliderPlayer();
	    
	void OnHit(CWeapon* Weapon, const reVector3Df* Direction, void* physics = NULL);
	void Update(f32 elapsedTime, OIS::Keyboard* Keyboard, OIS::Mouse* Mouse = 0);

private:
	Overlay*	m_Sight;
	Entity*		m_Gun;
	SceneNode*	m_GunNode;
	f32			m_AnimationSpeed;
	f32			m_ShotTime;

	OgreOde::RayGeometry*	m_GunRay;
	RaySceneQuery*			m_RayQuery;
	SceneManager*			m_SceneManager;

	void SetupPhysics();

};

#endif // __CFPSSOLIDERPLAYER__

#endif