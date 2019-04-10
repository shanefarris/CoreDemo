#include "CEditorMenu.h"
#include "CLevelGeneric.h"
#include "CCustomFrameListener.hpp"

using namespace Core;
using namespace Core::AI;
using namespace Core::GUI;
using namespace Core::Physics;
using namespace Core::Effects;

CEditorMenu::CEditorMenu(const std::string& _layout) : Core::GUI::CBaseLayout_MyGui(_layout)
{
	AssignWidget(btnPlayer, "btnPlayer");
	AssignWidget(btnCharState, "btnCharState");
	AssignWidget(btnAddEnemy, "btnAddEnemy");
	AssignWidget(cmbPlayers, "cmbPlayers");
	AssignWidget(cmbCharState, "cmbCharState");
	AssignWidget(btnPhysAddBox, "btnPhysAddBox");
	AssignWidget(btnObsAddSphere, "btnObsAddSphere");
	AssignWidget(btnObsAddBox, "btnObsAddBox");
	AssignWidget(btnPhysAddSphere, "btnPhysAddSphere");
	AssignWidget(btnPlaceObsBox, "btnPlaceObsBox");
	AssignWidget(btnPlaceObsSphere, "btnPlaceObsSphere");
	AssignWidget(btnAddPathPoint, "btnAddPathPoint");
	AssignWidget(btnAddWall, "btnAddWall");
	AssignWidget(frm, "_Main");

	MyGUI::PointerManager::getInstance().setVisible(true);

	btnPlayer->eventMouseButtonClick = MyGUI::newDelegate(this, &CEditorMenu::btnPlayer_Click);
	btnCharState->eventMouseButtonClick = MyGUI::newDelegate(this, &CEditorMenu::btnCharState_Click);
	btnAddEnemy->eventMouseButtonClick = MyGUI::newDelegate(this, &CEditorMenu::btnAddEnemy_Click);
	btnPhysAddBox->eventMouseButtonClick = MyGUI::newDelegate(this, &CEditorMenu::btnPhysAddBox_Click);
	btnPhysAddSphere->eventMouseButtonClick = MyGUI::newDelegate(this, &CEditorMenu::btnPhysAddSphere_Click);
	btnObsAddBox->eventMouseButtonClick = MyGUI::newDelegate(this, &CEditorMenu::btnObsAddBox_Click);
	btnObsAddSphere->eventMouseButtonClick = MyGUI::newDelegate(this, &CEditorMenu::btnObsAddSphere_Click);
	btnPlaceObsBox->eventMouseButtonClick = MyGUI::newDelegate(this, &CEditorMenu::btnPlaceObsBox_Click);
	btnPlaceObsSphere->eventMouseButtonClick = MyGUI::newDelegate(this, &CEditorMenu::btnPlaceObsSphere_Click);
	btnAddPathPoint->eventMouseButtonClick = MyGUI::newDelegate(this, &CEditorMenu::btnAddPathPoint_Click);
	btnAddWall->eventMouseButtonClick = MyGUI::newDelegate(this, &CEditorMenu::btnAddWall_Click);

	// Create players
	CLevelGeneric::solider = CGameObjectManager::Instance()->CreatePlayer("SOLIDER1", EPT_TEAM_1, "", CLevelGeneric::physics, false);
	CLevelGeneric::solider->SetPlayerPosition(0, 0, 0);
	CLevelGeneric::solider->SetVisible(false);

	CLevelGeneric::marine = CGameObjectManager::Instance()->CreatePlayer("GENMARINE1", EPT_TEAM_2, "", CLevelGeneric::physics, false);
	CLevelGeneric::marine->SetPlayerPosition(0, 0, 0);
	CLevelGeneric::marine->SetVisible(false);

	CLevelGeneric::zombie = CGameObjectManager::Instance()->CreatePlayer("ZOMBIE1", EPT_TEAM_3, "", CLevelGeneric::physics, false);
	CLevelGeneric::zombie->SetPlayerPosition(0, 0, 0);
	CLevelGeneric::zombie->SetVisible(false);

	CLevelGeneric::robot = CGameObjectManager::Instance()->CreatePlayer("ROBOT1", EPT_TEAM_4, "", CLevelGeneric::physics, false);
	CLevelGeneric::robot->SetPlayerPosition(0, 0, 0);
	CLevelGeneric::robot->SetVisible(false);

	curr_player = NULL;
}

CEditorMenu::~CEditorMenu()
{
}

void CEditorMenu::btnPlayer_Click(MyGUI::WidgetPtr _sender)
{
	String value = cmbPlayers->getCaption().asUTF8();
	if(value == "")
	{
		return;
	}
	else if(value == "Fps Solider")
	{
		CLevelGeneric::solider->SetVisible(true);
		CLevelGeneric::marine->SetVisible(false);
		CLevelGeneric::zombie->SetVisible(false);
		CLevelGeneric::robot->SetVisible(false);
		curr_player = CLevelGeneric::solider;
	}
	else if(value == "General Marine")
	{
		CLevelGeneric::solider->SetVisible(false);
		CLevelGeneric::marine->SetVisible(true);
		CLevelGeneric::zombie->SetVisible(false);
		CLevelGeneric::robot->SetVisible(false);
		curr_player = CLevelGeneric::marine;
	}
	else if(value == "Zombie")
	{
		CLevelGeneric::solider->SetVisible(false);
		CLevelGeneric::marine->SetVisible(false);
		CLevelGeneric::zombie->SetVisible(true);
		CLevelGeneric::robot->SetVisible(false);
		curr_player = CLevelGeneric::zombie;
	}
	else if(value == "Robot")
	{
		CLevelGeneric::solider->SetVisible(false);
		CLevelGeneric::marine->SetVisible(false);
		CLevelGeneric::zombie->SetVisible(false);
		CLevelGeneric::robot->SetVisible(true);
		curr_player = CLevelGeneric::robot;
	}
}

