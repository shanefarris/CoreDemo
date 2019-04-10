#ifdef COMPILE_ODE

#ifndef __CLEVELGENERIC_H__
#define __CLEVELGENERIC_H__

#include "CGameState.h"
#include "CFpsFrameListener.h"
#include "CGenericCollisionListener_Ode.h"
#include "CGenericTerrainGeometryHeightListener_Ode.h"

namespace Core
{
	namespace Physics
	{
		class CPhysicsStrategy_Ode;
	}

	class CLevelGeneric: public CGameState
	{
	private:
		CPlayer*						m_Player;
		RaySceneQuery*					m_RaySceneQuery;
		OgreOde::TerrainGeometry*		m_Terrain;
		CFpsFrameListener*				m_FrameListenerFramework;
		Physics::CPhysicsStrategy_Ode*	m_PhysicsStrategy;

		// Physics listeners
		Physics::CGenericCollisionListener_Ode* m_CollisionListener;
		//Physics::CGenericTerrainGeometryHeightListener_Ode* m_TerrainListener;

		void SetupPhysics();

	public:
		CLevelGeneric();
		~CLevelGeneric();

		void Update(const f32& deltaTime);
		void End();
		bool Load();

	};
}

#endif // __CLEVELGENERIC_H__

#endif