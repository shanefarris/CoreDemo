#ifndef __CLEVELGENERIC_H__
#define __CLEVELGENERIC_H__

#include "CGameState.h"
#include "CFpsFrameListener.h"

namespace Core
{
	class CLevelGeneric: public CGameState
	{
	private:
		CPlayer*			m_Player;
		CFpsFrameListener*  m_FrameListenerFramework;

	public:
		CLevelGeneric();
		~CLevelGeneric();

		void Update(const f32& elapsedTime);
		bool Load();
		void End();
	};
}

#endif // __CLEVELGENERIC_H__
