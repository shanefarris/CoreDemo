#ifndef __CLEVELGENERIC_H__
#define __CLEVELGENERIC_H__

#include "CGameState.h"
#include "CFpsFrameListener.h"
#include "CPlayer.h"

class CPlayerZombie;

namespace Core
{
	struct SceneImporterPackage;

	class CLevelGeneric: public CGameState
	{
	private:
		CPlayer*						m_Player;
		CFpsFrameListener*				m_FrameListenerFramework;

	public:
		CLevelGeneric();
		~CLevelGeneric();

		void Update(const f32& deltaTime);
		bool Load();
		void End();

		void CreateSceneImporterPackage(SceneImporterPackage& sip);
	};
}

#endif // __CLEVELGENERIC_H__
