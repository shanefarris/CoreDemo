#include "CMainMenuPage1.h"
#include "CGameManager.h"

using namespace Core;
using namespace GUI;

CMainMenuPage1::CMainMenuPage1(const std::string& _layout) :
	Core::GUI::CBaseLayout_MyGui(_layout),
	mFrameAdvise(false)
{
	AssignWidget(mButton1, "Button1");
	AssignWidget(mButton2, "Button2");
	AssignWidget(mButton3, "Button3");

	mButton1->setVisible(false);
	mButton2->setVisible(false);
	mButton3->setVisible(false);
	mMainWidget->setVisible(false);

	mButton1->eventMouseButtonClick = MyGUI::newDelegate(this, &CMainMenuPage1::StartProgram);
	mButton2->eventMouseButtonClick = MyGUI::newDelegate(this, &CMainMenuPage1::notifyMouseButtonClick);
	mButton3->eventMouseButtonClick = MyGUI::newDelegate(this, &CMainMenuPage1::EndProgram);
}

CMainMenuPage1::~CMainMenuPage1()
{
	FrameAdvise(false);
}

void CMainMenuPage1::setVisible(bool _visible)
{
	if (_visible)
	{
		mButton1->setVisible(false);
		mButton2->setVisible(false);
		mButton3->setVisible(false);

		MyGUI::LayerManager::getInstance().upLayerItem(mButton1);
		MyGUI::LayerManager::getInstance().upLayerItem(mButton2);
		MyGUI::LayerManager::getInstance().upLayerItem(mButton3);

		FrameAdvise(true);
		mCountTime = 0;
	}
	else
	{
		MyGUI::ControllerManager& manager = MyGUI::ControllerManager::getInstance();

		MyGUI::ControllerFadeAlpha * controller = createControllerFadeAlpha(0, 3, true);
		manager.addItem(mMainWidget, controller);

		controller = createControllerFadeAlpha(0, 3, true);
		manager.addItem(mButton1, controller);
		controller = createControllerFadeAlpha(0, 3, true);
		manager.addItem(mButton2, controller);
		controller = createControllerFadeAlpha(0, 3, true);
		manager.addItem(mButton3, controller);
	}
}

void CMainMenuPage1::notifyMouseButtonClick(MyGUI::WidgetPtr _sender)
{
	eventButtonPress(_sender, false);
}

void CMainMenuPage1::notifyFrameEvent(float _time)
{
	mCountTime += _time;

	const int offset = 30;
	const int offset_jamp = 1;
	const MyGUI::IntSize & view = MyGUI::Gui::getInstance().getViewSize();
	const float time_diff = 0.3;

	if (mCountTime > time_diff * 1.5)
	{
		if (!mButton1->isVisible())
		{
			mButton1->setPosition(view.width, (mButton1->getHeight() + offset) * 2 + offset);
			mButton1->setVisible(true);
			mButton1->setAlpha(1);
			MyGUI::IntPoint point(view.width - mButton1->getWidth() - offset, offset);
			MyGUI::ControllerManager::getInstance().addItem(mButton1, createControllerPosition(point));
		}
	}

	if (mCountTime > time_diff * 2)
	{
		if (!mButton2->isVisible())
		{
			mButton2->setPosition(view.width, (mButton2->getHeight() + offset) * 2 + offset);
			mButton2->setVisible(true);
			mButton2->setAlpha(1);
			MyGUI::IntPoint point(view.width - mButton2->getWidth() - offset, (mButton2->getHeight() + offset) + offset);
			MyGUI::ControllerManager::getInstance().addItem(mButton2, createControllerPosition(point));
		}
	}

	if (mCountTime > time_diff * 3)
	{
		if (!mButton3->isVisible())
		{
			mButton3->setPosition(view.width, (mButton3->getHeight() + offset) * 2 + offset);
			mButton3->setVisible(true);
			mButton3->setAlpha(1);
			MyGUI::IntPoint point(view.width - mButton3->getWidth() - offset, (mButton3->getHeight() + offset) * 2 + offset);
			MyGUI::ControllerPosition* controller = createControllerPosition(point);
			MyGUI::ControllerManager::getInstance().addItem(mButton3, controller);
			controller->eventPostAction = MyGUI::newDelegate(this, &CMainMenuPage1::notifyPostAction);
		}
		FrameAdvise(false);
	}
}

void CMainMenuPage1::notifyPostAction(MyGUI::WidgetPtr _sender)
{
	eventButtonPress(_sender, true);
}

void CMainMenuPage1::FrameAdvise(bool _advise)
{
	if (_advise)
	{
		if (!mFrameAdvise)
		{
			mFrameAdvise = true;
			MyGUI::Gui::getInstance().eventFrameStart += MyGUI::newDelegate(this, &CMainMenuPage1::notifyFrameEvent);
		}
	}
	else
	{
		if (mFrameAdvise)
		{
			mFrameAdvise = false;
			MyGUI::Gui::getInstance().eventFrameStart -= MyGUI::newDelegate(this, &CMainMenuPage1::notifyFrameEvent);
		}
	}
}

MyGUI::ControllerPosition * CMainMenuPage1::createControllerPosition(const MyGUI::IntPoint & _point)
{
	MyGUI::ControllerItem* item = MyGUI::ControllerManager::getInstance().createItem(MyGUI::ControllerPosition::getClassTypeName());
	MyGUI::ControllerPosition* controller = item->castType<MyGUI::ControllerPosition>();

	controller->setPosition(_point);
	controller->setTime(0.5);

	controller->setAction(MyGUI::newDelegate(MyGUI::action::inertionalMoveFunction));

	return controller;
}

MyGUI::ControllerFadeAlpha* CMainMenuPage1::createControllerFadeAlpha(float _alpha, float _coef, bool _enable)
{
	MyGUI::ControllerItem* item = MyGUI::ControllerManager::getInstance().createItem(MyGUI::ControllerFadeAlpha::getClassTypeName());
	MyGUI::ControllerFadeAlpha* controller = item->castType<MyGUI::ControllerFadeAlpha>();

	controller->setAlpha(_alpha);
	controller->setCoef(_coef);
	controller->setEnabled(_enable);

	return controller;
}

void CMainMenuPage1::EndProgram(MyGUI::WidgetPtr _sender)
{
	CGameManager::Instance()->SetQuit(true);
}

void CMainMenuPage1::StartProgram(MyGUI::WidgetPtr _sender)
{
	Shutdown();
	CGameManager::Instance()->SetReadyToAdvance(true);
}
