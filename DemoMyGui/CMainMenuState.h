#ifndef __CMAINMENUSTATE_H__
#define __CMAINMENUSTATE_H__

#include "CMenuState.h"
#include "CGuiManager.h"
#include "CGameManager.h"

using namespace Core;

namespace MyGUI
{
	class Widget;
}

namespace Core
{
	class CMainMenuState : public CMenuState
	{
	public:
		CMainMenuState();
		~CMainMenuState() {}

		// Methods inherited from GameState
		bool Load();
		void Update(const f32& elapsedTime);
		void End();

		bool keyPressed( const OIS::KeyEvent &arg );
		bool keyReleased( const OIS::KeyEvent &arg );
		bool mouseMoved(const OIS::MouseEvent &arg);
		bool mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id);
		bool mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id);
		bool frameRenderingQueued(const FrameEvent& evt) { return true; }
		bool frameEnded(const FrameEvent& evt) { return true; }

		// Events for the menu controls
		void NotifyMouseButtonClick(MyGUI::Widget* sender, bool isCompleted);

		bool mIsAnim;

	};
}

#endif // __CMAINMENUSTATE_H__