#ifndef __CGAME_H__
#define __CGAME_H__

#include "IGameFramework.h"
#include "CLevelSpace.h"

namespace Core
{
	class CGame : public IGameFramework
	{
	private:
		CGameState* Level;

		void ShouldQuit(bool isQuit);
		void Advance();

	public:
		CGame();
		~CGame();

		void Run();
		void Update();
	};
}

#endif // __CGAME_H__
