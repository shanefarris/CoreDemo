#ifndef __CLEVELANIMATED_H__
#define __CLEVELANIMATED_H__

#include "CGameState.h"
#include "CFpsFrameListener.h"

namespace Core
{
	class CLevelGeneric: public CGameState
	{
	public:
		CLevelGeneric();
		~CLevelGeneric();

		void Update(const f32& elapsedTime);
		void End();
		bool Load();

	};
}

#endif // __CLEVELANIMATED_H__
