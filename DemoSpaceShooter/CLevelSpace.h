#ifndef __CLEVELSPACE_H__
#define __CLEVELSPACE_H__

#include "CGameState.h"
#include "CSpaceShipPlayer.h"
#include "CSpaceShipFrameListener.h"
#include "SoundEngine.h"
#include "CPhysicsManager.h"

namespace Core
{
	class CLevelSpace: public CGameState
	{
	private:
		CPlayer* m_Player;
		CSpaceShipFrameListener* m_FrameListenerFramework;

	public:
		CLevelSpace();
		~CLevelSpace();

		void Update(f32 elapsedTime, OIS::Keyboard* Input, OIS::Mouse* Mouse);
		void End();
		bool Load();

	};
}

#endif // __CLEVELSPACE_H__
