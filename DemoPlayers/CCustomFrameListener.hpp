#ifndef __CCUSTOM_FRAMELISTENER_H__
#define __CCUSTOM_FRAMELISTENER_H__

#include "CFrameListenerFramework.h"
#include "IGuiStrategy.h"
#include "CoreEngine.h"
#include "Methods.h"
#include "Wall.h"

namespace Core
{
	class CCustomFrameListener : public CFrameListenerFramework
	{
	public:
		bool				 btnPlaceObsBox_Click;		// is the place obstacle button clicked BOX
		bool				 btnPlaceObsSphere_Click;	// is the place obstacle button clicked SPHERE
		bool				 btnAddPathPoint_Click;		// is the add path point clicked
		bool				 btnAddWall_Click;			// is the add wall button clicked
		reVector3Df			 m_CurrWallFrom;			// pos of "from" wall
		u32					 m_WallCounter;				// how many times have we clicked since we added a wall
		ManualObject*		 m_WallManObj;				// current line for the wall
		Ogre::RaySceneQuery* mRayScnQuery;				// general query
		Vector<reVector3Df*> PathPoints;				// points for a path
		Vector<AI::Wall*>	 m_Walls;					// list of walls
		Vector<ManualObject*> m_WallManObjs;			// lines as final walls
		String				 m_WallMatNameRed;
		String				 m_WallMatNameWhite;

		CCustomFrameListener(RenderWindow* win) : CFrameListenerFramework(win) 
		{ 
			CInputManager::Instance()->GetKeyboard()->setEventCallback(this);
			CInputManager::Instance()->GetMouse()->setEventCallback(this);
			mRayScnQuery = CGameManager::Instance()->GetSceneManager()->createRayQuery(Ogre::Ray());
			btnPlaceObsBox_Click = false;
			btnAddPathPoint_Click = false;
			btnAddWall_Click = false;
			m_WallCounter = 0;
			m_WallManObj = NULL;
			m_WallMatNameRed = "wall_mat_red";
			m_WallMatNameWhite = "wall_mat_white";

			// Add scenarios to the console
			m_Console->AddCommandToHistory("scenario load arena");
			m_Console->AddCommandToHistory("scenario load sphere_obs");
			m_Console->AddCommandToHistory("scenario load follow_path");
			m_Console->AddCommandToHistory("scenario load zombie_seek");
			m_Console->AddCommandToHistory("scenario load test_wall");
		}

		bool frameRenderingQueued(const FrameEvent& evt)
		{
			//CFrameListenerFramework::frameRenderingQueued(evt);
			TimeUntilNextToggle -= evt.timeSinceLastFrame;

			if(m_Window->isClosed())	
				return false;

			if(m_Keyboard->isKeyDown(OIS::KC_SYSRQ) && TimeUntilNextToggle <= 0)
			{
				std::ostringstream ss;
				ss << "screenshot_" << ++NumScreenShots << ".png";
				m_Window->writeContentsToFile(ss.str());
				TimeUntilNextToggle = 0.5;
			}

			// Exit if we press Esc
			if(m_Keyboard->isKeyDown(OIS::KC_ESCAPE))
				return false;

			return true;
		}

		bool mouseMoved(const OIS::MouseEvent &arg)
		{
			if(btnAddWall_Click && m_WallCounter > 0)
			{
				MyGUI::IntPoint point = MyGUI::InputManager::getInstance().getMousePosition();	
				Ogre::Ray mouseRay = CCameraManager::Instance()->GetCamera()->getCameraToViewportRay(
					point.left / float(arg.state.width), point.top / float(arg.state.height));
				mRayScnQuery->setRay(mouseRay);

				Ogre::RaySceneQueryResult& result = mRayScnQuery->execute();
				for(Ogre::RaySceneQueryResult::iterator iter = result.begin();
					iter != result.end();
					iter++)
				{
					if(iter->movable)
					{ 
						if(iter->movable->getName() == "PlaneEntity")
						{
							reVector3Df pos = mouseRay.getPoint(iter->distance);
							CORE_TEXT("mouse_coor", StringConverter::toString(pos));

							if(m_WallManObj)
							{
								m_WallManObj->clear();
								m_WallManObj->begin(m_WallMatNameWhite, Ogre::RenderOperation::OT_LINE_LIST);
								m_WallManObj->position(m_CurrWallFrom);
								m_WallManObj->position(pos);
								m_WallManObj->end();
							}
							else
							{
								m_WallManObj = EditorHelper::Draw3D_Line(m_CurrWallFrom, pos, ColourValue::White, m_WallMatNameWhite);
							}
						}
					}
				}
			}
			
			if(m_GuiStrategy)
				return m_GuiStrategy->MouseMoved(arg);
			return true;
		}

