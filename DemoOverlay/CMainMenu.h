#ifdef COMPILE_OVERLAYS

#ifndef __CMAINMENU_H__
#define __CMAINMENU_H__

#include "CMenuState.h"

using namespace Core;

namespace Core
{
	class CMainMenu : public CMenuState
	{
	public:
		CMainMenu();

		void Update(const f32& elapsedTime) { };
		bool Load();
		void End();

		bool keyPressed(const OIS::KeyEvent &e);
		bool keyReleased(const OIS::KeyEvent &e);

		bool mouseMoved(const OIS::MouseEvent &e);
		bool mousePressed(const OIS::MouseEvent &e, OIS::MouseButtonID id);
		bool mouseReleased(const OIS::MouseEvent &e, OIS::MouseButtonID id);

		bool frameRenderingQueued(const FrameEvent& Event);
		bool frameEnded(const FrameEvent& Event);
	};
}

#endif // __CMAINMENU_H__

#endif