#ifndef __CLEVELANIMATED_H__
#define __CLEVELANIMATED_H__

#include "CGameState.h"
#include "CFpsFrameListener.h"

namespace Core
{
	class CLevelGeneric: public CGameState
	{
	private:
		CPlayer*			m_Player;
		CFpsFrameListener* m_FrameListenerFramework;

	public:
		CLevelGeneric();
		~CLevelGeneric();

		void Update(f32 elapsedTime, OIS::Keyboard* Input, OIS::Mouse* Mouse);
		void End();
		bool Load();

	};
}

#endif // __CLEVELANIMATED_H__
