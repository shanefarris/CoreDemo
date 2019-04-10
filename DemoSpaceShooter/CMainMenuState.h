#ifndef __CMAINMENUSTATE_H__
#define __CMAINMENUSTATE_H__

#include "CMenuState.h"
#include "CGuiManager.h"
#include "CGameManager.h"

using namespace Core;

namespace Core
{
	class CMainMenuState : public CMenuState
	{
	public:
		CMainMenuState();
		~CMainMenuState() {}

		// Methods inherited from GameState
		void Start();
		void Update(f32 DeltaTimeSecs);
		void End();

		bool keyPressed( const OIS::KeyEvent &arg );
		bool keyReleased( const OIS::KeyEvent &arg );

		bool mouseMoved(const OIS::MouseEvent &arg);
		bool mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id);
		bool mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id);

		bool FrameStarted(const FrameEvent& evt) { return true; }
		bool FrameEnded(const FrameEvent& evt) { return true; }

	protected:
		//bool HandleEnter(const CEGUI::EventArgs& e);
		//CEGUI::MouseButton CMainMenuState::ConvertOISMouseButtonToCegui(int buttonID);

	};
}

#endif // __CMAINMENUSTATE_H__