void CEditorMenu::btnCharState_Click(MyGUI::WidgetPtr _sender)
{
	CPlayer* player = NULL;
	if(CLevelGeneric::marine->GetIsVisible())
		player = CLevelGeneric::marine;
	else if(CLevelGeneric::zombie->GetIsVisible())
		player = CLevelGeneric::zombie;
	else if(CLevelGeneric::solider->GetIsVisible())
		player = CLevelGeneric::solider;
	else if(CLevelGeneric::robot->GetIsVisible())
		player = CLevelGeneric::robot;

	if(!player)
		return;

	String value = cmbCharState->getCaption().asUTF8();
	if(value == "Wander")
	{
		player->SetCharacterState(EPS_WANDERING);
		player->SetTextLabelCaption("Wandering");
	}
	else if(value == "Seek")
	{
		player->SetCharacterState(EPS_SEEKING);
		player->SetTextLabelCaption("Seeking");
	}
	else if(value == "Evade")
	{
		player->SetCharacterState(EPS_EVADING);
		player->SetTextLabelCaption("Evading");
	}
	else if(value == "Seek Obs")
	{
		player->SetCharacterState(EPS_SEEK_OBS);
		player->SetTextLabelCaption("Seeking Obs");
	}
	else if(value == "Follow Path")
	{
		player->SetAiPathPoints(CLevelGeneric::FrameListenerFramework->PathPoints);
		player->SetCharacterState(EPS_WALKING);
		player->SetTextLabelCaption("Follow Path");
	}
	player->SetTextLabelEnabled(true);
}

void CEditorMenu::btnAddEnemy_Click(MyGUI::WidgetPtr _sender)
{
	// If its a marine or solider then add a zombie, otherwise add a solider
	CPlayer* player = NULL;
	if(CLevelGeneric::marine->GetIsVisible() || CLevelGeneric::solider->GetIsVisible())
		player = CGameObjectManager::Instance()->CreatePlayer("ZOMBIE1", EPT_TEAM_3, "", CLevelGeneric::physics, false);
	else
		player = CGameObjectManager::Instance()->CreatePlayer("SOLIDER1", EPT_TEAM_1, "", CLevelGeneric::physics, false);

	if(player)
		player->SetPlayerPosition(((float) rand ()) / ((float) RAND_MAX) * 200, 0, ((float) rand ()) / ((float) RAND_MAX) * 200);

	// Set the oposite character state of the current player
	if(curr_player)
	{
		Core::AI::CCharacterState* state = curr_player->GetCharacterState();
		if(state->GetStateType() == EPS_SEEKING)
			player->SetCharacterState(EPS_EVADING);
		else if(state->GetStateType() == EPS_EVADING || state->GetStateType() == EPS_WANDERING)
		{
			player->SetCharacterState(EPS_SEEKING);
			player->SetAiTarget(curr_player->GetAiVehicle());
		}
	}
}

void CEditorMenu::btnPhysAddBox_Click(MyGUI::WidgetPtr _sender)
{
	IGameObject* obj = CGameObjectManager::Instance()->CreateObject("CRATE_1");

	if(obj && CLevelGeneric::physics)
	{
		f32 x = CUtility::frandom2(-200, 200);
		f32 z = CUtility::frandom2(-200, 200);
		obj->Node->setPosition(reVector3Df(x, 50, z));
		CLevelGeneric::physics->AddCube(obj, 0, 0, 0);
	}
}

void CEditorMenu::btnPhysAddSphere_Click(MyGUI::WidgetPtr _sender)
{
	IGameObject* obj = CGameObjectManager::Instance()->CreateObject("CRATE_1");

	if(obj && CLevelGeneric::physics)
	{
		// find a sphere model
		f32 x = CUtility::frandom2(-200, 200);
		f32 z = CUtility::frandom2(-200, 200);
		obj->Node->setPosition(reVector3Df(x, 50, z));
		CLevelGeneric::physics->AddSphere(obj, 0, 0, 0);
	}
}

void CEditorMenu::btnObsAddBox_Click(MyGUI::WidgetPtr _sender)
{
	f32 x = CUtility::frandom2(-200, 200);
	f32 z = CUtility::frandom2(-200, 200);
	EditorHelper::AddObstacleBox(reVector3Df(x, 0, z));
}

void CEditorMenu::btnObsAddSphere_Click(MyGUI::WidgetPtr _sender)
{
	f32 x = CUtility::frandom2(-200, 200);
	f32 z = CUtility::frandom2(-200, 200);
	EditorHelper::AddObstacleSphere(reVector3Df(x, 0, z));
}

void CEditorMenu::btnPlaceObsBox_Click(MyGUI::WidgetPtr _sender)
{
	CLevelGeneric::FrameListenerFramework->btnPlaceObsBox_Click = true;
}

void CEditorMenu::btnPlaceObsSphere_Click(MyGUI::WidgetPtr _sender)
{
	CLevelGeneric::FrameListenerFramework->btnPlaceObsSphere_Click = true;
}

void CEditorMenu::btnAddPathPoint_Click(MyGUI::WidgetPtr _sender)
{
	CLevelGeneric::FrameListenerFramework->btnAddPathPoint_Click = true;
}

void CEditorMenu::btnAddWall_Click(MyGUI::WidgetPtr _sender)
{
	CLevelGeneric::FrameListenerFramework->btnAddWall_Click = true;
}

void CEditorMenu::SetVisible(bool Visible)
{
	frm->setVisible(Visible);
	MyGUI::PointerManager::getInstance().setVisible(Visible);
}