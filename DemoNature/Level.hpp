#include "CGameState.h"
#include "CFpsFrameListener.h"
#include "CCameraManager.h"
#include "CPlayer.h"

namespace Core
{
	class Level: public CGameState
	{
	private:
		CPlayer*			m_Player;
		CFpsFrameListener* m_FrameListenerFramework;

	public:
		Level() { }
		~Level() { }

		void Update(const f32& elapsedTime)
		{
		}

		void End()
		{
			m_GameManager->RemoveResourceArchive(".\\Media.zip", "Level");
		}

		bool Load()
		{
			m_GameManager->AddResourceArchive(".\\Media.zip", "Level");

			// setup camera
			CCameraManager::Instance()->CreateCamera(ECM_FREE);

			// Setup FrameListener
			m_FrameListenerFramework = new CFpsFrameListener(m_GameManager->GetRenderWindow());
			m_GameManager->GetEngineDevice()->addFrameListener(m_FrameListenerFramework);

			return true;
		}

	};
}
