// COMP710 GP 2D Framework
#ifndef __INPUTHANDER_H__
#define __INPUTHANDER_H__

// Library includes:
#include <SDL.h>

// Forward Declarations:
class Game;

class InputHandler
{
	//Member Methods:
public:
	InputHandler();
	~InputHandler();

	bool Initialise();
	void ProcessInput(Game& game);
	bool jumpLeft;
	bool jumpRight;

	bool isAButtonOn = false;
	bool isDButtonOn = false;
	bool isLChurning = false;
	bool isRChurning = false;
protected:

private:
	InputHandler(const InputHandler& inputHandler);
	InputHandler& operator=(const InputHandler& inputHandler);

	//Member Data:
public:

protected:
	SDL_Joystick* m_pGameController;

private:

};

#endif // __INPUTHANDER_H__
