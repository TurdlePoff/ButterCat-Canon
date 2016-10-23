// COMP710 GP 2D Framework

// This include:
#include "inputhandler.h"

// Local includes:
#include "game.h"
#include "Player.h"

// Library includes:
#include <cassert>

InputHandler::InputHandler()
: m_pGameController(0)
{

}

InputHandler::~InputHandler()
{
	if (m_pGameController)
	{
		SDL_JoystickClose(m_pGameController);
		m_pGameController = 0;
	}
}

bool 
InputHandler::Initialise()
{
	int numControllesr = SDL_NumJoysticks();

	m_pGameController = SDL_JoystickOpen(0);
	//assert(m_pGameController);
	jumpLeft = false;
	jumpRight = false;
	return (true);
}

void 
InputHandler::ProcessInput(Game& game)
{
	// W02.1: Receive Input Events below...
	SDL_Event e;
	while (SDL_PollEvent(&e) != 0)
	{
		/*else if (e.type == SDL_JOYBUTTONDOWN)
		{*/
		if (e.type == SDL_QUIT)
		{
			game.Quit();
		}
		else if (e.type == SDL_KEYDOWN)
		{
			// W02.3: Tell the game to fire a player bullet...
			//if (e.jbutton.button == 10)
			switch (e.key.keysym.sym){
			case(SDLK_SPACE) :
				if (game.IsMenu()){
					game.StartExitGame();
				}
				if (game.IsAiming()){
					
				}
				game.PlayerJump();
				break;
			case(SDLK_LEFT) :
				game.MoveCatLeft();
				jumpLeft = true;
				break;
			case(SDLK_RIGHT) :
				game.MoveCatRight();
				jumpRight = true;
				break;
			case(SDLK_UP) :
				game.SetStartExit(true);
				break;
			case(SDLK_DOWN) :
				game.SetStartExit(false);
				break;
			case(SDLK_a) :
				if (!isAButtonOn && !isLChurning){
					game.ChurnLR('l');
					isLChurning = true;
					isAButtonOn = true;
					/*game.EnableADButton('a', false);
					game.EnableADButton('d', true);*/
				}
				isRChurning = false;
				isDButtonOn = false;
				break;
			case(SDLK_d) :
				if (!isDButtonOn && !isRChurning){
					game.ChurnLR('r');
					isRChurning = true;
					isDButtonOn = true;
				}
				isLChurning = false;
				isAButtonOn = false;
				break;
			case(SDLK_w) :
				game.SetCanonCounter(true);
				game.AimSound();
				break;
			case(SDLK_s) :
				game.SetCanonCounter(false);
				game.AimSound();
				break;
			case(SDLK_1) :
				game.ChangeGameState('m');
				break;
			case(SDLK_2) :
				game.ChangeGameState('c');
				break;
			case(SDLK_3) :
				game.ChangeGameState('a');
				break;
			case(SDLK_4) :
				game.ChangeGameState('s');
				break;
			case(SDLK_5) :
				game.PowerUps();
				break;

			}
		}

		else if (e.type == SDL_KEYUP)
		{
			switch (e.key.keysym.sym){
			case(SDLK_LEFT) :
				jumpLeft = false;
				break;
			case(SDLK_RIGHT) :
				game.MoveCatRight();
				jumpRight = false;
				break;				
			}
			game.StopCat();
			game.JumpRelease();
		}

	}
}
