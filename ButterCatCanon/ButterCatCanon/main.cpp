// COMP710 GP 2D Framework

// Library includes:
#include <SDL.h>

// Local includes:
#include "game.h"
#include "logmanager.h"
#include <crtdbg.h>

int main(int argc, char* argv[])
{
	//_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	Game& gameInstance = Game::GetInstance();
	if (!gameInstance.Initialise())
	{
		LogManager::GetInstance().Log("Game Init Failed!");

		return (1);
	}
	
	while (gameInstance.DoGameLoop())
	{
		// No body.
	}

	Game::DestroyInstance();

	return (0);
}