		bool mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
		{
			if(btnPlaceObsBox_Click)
			{
				PlaceObsBox(float(arg.state.width), float(arg.state.height));
			}
			else if(btnPlaceObsSphere_Click)
			{
				PlaceObsSphere(float(arg.state.width), float(arg.state.height));
			}
			else if(btnAddPathPoint_Click)
			{
				AddPathPoint(float(arg.state.width), float(arg.state.height));
			}
			else if(btnAddWall_Click)
			{
				AddWall(float(arg.state.width), float(arg.state.height));
			}

			if(m_GuiStrategy)
				return m_GuiStrategy->MousePressed(arg, id);
			return true;
		}

		bool mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
		{
			if(m_GuiStrategy)
				return m_GuiStrategy->MouseReleased(arg, id);
			return true;
		}

		bool keyPressed(const OIS::KeyEvent &arg)
		{
			if(arg.key == OIS::KC_F12)
			{
				m_Console->SetVisible(!m_Console->GetVisible());
				CGameManager::Instance()->SetConsoleVisible(m_Console->GetVisible() || m_LuaConsole->IsVisible());
				return true;
			}
			else if(arg.key == OIS::KC_GRAVE)
			{
				m_LuaConsole->SetVisible(!m_LuaConsole->IsVisible());
				CGameManager::Instance()->SetConsoleVisible(m_Console->GetVisible() || m_LuaConsole->IsVisible());
				return true;
			}
			else if(arg.key == OIS::KC_ESCAPE)
			{
				CGameManager::Instance()->SetReadyToAdvance(true);
				return true;
			}
			else if(m_GuiStrategy && !CGameManager::Instance()->GetConsoleVisible())
			{
				return m_GuiStrategy->KeyPressed(arg);
			}
			// A key was pressed, pass to the consoles
			else
			{
				if(m_Console->GetVisible() == true)
				{
					m_Console->OnKeyPressed(arg);
				}
				else if(m_LuaConsole->IsVisible())
				{
					m_LuaConsole->OnKeyPressed(arg);
				}
			}

			return true;
		}

		bool keyReleased(const OIS::KeyEvent &arg)
		{
			if(m_GuiStrategy)
				return m_GuiStrategy->KeyReleased(arg);
			return true;
		}

	private:
		void PlaceObsBox(f32 Width, f32 Height)
		{
			MyGUI::IntPoint point = MyGUI::InputManager::getInstance().getMousePosition();	

			Ogre::Ray mouseRay = CCameraManager::Instance()->GetCamera()->getCameraToViewportRay(
				point.left / Width, 
				point.top / Height);

			mRayScnQuery->setRay(mouseRay);

			Ogre::RaySceneQueryResult& result = mRayScnQuery->execute();
			for(Ogre::RaySceneQueryResult::iterator iter = result.begin();
				iter != result.end();
				iter++)
			{
				if(iter->movable)
				{ 
					if(iter->movable->getName() == "PlaneEntity")
					{
						reVector3Df m_LastMousePlane = mouseRay.getPoint(iter->distance);
						CORE_TEXT("mouse_coor", StringConverter::toString(m_LastMousePlane));
						EditorHelper::AddObstacleBox(m_LastMousePlane + reVector3Df(0, 0, 0));
					}
				}
			}
			if(CInputManager::Instance()->GetKeyboard()->isKeyDown(OIS::KC_LSHIFT) ||
				CInputManager::Instance()->GetKeyboard()->isKeyDown(OIS::KC_RSHIFT))
			{
				btnPlaceObsBox_Click = true;
			}
			else
			{
				btnPlaceObsBox_Click = false;
			}
		}

		void PlaceObsSphere(f32 Width, f32 Height)
		{
			MyGUI::IntPoint point = MyGUI::InputManager::getInstance().getMousePosition();	

			Ogre::Ray mouseRay = CCameraManager::Instance()->GetCamera()->getCameraToViewportRay(
				point.left / Width, 
				point.top / Height);

			mRayScnQuery->setRay(mouseRay);

			Ogre::RaySceneQueryResult& result = mRayScnQuery->execute();
			for(Ogre::RaySceneQueryResult::iterator iter = result.begin();
				iter != result.end();
				iter++)
			{
				if(iter->movable)
				{ 
					if(iter->movable->getName() == "PlaneEntity")
					{
						reVector3Df m_LastMousePlane = mouseRay.getPoint(iter->distance);
						CORE_TEXT("mouse_coor", StringConverter::toString(m_LastMousePlane));
						EditorHelper::AddObstacleSphere(m_LastMousePlane + reVector3Df(0, 0, 0));
					}
				}
			}
			if(CInputManager::Instance()->GetKeyboard()->isKeyDown(OIS::KC_LSHIFT) ||
				CInputManager::Instance()->GetKeyboard()->isKeyDown(OIS::KC_RSHIFT))
			{
				btnPlaceObsSphere_Click = true;
			}
			else
			{
				btnPlaceObsSphere_Click = false;
			}
		}

