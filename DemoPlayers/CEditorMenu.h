#ifndef __CEDITOR_MENU_H__
#define __CEDITOR_MENU_H__

#include "CoreEngine.h"

namespace Core
{
namespace GUI
{

class CEditorMenu : public Core::GUI::CBaseLayout_MyGui
{
public:
	CEditorMenu(const std::string& _layout);
	~CEditorMenu();

	void SetVisible(bool Visible);

private:
	void btnPlayer_Click(MyGUI::WidgetPtr _sender);
	void btnCharState_Click(MyGUI::WidgetPtr _sender);
	void btnAddEnemy_Click(MyGUI::WidgetPtr _sender);
	void btnPhysAddBox_Click(MyGUI::WidgetPtr _sender);
	void btnPhysAddSphere_Click(MyGUI::WidgetPtr _sender);
	void btnObsAddBox_Click(MyGUI::WidgetPtr _sender);
	void btnObsAddSphere_Click(MyGUI::WidgetPtr _sender);
	void btnPlaceObsBox_Click(MyGUI::WidgetPtr _sender);
	void btnPlaceObsSphere_Click(MyGUI::WidgetPtr _sender);
	void btnAddPathPoint_Click(MyGUI::WidgetPtr _sender);
	void btnAddWall_Click(MyGUI::WidgetPtr _sender);

	CPlayer* curr_player;

	MyGUI::ButtonPtr	btnPlayer;
	MyGUI::ButtonPtr	btnCharState;
	MyGUI::ButtonPtr	btnAddEnemy;
	MyGUI::ButtonPtr	btnPhysAddBox;
	MyGUI::ButtonPtr	btnPhysAddSphere;
	MyGUI::ButtonPtr	btnObsAddBox;
	MyGUI::ButtonPtr	btnObsAddSphere;
	MyGUI::ButtonPtr	btnPlaceObsBox;
	MyGUI::ButtonPtr	btnPlaceObsSphere;
	MyGUI::ButtonPtr	btnAddPathPoint;
	MyGUI::ButtonPtr	btnAddWall;
	MyGUI::ComboBoxPtr	cmbPlayers;
	MyGUI::ComboBoxPtr	cmbCharState;
	MyGUI::Widget*		frm;

};
}
}

#endif // __CEDITOR_MENU_H__