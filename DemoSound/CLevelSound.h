#ifndef __CLEVELANIMATED_H__
#define __CLEVELANIMATED_H__

#include "CGameState.h"
#include "CCustomFpsFrameListener.h"

namespace Core
{
	class CLevelSound: public CGameState
	{
	private:
		CFrameListenerFramework* m_FrameListenerFramework;

	public:
		CLevelSound();
		~CLevelSound();

		void Update(const f32& elapsedTime);
		void End();
		bool Load();

	};
}

#endif // __CLEVELANIMATED_H__
