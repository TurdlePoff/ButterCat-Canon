// COMP710 GP 2D Framework
#ifndef __GAME_H__
#define __GAME_H__

#include "Player.h"
#include "PowerUp.h"
#include "Bullet.h"
#include "Explosion.h"
#include "SoundSys.h"

#include <vector>
using namespace std;

// Forward Declarations
class BackBuffer;
class InputHandler;
class Sprite;

class Game
{
	//Member Methods:
public:
	static Game& GetInstance();
	static void DestroyInstance();
	~Game();

	bool Initialise();
	bool DoGameLoop();
	void Quit();
	bool isHit;

	void MoveCatLeft(); 
	void MoveCatRight();
	void PlayerJump();
	void JumpRelease();
	void StopCat();

	enum GameState {Menu, Churn, Aim, Fly, Slide};
	enum CanonState {One, Two, Thre, Four};
	GameState gameState;
	CanonState canonState;

	bool pJump;
	void SetCanonChurn();
	void SetCanonAim();

	void ButterCat(GameState state);
	void CanonOne(bool LR);
	void CanonTwo(bool LR);
	void CanonThree(bool LR);
	void CanonFour(bool LR);
	void SetCanonState(char state);
	void PauseChurn();
	void ResumeChurn();
	void ChurnLR(char lr);
	void ChangeGameState(char state);
	bool isLR = true;

	bool isStart = true;
	void StartExitGame();
	void SetStartExit(bool val);
	bool IsMenu();
	bool IsAiming();

	int canonStateCounter = 2;
	void SetCanonCounter(bool up);

	void EnableADButton(char ad, bool off);
	void ADButtonA();
	void ADButtonD();
	void ADButtonW();
	void ADButtonS();

	int churnCount = 0;
	void AddChurnCount();
	void EnableWSButton(bool off);
	void PowerUps();
	void CreateBanner();
	void AimSound();

	void CreateBg(int x, int y);
	void CreateBaguette(int x, int y);
	void SpawnPowerUp(int x, int y);
	void SpawnExplosion(int x, int y);
	
protected:
	void Process(float deltaTime);
	void Draw(BackBuffer& backBuffer);

private:
	Game(const Game& game);
	Game& operator=(const Game& game);
	
	Game();

	//Member Data:
public:

protected:
	static Game* sm_pInstance;
	BackBuffer* m_pBackBuffer;
	InputHandler* m_pInputHandler;
	bool m_looping;


	const int width = 800;
	const int height = 600;
	const int origHeight = 800;

	// Simulation Counters:
	float m_elapsedSeconds;
	float m_lag;
	float m_executionTime;
	int m_lastTime;
	int m_frameCount;
	int m_FPS;
	int m_numUpdates;
	bool m_drawDebugInfo;
	float jumpHeight;

	//PowerUp Values
	int rowDest = 80;
	int heightDest = 80;
	const int destCalc = width / 14;
	//Bg values
	int bgRow, bgCol = 0;
	int bgWidth = width / 5;
	int bgHeight = origHeight / 4;

	

	// Game Entities:
	// W02.1: Add a Player field.    
	Player* m_pPlayer;
	Player* m_Canon;

	// W02.2: Add an alien enemy container field.
	vector<PowerUp*> pPowerUpContainer;
	vector<PowerUp*>::iterator iterPup;
	vector<PowerUp*> pBgContainer;
	vector<PowerUp*>::iterator iterBg;
	vector<PowerUp*> pBagContainer;
	vector<PowerUp*>::iterator iterBag;
	vector<PowerUp*> pBannerContainer;

	// Explosion container
	PowerUp* pSinglePowerUp;

	//Sprites
	Sprite* gameBgSprite;
	PowerUp* pBg;
	Sprite* pPUpSprite;
	PowerUp* pPUp;
	Sprite* gameBagSprite;
	PowerUp* pBag;
	Sprite* pBannerSprite;
	PowerUp* pBanner;

	//Animated Sprites
	AnimatedSprite* pExplosionSprite;
	AnimatedSprite* pCanonSprite1;

	//Button sprites
	AnimatedSprite* pBSpriteA;
	AnimatedEntity* m_pBA;
	AnimatedSprite* pBSpriteD;
	AnimatedEntity* m_pBD;

	AnimatedSprite* pButtonUp;
	AnimatedSprite* pButtonDown;
	AnimatedSprite* pButtonJump;

	AnimatedSprite* pCanonSprite2;
	AnimatedSprite* pPlayerSprite;

	//FMOD
	SoundSys soundSys;
	SoundClass soundJump, soundExplosion, soundLoadCanon, soundItem, soundAim, soundSelect;
	SoundClass soundBG;

private:

};

#endif // __GAME_H__
