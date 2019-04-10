#ifndef __CLEVELGENERIC_H__
#define __CLEVELGENERIC_H__


#include "CoreEngine.h"
#include "CEditorMenu.h"
#include "CCustomFrameListener.hpp"

using namespace Core;
using namespace Core::AI;
using namespace Core::GUI;
using namespace Core::Physics;
using namespace Core::Effects;

namespace Core
{
	class CLevelGeneric: public CGameState
	{
	public:
		CLevelGeneric() { }
		~CLevelGeneric();

		void Update(const f32& elapsedTime);
		bool Load();
		void End() { }

		CEditorMenu* page;

		static IPhysicsStrategy*	physics;
		static f32					TimeUntilNextToggle;
		static CGuiStrategy_MyGui*	Strategy;
		static CCustomFrameListener* FrameListenerFramework;
		static CPlayer*				solider;
		static CPlayer*				marine;
		static CPlayer*				zombie;
		static CPlayer*				robot;
	};
}


#endif // __CLEVELGENERIC_H__
