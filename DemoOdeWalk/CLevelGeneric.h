#ifdef COMPILE_ODE

#ifndef __CLEVELANIMATED_H__
#define __CLEVELANIMATED_H__

#include "CGameState.h"
#include "CFpsPlayer.h"
#include "CFpsFrameListener.h"

namespace Core
{
	class CLevelGeneric: public CGameState
	{
	private:
		CPlayer* m_Player;
		CFpsFrameListener* m_FrameListenerFramework;

	public:
		CLevelGeneric();
		~CLevelGeneric();

		void Update(const f32& elapsedTime);
		void End();
		bool Load();

	};
}

#endif // __CLEVELANIMATED_H__

#endif