#ifndef __CLEVELGENERIC_H__
#define __CLEVELGENERIC_H__

#include "CGameState.h"
#include "CFpsFrameListener.h"
#include "IPhysicsStrategy.h"

namespace Core
{
	namespace SceneLoader
	{
		class CSceneImporter;
	}

	class CLevelGeneric: public CGameState
	{
	private:
		CPlayer*						m_Player;
		RaySceneQuery*					m_RaySceneQuery;
		CFpsFrameListener*				m_FrameListenerFramework;
		Physics::IPhysicsStrategy*		m_PhysicsStrategy;
		SceneLoader::CSceneImporter*	m_Importer;

		void SetupPhysics();

	public:
		CLevelGeneric();
		~CLevelGeneric();

		void Update(const f32& deltaTime);
		bool Load();
		void End();
	};
}

#endif // __CLEVELGENERIC_H__
