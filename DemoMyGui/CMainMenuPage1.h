#ifndef __CMAINMENUPAGE1_H__
#define __CMAINMENUPAGE1_H__

#include "MyGui/MyGUI.h"
#include "CBaseLayout_MyGui.h"

namespace Core
{
namespace GUI
{

class CMainMenuPage1 : public Core::GUI::CBaseLayout_MyGui
{
public:
	CMainMenuPage1(const std::string& _layout);
	~CMainMenuPage1();

	MyGUI::WidgetPtr getClient() { return mMainWidget->getClientWidget(); }

	void setVisible(bool _visible);

	MyGUI::delegates::CDelegate2<MyGUI::WidgetPtr, bool>  eventButtonPress;

private:
	void notifyMouseButtonClick(MyGUI::WidgetPtr _sender);
	void notifyFrameEvent(float _time);
	void notifyPostAction(MyGUI::WidgetPtr _sender);
	void EndProgram(MyGUI::WidgetPtr _sender);
	void StartProgram(MyGUI::WidgetPtr _sender);

	MyGUI::ControllerPosition* createControllerPosition(const MyGUI::IntPoint & _point);
	MyGUI::ControllerFadeAlpha* createControllerFadeAlpha(float _alpha, float _coef, bool _enable);

	void FrameAdvise(bool _advise);

private:
	bool mFrameAdvise;
	MyGUI::ButtonPtr mButton1;
	MyGUI::ButtonPtr mButton2;
	MyGUI::ButtonPtr mButton3;
	float mCountTime;

};
}
}

#endif // __CMAINMENUPAGE1_H__