		void AddPathPoint(f32 Width, f32 Height)
		{
			MyGUI::IntPoint point = MyGUI::InputManager::getInstance().getMousePosition();	

			Ogre::Ray mouseRay = CCameraManager::Instance()->GetCamera()->getCameraToViewportRay(
				point.left / Width, 
				point.top / Height);

			mRayScnQuery->setRay(mouseRay);

			Ogre::RaySceneQueryResult& result = mRayScnQuery->execute();
			for(Ogre::RaySceneQueryResult::iterator iter = result.begin();
				iter != result.end();
				iter++)
			{
				if(iter->movable)
				{ 
					if(iter->movable->getName() == "PlaneEntity")
					{
						reVector3Df m_LastMousePlane = mouseRay.getPoint(iter->distance);
						CORE_TEXT("mouse_coor", StringConverter::toString(m_LastMousePlane));
						reVector3Df* point = new reVector3Df();
						point->x = m_LastMousePlane.x;
						point->y = 1.0;
						point->z = m_LastMousePlane.z;
						EditorHelper::AddPathSphere((*point));
						PathPoints.push_back(point);

						// Draw a line between the last two points
						if(PathPoints.size() > 1)
						{
							reVector3Df* from = PathPoints[PathPoints.size() -1];
							reVector3Df* to = PathPoints[PathPoints.size() -2];
							EditorHelper::Draw3D_Line((*from), (*to), ColourValue::Blue, String(""));
						}
					}
				}
			}
			if(CInputManager::Instance()->GetKeyboard()->isKeyDown(OIS::KC_LSHIFT) ||
				CInputManager::Instance()->GetKeyboard()->isKeyDown(OIS::KC_RSHIFT))
			{
				btnAddPathPoint_Click = true;
			}
			else
			{
				btnAddPathPoint_Click = false;
			}
		}

		void AddWall(f32 Width, f32 Height)
		{
			MyGUI::IntPoint point = MyGUI::InputManager::getInstance().getMousePosition();	

			Ogre::Ray mouseRay = CCameraManager::Instance()->GetCamera()->getCameraToViewportRay(point.left / Width, point.top / Height);
			mRayScnQuery->setRay(mouseRay);

			Ogre::RaySceneQueryResult& result = mRayScnQuery->execute();
			for(Ogre::RaySceneQueryResult::iterator iter = result.begin();
				iter != result.end();
				iter++)
			{
				if(iter->movable)
				{ 
					if(iter->movable->getName() == "PlaneEntity")
					{
						if(m_WallCounter == 0)
						{
							m_CurrWallFrom = mouseRay.getPoint(iter->distance);
							m_CurrWallFrom.y = 1.0;
							CORE_TEXT("mouse_coor", StringConverter::toString(m_CurrWallFrom));
							m_WallCounter++;
						}
						else
						{
							if(m_WallManObj)
							{
								CGameManager::Instance()->GetSceneManager()->destroyManualObject(m_WallManObj);
								m_WallManObj = NULL;
							}

							reVector3Df to = mouseRay.getPoint(iter->distance);
							to.y = 1;
							CORE_TEXT("mouse_coor", StringConverter::toString(to));
							btnAddWall_Click = false;
							m_WallCounter = 0;

							// create the wall
							CGameObjectManager::Instance()->AddWall(m_CurrWallFrom, to);

							// Remove all wall lines
							//for(u32 i = 0; i < m_WallManObjs.size(); i++)
							//{
							//	CGameManager::Instance()->GetSceneManager()->destroyMovableObject(m_WallManObjs[i]);
							//}
							//m_WallManObjs.clear();

							// draw
							CGameObjectManager::Instance()->GetWalls(m_Walls);
							ManualObject* m = EditorHelper::Draw3D_Line(
								m_Walls[m_Walls.size() -1]->GetFrom(), m_Walls[m_Walls.size() -1]->GetTo(), ColourValue::Red, m_WallMatNameRed);

							if(m)
								m_WallManObjs.push_back(m);
							//for(u32 i = 0; i < m_Walls.size(); i++)
							//{
							//	m_WallManObjs.push_back(Draw3D_Line(m_Walls[i]->GetFrom(), m_Walls[i]->GetTo(), ColourValue::Blue));
							//}
							
						}
					}
				}
			}
		}


	};
}


#endif // __CCUSTOM_FRAMELISTENER_H__
