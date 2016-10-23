// COMP710 GP 2D Framework

// This includes:
#include "game.h"

// Local includes:
#include "backbuffer.h"
#include "inputhandler.h"
#include "logmanager.h"
#include "sprite.h"
#include "AnimatedSprite.h"
#include "Player.h"
#include "PowerUp.h"
#include "SoundSys.h"

// Library includes:
#include <cassert>
#include <SDL.h>
#include "SDL_ttf.h"
#include <stdio.h>

// Static Members:
Game* Game::sm_pInstance = 0;

Game&
Game::GetInstance()
{
	if (sm_pInstance == 0)
	{ 
		sm_pInstance = new Game();
	}

	assert(sm_pInstance);

	return (*sm_pInstance);
}

void 
Game::DestroyInstance()
{
	delete sm_pInstance; 
	sm_pInstance = 0;
}

Game::Game()
: m_pBackBuffer(0)
, m_pInputHandler(0)
, m_looping(true)
, m_executionTime(0)
, m_elapsedSeconds(0)
, m_frameCount(0)
, m_FPS(0)
, m_numUpdates(0)
, m_lastTime(0)
, m_lag(0)
{
	
}

Game::~Game()
{
	delete m_pBackBuffer;
	m_pBackBuffer = 0;
}

bool 
Game::Initialise()
{
	m_pBackBuffer = new BackBuffer();
	if (!m_pBackBuffer->Initialise(width, height))
	{
		LogManager::GetInstance().Log("BackBuffer Init Fail!");
		return (false);
	}

	m_pInputHandler = new InputHandler();
	if (!m_pInputHandler->Initialise())
	{
		LogManager::GetInstance().Log("InputHandler Init Fail!");
		return (false);
	}
	soundSys.createSound(&soundJump, "assets\\jumpSound.wav");
	soundSys.createSound(&soundLoadCanon, "assets\\loadCanon.wav");
	soundSys.createSound(&soundItem, "assets\\item.wav");
	soundSys.createSound(&soundAim, "assets\\aim.wav");
	soundSys.createSound(&soundSelect, "assets\\select.wav");


	for (float i = 0; i < 5; i++){
		// W02.2: Fill the container with these new enemies.
		CreateBg(width, 0);
	}

	bgCol = 0;
	for (float i = 0; i < width; i++){
		// W02.2: Fill the container with these new enemies.
		CreateBaguette(bgCol, 500);
		//bgRow += 192;
		bgCol += 383;
	}

	//Create Buttons
	m_pBA = new Player();
	m_pBD = new Player();
	
	//Create player cat and setup cat
	m_pPlayer = new Player();
	ButterCat(Slide);
	m_pPlayer->SetPositionX((float)width/2);
	m_pPlayer->SetPositionY((float)origHeight);
	m_pPlayer->SetHorizontalVelocity(0);
	m_pPlayer->SetVerticalVelocity(0);
	m_pPlayer->SetDead(false);
	//canon
	m_Canon = new Player();
	m_Canon->SetPositionX(0);
	m_Canon->SetPositionY((float)origHeight);
	m_Canon->SetHorizontalVelocity(0);
	m_Canon->SetVerticalVelocity(0);
	m_Canon->SetDead(false);
	CreateBanner();
	
	SetCanonChurn();
	canonState = Two;
	gameState = Menu;
	
	m_lastTime = SDL_GetTicks();
	m_lag = 0.0f;
	return (true);
}

bool 
Game::DoGameLoop()
{
	const float stepSize = 1.0f / 60.0f;

	assert(m_pInputHandler);
	m_pInputHandler->ProcessInput(*this);

	if (m_looping)
	{
		int current = SDL_GetTicks();
		float deltaTime = (current - m_lastTime) / 1000.0f;
		m_lastTime = current;

		m_executionTime += deltaTime;

		m_lag += deltaTime;

		while (m_lag >= stepSize)
		{
			Process(stepSize);

			m_lag -= stepSize;

			++m_numUpdates;
		}
		
		Draw(*m_pBackBuffer);
	}

	SDL_Delay(1);

	return (m_looping);
}

void 
Game::Process(float deltaTime)
{
	// Count total simulation time elapsed:
	m_elapsedSeconds += deltaTime;

	// Frame Counter:
	if (m_elapsedSeconds > 1)
	{
		m_elapsedSeconds -= 1;
		m_FPS = m_frameCount;
		m_frameCount = 0;
	}

	for (PowerUp* bag : pBagContainer){
		bag->Process(deltaTime);
	}

	// Update the game world simulation:
	for (PowerUp* bg : pBgContainer){
		bg->Process(deltaTime);
	}
	// Ex003.5: Process each alien enemy in the container.
	for (PowerUp* enemy : pPowerUpContainer){
		enemy->Process(deltaTime);
	}

	
	// W02.1: Update player...
	m_pPlayer->Process(deltaTime);
	ButterCat(Slide);
	m_Canon->Process(deltaTime);
	if (gameState == Churn || gameState == Aim){
		m_pBA->Process(deltaTime);
		m_pBD->Process(deltaTime);
	}

	if (gameState == Aim){
		m_pBA->SetPositionX((float)180);
		m_pBA->SetPositionY((float)280);
		pBSpriteA->SetX((int)m_pBA->GetPositionX());
		pBSpriteA->SetY((int)m_pBA->GetPositionY());
		pBSpriteA = m_pBackBuffer->CreateAnimatedSprite("assets\\buttons.png");
		m_pBA->Initialise(pBSpriteA);

		m_pBD->SetPositionX((float)180);
		m_pBD->SetPositionY((float)340);
		pBSpriteD->SetX((int)m_pBD->GetPositionX());
		pBSpriteD->SetY((int)m_pBD->GetPositionY());

		pBSpriteD = m_pBackBuffer->CreateAnimatedSprite("assets\\buttons.png");
		m_pBD->Initialise(pBSpriteD);
	}

	pBanner->Process(deltaTime);

	isHit = false;
	for (iterPup = pPowerUpContainer.begin(); iterPup < pPowerUpContainer.end();)
	{
		pSinglePowerUp = *iterPup;
		if (m_pPlayer->IsCollidingWithEnt(**iterPup)){
			soundSys.playSound(soundItem, false);
			delete *iterPup;
			iterPup = pPowerUpContainer.erase(iterPup);
			isHit = true;
		}
		else{
			iterPup++;
		}

	}
	CanonTwo(true);
	if (gameState == Aim){
		if (canonState == 0)
			CanonOne(isLR);
		else if (canonState == 1)
			CanonTwo(isLR);
		else if (canonState == 2)
			CanonThree(isLR);
		else if (canonState == 3)
			CanonFour(isLR);
	}
	
}


void 
Game::Draw(BackBuffer& backBuffer)
{
	++m_frameCount;

	backBuffer.Clear();
	//Text Settings
	SDL_Color white = { 255, 255, 255, 255 };
	SDL_Color yellow = { 255, 255, 0, 255 };
	SDL_Color black = { 0, 0, 0, 255 };
	char fps[5];
	char bCount[25];
	sprintf_s(fps, "%d", m_FPS);
	sprintf_s(bCount, "%d", churnCount);

	for (PowerUp* bg : pBgContainer){
		bg->Draw(backBuffer);
	}

	for (PowerUp* bag : pBagContainer){
		bag->Draw(backBuffer);
	}
	gameBgSprite->Draw(backBuffer);	// W02.2: Draw all bullets in container...
	//gameBagSprite->Draw(backBuffer);
	for (PowerUp* pUp : pPowerUpContainer){
		pUp->Draw(backBuffer);
	}

	// W02.1: Draw the player ship...
	if (gameState == Churn || gameState == Aim){
		m_Canon->Draw(backBuffer);
		m_pBA->Draw(backBuffer);
		m_pBD->Draw(backBuffer);
	}

	if (gameState == Aim){
		if (canonState == 0)
			CanonOne(isLR);
		else if (canonState == 1)
			CanonTwo(isLR);
		else if (canonState == 2)
			CanonThree(isLR);
		else if (canonState == 3)
			CanonFour(isLR);
	}

	pBanner->Draw(backBuffer);

	if (gameState == Slide){
		m_pPlayer->Draw(backBuffer);
	}

	m_pPlayer->Draw(backBuffer);
	// Draw Score Text
	m_pBackBuffer->DrawTextOnScreen(white, "fonts//bubble.ttf", "ButterCat Canon", (int)50.2, 97, 12);
	m_pBackBuffer->DrawTextOnScreen(yellow, "fonts//bubble.ttf", "ButterCat Canon", 50, 100, 10);
	m_pBackBuffer->DrawTextOnScreen(black, "fonts//bubble.ttf", "FrameCount: ", 11, width - 300, 77);
	m_pBackBuffer->DrawTextOnScreen(black, "fonts//bubble.ttf", fps, 20, width - 180, 70);
	m_pBackBuffer->DrawTextOnScreen(black, "fonts//bubble.ttf", "ChurnPower: ", 11, 130, 75);
	m_pBackBuffer->DrawTextOnScreen(black, "fonts//bubble.ttf", bCount, 20, 243, 70);
	if (gameState == Menu){
		if (isStart){
			m_pBackBuffer->DrawTextOnScreen(black, "fonts//bubble.ttf", "o", 20, 150, height / 2 - 48);
			m_pBackBuffer->DrawTextOnScreen(yellow, "fonts//bubble.ttf", "o", 19, 153, height / 2 - 50);
		}
		else{
			m_pBackBuffer->DrawTextOnScreen(black, "fonts//bubble.ttf", "o", 20, 160, height / 2 + 35);
			m_pBackBuffer->DrawTextOnScreen(yellow, "fonts//bubble.ttf", "o", 19, 163, height / 2 + 33);
		}
		m_pBackBuffer->DrawTextOnScreen(black, "fonts//bubble.ttf", "Start Game", 51, 202, height /2-50);
		m_pBackBuffer->DrawTextOnScreen(yellow, "fonts//bubble.ttf", "Start Game", 50, 205, height / 2-50);
		m_pBackBuffer->DrawTextOnScreen(black, "fonts//bubble.ttf", "Exit Game", 51, 222, height / 2 + 35);
		m_pBackBuffer->DrawTextOnScreen(yellow, "fonts//bubble.ttf", "Exit Game", 50, 225, height / 2+33);

	}

	if (gameState == Churn){
		m_pBackBuffer->DrawTextOnScreen(black, "fonts//bubble.ttf", "Click the 'A' and 'D'", (int)30.2, 230, 120);
		//m_pBackBuffer->DrawTextOnScreen(white, "fonts//bubble.ttf", "Click the 'A' and 'D'", (int)30.2, 234, 125);
		m_pBackBuffer->DrawTextOnScreen(yellow, "fonts//bubble.ttf", "Click the 'A' and 'D'", (int)30.2, 232, 122);
		m_pBackBuffer->DrawTextOnScreen(black, "fonts//bubble.ttf", "Buttons consecutively", (int)30.2, 225, 178);
		//m_pBackBuffer->DrawTextOnScreen(white, "fonts//bubble.ttf", "Buttons consecutively", (int)30.2, 229, 183);
		m_pBackBuffer->DrawTextOnScreen(yellow, "fonts//bubble.ttf", "Buttons consecutively", (int)30.2, 227, 180);
		m_pBackBuffer->DrawTextOnScreen(black, "fonts//bubble.ttf", "to charge up the canon!", (int)30.2, 225, 238);
		//m_pBackBuffer->DrawTextOnScreen(white, "fonts//bubble.ttf", "to charge up the canon!", (int)30.2, 229, 243);
		m_pBackBuffer->DrawTextOnScreen(yellow, "fonts//bubble.ttf", "to charge up the canon!", (int)30.2, 227, 240);
		m_pBackBuffer->DrawTextOnScreen(black, "fonts//bubble.ttf", bCount, 20, 127, 272);
		//m_pBackBuffer->DrawTextOnScreen(white, "fonts//bubble.ttf", bCount, 20, 132, 272);
		m_pBackBuffer->DrawTextOnScreen(yellow, "fonts//bubble.ttf", bCount, 20, 130, 270);
	}
	if (gameState == Aim){
		m_pBackBuffer->DrawTextOnScreen(black, "fonts//bubble.ttf", "Click the 'W' and 'S'", (int)30.2, 230, 120);
		m_pBackBuffer->DrawTextOnScreen(yellow, "fonts//bubble.ttf", "Click the 'W' and 'S'", (int)30.2, 232, 122);
		m_pBackBuffer->DrawTextOnScreen(black, "fonts//bubble.ttf", "Buttons to aim", (int)30.2, 230, 178);
		m_pBackBuffer->DrawTextOnScreen(yellow, "fonts//bubble.ttf", "Buttons to aim", (int)30.2, 232, 180);
		m_pBackBuffer->DrawTextOnScreen(black, "fonts//bubble.ttf", "and 'Space' to fire!", (int)30.2, 230, 238);
		m_pBackBuffer->DrawTextOnScreen(yellow, "fonts//bubble.ttf", "and 'Space' to fire!", (int)30.2, 232, 240);
	}
	backBuffer.Present();
}

void 
Game::Quit()
{
	m_looping = false;
}

void
Game::MoveCatLeft()
{
	m_pPlayer->SetHorizontalVelocity(-10.0f);
}

void
Game::MoveCatRight()
{
	m_pPlayer->SetHorizontalVelocity(+5.0f);
}

void
Game::StopCat()
{
	m_pPlayer->SetHorizontalVelocity(0.0f);
}

// W02.2: Spawn a PowerUp in game.
void 
Game::SpawnPowerUp(int x, int y)
{
	// W02.2: Load the power up sprite file.
	pPUpSprite = m_pBackBuffer->CreateSprite("assets\\bUp.png");
	pPUpSprite->SetX(x);
	pPUpSprite->SetY(y);
	// W02.2: Create a new PowerUp object. 
	pPUp = new PowerUp();
	pPUp->SetPositionX((float)x);
	pPUp->SetPositionY((float)y);
	pPUp->SetHorizontalVelocity(0);
	pPUp->SetVerticalVelocity(0);
	pPUp->Initialise(pPUpSprite);
	pPUp->SetDead(false);
	// W02.2: Add the new PowerUp to the enemy container.
	pPowerUpContainer.push_back(pPUp);
}

void
Game::CreateBg(int x, int y)
{
	// W02.2: Load the power up sprite file.
	gameBgSprite = m_pBackBuffer->CreateSprite("assets\\fullbg.png");
	gameBgSprite->SetX(x);
	gameBgSprite->SetY(y);
	// W02.2: Create a new PowerUp object. 
	pBg = new PowerUp();
	pBg->SetPositionX((float)x);
	pBg->SetPositionY((float)y);
	pBg->SetHorizontalVelocity(0);
	pBg->SetVerticalVelocity(0);
	pBg->Initialise(gameBgSprite);
	pBg->SetDead(false);
	// W02.2: Add the new PowerUp to the enemy container.
	pBgContainer.push_back(pBg);
}

void
Game::CreateBanner()
{
	// W02.2: Load the power up sprite file.
	pBannerSprite = m_pBackBuffer->CreateSprite("assets\\banner.png");
	pBannerSprite->SetX(width/2);
	pBannerSprite->SetY(0);
	// W02.2: Create a new PowerUp object. 
	pBanner = new PowerUp();
	pBanner->SetPositionX((float)width/2);
	pBanner->SetPositionY(0);
	pBanner->SetHorizontalVelocity(0);
	pBanner->SetVerticalVelocity(0);
	pBanner->Initialise(pBannerSprite);
	pBanner->SetDead(false);
}

void
Game::CreateBaguette(int x, int y)
{
	// W02.2: Load the power up sprite file.
	gameBagSprite = m_pBackBuffer->CreateSprite("assets\\midbgt.png");
	gameBagSprite->SetX(x);
	gameBagSprite->SetY(y);
	// W02.2: Create a new PowerUp object. 
	pBag = new PowerUp();
	pBag->SetPositionX((float)x);
	pBag->SetPositionY((float)y);
	pBag->SetHorizontalVelocity(0);
	pBag->SetVerticalVelocity(0);
	pBag->Initialise(gameBagSprite);
	pBag->SetDead(false);
	// W02.2: Add the new PowerUp to the enemy container.
	pBagContainer.push_back(pBag);
}

void
Game::SetCanonChurn()
{
	gameState = Churn;
	CanonTwo(true);
}

void
Game::SetCanonCounter(bool up){
	//Check if gamestate is Aiming
	if (gameState == Aim){
		//If user clicks up, add to counter, otherwise minus

		if (canonStateCounter > 4)
			canonStateCounter = 4;
		else if (canonStateCounter < 1)
			canonStateCounter = 1;

		if (up == true){
			--canonStateCounter;
		}
		else {
			++canonStateCounter;
		}
		//if counter is out of bounds, fix
		if (canonStateCounter > 4)
			canonStateCounter = 4;
		else if (canonStateCounter < 1)
			canonStateCounter = 1;

		//Set canon state
		if (canonStateCounter == 1)
			canonState = One;
		else if (canonStateCounter == 2)
			canonState = Two;
		else if (canonStateCounter == 3)
			canonState = Thre;
		else if (canonStateCounter == 4)
			canonState = Four;
	}
}

void
Game::CanonOne(bool LR)
{
	//enum GameState { Menu, Churn, Aim, Fly, Slide };
	//enum CanonState { One, Two, Three, Four };
	if (gameState == Aim && canonState == One){
		if (isLR){
			pCanonSprite1 = m_pBackBuffer->CreateAnimatedSprite("assets\\canonSpriteSheet.png");
			m_Canon->Initialise(pCanonSprite1);
			pCanonSprite1->SetFrameSpeed(0.0f);
			pCanonSprite1->SetFrameWidth(197);
			pCanonSprite1->SetFrameHeight(161);
			pCanonSprite1->SetNumOfFrames(0);
			pCanonSprite1->SetPosYs(0);
			pCanonSprite1->SetPosXs(0);
			pCanonSprite1->SetLooping(true);
			pCanonSprite1->SetX((int)m_Canon->GetPositionX());
			pCanonSprite1->SetY((int)m_Canon->GetPositionY());
			EnableWSButton(true);
		}
		else{
			pCanonSprite1 = m_pBackBuffer->CreateAnimatedSprite("assets\\canonSpriteSheet.png");
			m_Canon->Initialise(pCanonSprite1);
			pCanonSprite1->SetFrameSpeed(0.0f);
			pCanonSprite1->SetFrameWidth(197);
			pCanonSprite1->SetFrameHeight(161);
			pCanonSprite1->SetNumOfFrames(0);
			pCanonSprite1->SetPosYs(0);
			pCanonSprite1->SetPosXs(197);
			pCanonSprite1->SetLooping(true);
			pCanonSprite1->SetX((int)m_Canon->GetPositionX());
			pCanonSprite1->SetY((int)m_Canon->GetPositionY());
			EnableWSButton(false);
		}
	}
}

void
Game::CanonTwo(bool LR)
{
	//enum GameState { Menu, Churn, Aim, Fly, Slide };
	//enum CanonState { One, Two, Three, Four };
	if (gameState == Churn || gameState == Aim && canonState == Two){
		if (isLR){
			pCanonSprite1 = m_pBackBuffer->CreateAnimatedSprite("assets\\canonSpriteSheet.png");
			m_Canon->Initialise(pCanonSprite1);
			pCanonSprite1->SetFrameSpeed(0.0f);
			pCanonSprite1->SetFrameWidth(197);
			pCanonSprite1->SetFrameHeight(161);
			pCanonSprite1->SetNumOfFrames(0);
			pCanonSprite1->SetPosYs(161);
			pCanonSprite1->SetPosXs(0);
			pCanonSprite1->SetLooping(true);
			pCanonSprite1->SetX((int)m_Canon->GetPositionX());
			pCanonSprite1->SetY((int)m_Canon->GetPositionY());
			if (gameState == Churn){
				ADButtonA();
			}
			else if(gameState == Aim){
				EnableWSButton(true);
			}
		}
		else{
			pCanonSprite1 = m_pBackBuffer->CreateAnimatedSprite("assets\\canonSpriteSheet.png");
			m_Canon->Initialise(pCanonSprite1);
			pCanonSprite1->SetFrameSpeed(0.0f);
			pCanonSprite1->SetFrameWidth(197);
			pCanonSprite1->SetFrameHeight(161);
			pCanonSprite1->SetNumOfFrames(0);
			pCanonSprite1->SetPosYs(161);
			pCanonSprite1->SetPosXs(197);
			pCanonSprite1->SetLooping(true);
			pCanonSprite1->SetX((int)m_Canon->GetPositionX());
			pCanonSprite1->SetY((int)m_Canon->GetPositionY());
			if (gameState == Churn){
				ADButtonD();
			}else if (gameState == Aim){
				EnableWSButton(false);
			}
		}
	}
}

void
Game::CanonThree(bool LR)
{
	//enum GameState { Menu, Churn, Aim, Fly, Slide };
	//enum CanonState { One, Two, Three, Four };
	if (gameState == Aim && canonState == Thre){
		if (isLR){
			pCanonSprite1 = m_pBackBuffer->CreateAnimatedSprite("assets\\canonSpriteSheet.png");
			m_Canon->Initialise(pCanonSprite1);
			pCanonSprite1->SetFrameSpeed(0.0f);
			pCanonSprite1->SetFrameWidth(197);
			pCanonSprite1->SetFrameHeight(161);
			pCanonSprite1->SetNumOfFrames(0);
			pCanonSprite1->SetPosYs(322);
			pCanonSprite1->SetPosXs(0);
			pCanonSprite1->SetLooping(true);
			pCanonSprite1->SetX((int)m_Canon->GetPositionX());
			pCanonSprite1->SetY((int)m_Canon->GetPositionY());
			EnableWSButton(true);
		}
		else{
			pCanonSprite1 = m_pBackBuffer->CreateAnimatedSprite("assets\\canonSpriteSheet.png");
			m_Canon->Initialise(pCanonSprite1);
			pCanonSprite1->SetFrameSpeed(0.0f);
			pCanonSprite1->SetFrameWidth(197);
			pCanonSprite1->SetFrameHeight(161);
			pCanonSprite1->SetNumOfFrames(0);
			pCanonSprite1->SetPosYs(322);
			pCanonSprite1->SetPosXs(197);
			pCanonSprite1->SetLooping(true);
			pCanonSprite1->SetX((int)m_Canon->GetPositionX());
			pCanonSprite1->SetY((int)m_Canon->GetPositionY());
			EnableWSButton(false);
		}
	}
}

void
Game::CanonFour(bool LR)
{
	//enum GameState { Menu, Churn, Aim, Fly, Slide };
	//enum CanonState { One, Two, Three, Four };
	if (gameState == Aim && canonState == Four){
		if (isLR){
			pCanonSprite1 = m_pBackBuffer->CreateAnimatedSprite("assets\\canonSpriteSheet.png");
			m_Canon->Initialise(pCanonSprite1);
			pCanonSprite1->SetFrameSpeed(0.0f);
			pCanonSprite1->SetFrameWidth(197);
			pCanonSprite1->SetFrameHeight(161);
			pCanonSprite1->SetNumOfFrames(0);
			pCanonSprite1->SetPosYs(484);
			pCanonSprite1->SetPosXs(0);
			pCanonSprite1->SetLooping(true);
			pCanonSprite1->SetX((int)m_Canon->GetPositionX());
			pCanonSprite1->SetY((int)m_Canon->GetPositionY());
			EnableWSButton(true);
		}
		else{
			pCanonSprite1 = m_pBackBuffer->CreateAnimatedSprite("assets\\canonSpriteSheet.png");
			m_Canon->Initialise(pCanonSprite1);
			pCanonSprite1->SetFrameSpeed(0.0f);
			pCanonSprite1->SetFrameWidth(197);
			pCanonSprite1->SetFrameHeight(161);
			pCanonSprite1->SetNumOfFrames(0);
			pCanonSprite1->SetPosYs(484);
			pCanonSprite1->SetPosXs(197);
			pCanonSprite1->SetLooping(true);
			pCanonSprite1->SetX((int)m_Canon->GetPositionX());
			pCanonSprite1->SetY((int)m_Canon->GetPositionY());
			EnableWSButton(false);
		}
	}
}

void
Game::SetCanonState(char cState)
{
	switch (cState){
		case 'One':
			canonState = One;
			break;
		case 'Two':
			canonState = Two;
			break;
		case 'Thr':
			canonState = Thre;
			break;
		case 'Four':
			canonState = Four;
			break;
	}
}

void
Game::AimSound(){
	if (gameState == Aim){
		soundSys.playSound(soundAim, false);
	}
}

void
Game::EnableADButton(char ad, bool off)
{
	switch (ad){
		case 'a':{
			pBSpriteA = m_pBackBuffer->CreateAnimatedSprite("assets\\buttons.png");
			m_pBA->Initialise(pBSpriteA);
			m_pBA->SetPositionX((float)100);
			m_pBA->SetPositionY((float)320);
			m_pBA->SetHorizontalVelocity(0);
			m_pBA->SetVerticalVelocity(0);
			m_pBA->SetDead(false);
			if (off){
				pBSpriteA->SetFrameSpeed(0.0f);
				pBSpriteA->SetFrameWidth(50);
				pBSpriteA->SetFrameHeight(47);
				pBSpriteA->SetNumOfFrames(0);
				pBSpriteA->SetPosYs(1);
				pBSpriteA->SetPosXs(2);
				pBSpriteA->SetLooping(false);
				pBSpriteA->SetX((int)m_pBD->GetPositionX());
				pBSpriteA->SetY((int)m_pBD->GetPositionY());
			}
			else{
				pBSpriteA->SetFrameSpeed(0.0f);
				pBSpriteA->SetFrameWidth(50);
				pBSpriteA->SetFrameHeight(47);
				pBSpriteA->SetNumOfFrames(0);
				pBSpriteA->SetPosYs(1);
				pBSpriteA->SetPosXs(51);
				pBSpriteA->SetLooping(false);
				pBSpriteA->SetX((int)m_pBA->GetPositionX());
				pBSpriteA->SetY((int)m_pBA->GetPositionY());

			}

		}
		break;
		case 'd':{
			pBSpriteD = m_pBackBuffer->CreateAnimatedSprite("assets\\buttons.png");
			m_pBD->Initialise(pBSpriteD);
			m_pBD->SetPositionX((float)200);
			m_pBD->SetPositionY((float)320);
			m_pBD->SetHorizontalVelocity(0);
			m_pBD->SetVerticalVelocity(0);
			m_pBD->SetDead(false);
			if (off){
				pBSpriteD->SetFrameSpeed(0.0f);
				pBSpriteD->SetFrameWidth(50);
				pBSpriteD->SetFrameHeight(47);
				pBSpriteD->SetNumOfFrames(0);
				pBSpriteD->SetPosYs(47);
				pBSpriteD->SetPosXs(2);
				pBSpriteD->SetLooping(false);
				pBSpriteD->SetX((int)m_pBD->GetPositionX());
				pBSpriteD->SetY((int)m_pBD->GetPositionY());
			}
			else{
				pBSpriteD->SetFrameSpeed(0.0f);
				pBSpriteD->SetFrameWidth(50);
				pBSpriteD->SetFrameHeight(47);
				pBSpriteD->SetNumOfFrames(0);
				pBSpriteD->SetPosYs(47);
				pBSpriteD->SetPosXs(51);
				pBSpriteD->SetLooping(false);
				pBSpriteD->SetX((int)m_pBD->GetPositionX());
				pBSpriteD->SetY((int)m_pBD->GetPositionY());

			}
			
		}
	}
	
}

void
Game::EnableWSButton(bool off)
{
	switch (canonState){
		case One:
		case Two: {
		pBSpriteA = m_pBackBuffer->CreateAnimatedSprite("assets\\buttons.png");
		m_pBA->Initialise(pBSpriteA);
		m_pBA->SetPositionX((float)180);
		m_pBA->SetPositionY((float)300);
		m_pBA->SetHorizontalVelocity(0);
		m_pBA->SetVerticalVelocity(0);
		m_pBA->SetDead(false);
		if (off){
			pBSpriteA->SetFrameSpeed(0.0f);
			pBSpriteA->SetFrameWidth(50);
			pBSpriteA->SetFrameHeight(47);
			pBSpriteA->SetNumOfFrames(0);
			pBSpriteA->SetPosYs(95);
			pBSpriteA->SetPosXs(2);
			pBSpriteA->SetLooping(false);
			pBSpriteA->SetX((int)m_pBA->GetPositionX());
			pBSpriteA->SetY((int)m_pBA->GetPositionY());
		}
		else{
			pBSpriteA->SetFrameSpeed(0.0f);
			pBSpriteA->SetFrameWidth(50);
			pBSpriteA->SetFrameHeight(47);
			pBSpriteA->SetNumOfFrames(0);
			pBSpriteA->SetPosYs(95);
			pBSpriteA->SetPosXs(52);
			pBSpriteA->SetLooping(false);
			pBSpriteA->SetX((int)m_pBA->GetPositionX());
			pBSpriteA->SetY((int)m_pBA->GetPositionY());
		}
		break;
		}
		case Thre:
		case Four:{
			pBSpriteD = m_pBackBuffer->CreateAnimatedSprite("assets\\buttons.png");
			m_pBD->Initialise(pBSpriteD);
			m_pBA->SetPositionX((float)180);
			m_pBA->SetPositionY((float)340);
			m_pBD->SetHorizontalVelocity(0);
			m_pBD->SetVerticalVelocity(0);
			m_pBD->SetDead(false);
			if (off){
				pBSpriteD->SetFrameSpeed(0.0f);
				pBSpriteD->SetFrameWidth(50);
				pBSpriteD->SetFrameHeight(47);
				pBSpriteD->SetNumOfFrames(0);
				pBSpriteD->SetPosYs(142);
				pBSpriteD->SetPosXs(2);
				pBSpriteD->SetLooping(false);
			}
			else{
				pBSpriteD->SetFrameSpeed(0.0f);
				pBSpriteD->SetFrameWidth(50);
				pBSpriteD->SetFrameHeight(47);
				pBSpriteD->SetNumOfFrames(0);
				pBSpriteD->SetPosYs(142);
				pBSpriteD->SetPosXs(52);
				pBSpriteD->SetLooping(false);
				pBSpriteD->SetX((int)m_pBD->GetPositionX());
				pBSpriteD->SetY((int)m_pBD->GetPositionY());
			}
			break;

		}
	}

}

void
Game::ButterCat(GameState state)
{

	switch (state){
	case(Slide) : {

		pPlayerSprite = m_pBackBuffer->CreateAnimatedSprite("assets\\moveSpriteSheet.png");
		m_pPlayer->Initialise(pPlayerSprite);
		pPlayerSprite->SetFrameSpeed(1.0f);
		pPlayerSprite->SetFrameWidth(187);
		pPlayerSprite->SetFrameHeight(139);
		pPlayerSprite->SetNumOfFrames(2);
		pPlayerSprite->SetPosYs(1);
		pPlayerSprite->SetPosXs(187);
		pPlayerSprite->SetLooping(false);
		pPlayerSprite->SetX((int)m_pPlayer->GetPositionX());
		pPlayerSprite->SetY((int)m_pPlayer->GetPositionY());
		pPlayerSprite->StartAnimating();

	}break; 
	case(Fly) : {
		pPlayerSprite = m_pBackBuffer->CreateAnimatedSprite("assets\\moveSpriteSheet.png");
		m_pPlayer->Initialise(pPlayerSprite);
		pPlayerSprite->SetFrameSpeed(0.0f);
		pPlayerSprite->SetFrameWidth(187);
		pPlayerSprite->SetFrameHeight(139);
		pPlayerSprite->SetNumOfFrames(0);
		pPlayerSprite->SetPosYs(0);
		pPlayerSprite->SetPosXs(0);
		pPlayerSprite->SetLooping(true);
		pPlayerSprite->SetX((int)m_pPlayer->GetPositionX());
		pPlayerSprite->SetY((int)m_pPlayer->GetPositionY());

	}
	}
	pPlayerSprite->StartAnimating();
}

void
Game::PlayerJump()
{
	if (gameState == Slide){
		soundSys.playSound(soundJump, false);
		pJump = true;
		m_pPlayer->SetVerticalVelocity(-10.0f);
	}
}

void
Game::JumpRelease()
{
	pJump = false;
	if (m_pPlayer->GetPositionY() < origHeight){
		m_pPlayer->SetVerticalVelocity(+5.0f);
	}
}

void
Game::ChangeGameState(char state)
{
	switch (state){
	case 'm':
		gameState = Menu;
		break;
	case 'c':
		gameState = Churn;
		break;
	case 'a':{
		gameState = Aim;
	}
		break;
	case 's':
		gameState = Slide;
		break;
	}
}

void
Game::StartExitGame(){
	
	if (isStart){
		
		gameState = Churn;
		
	}
	else{
		Quit();
	}
}

void
Game::SetStartExit(bool val){
	if (gameState == Menu){
		soundSys.playSound(soundSelect, false);
	}
	isStart = val;
}

void
Game::ADButtonA()
{
	EnableADButton('a', true);
	EnableADButton('d', false);
}

void
Game::ADButtonD()
{
	EnableADButton('a', false);
	EnableADButton('d', true);
}


void
Game::ChurnLR(char lr)
{
	if (gameState == Churn){
		switch (lr){
		case 'l':
			isLR = true;
			break;
		case 'r':
			isLR = false;
			break;
		}
		soundSys.playSound(soundLoadCanon, false);
		AddChurnCount();
	}
}

bool
Game::IsMenu(){
	if (gameState == Menu){
		return true;
	}
	else{
		return false;
	}
}

bool
Game::IsAiming(){
	if (gameState == Aim){
		return true;
	}
	else{
		return false;
	}
}

void
Game::AddChurnCount()
{
	++churnCount;
}

void
Game::ResumeChurn()
{
	pCanonSprite1->Pause(false);
}

void
Game::PauseChurn()
{
	pCanonSprite1->Pause(true);
}

void
Game::PowerUps()
{
	/*for (int i = 0; i < 4; i++){
	for (int j = 0; j < 12; j++){*/
	// W02.2: Fill the container with these new enemies.
	//SpawnPowerUp(rowDest, heightDest);
	SpawnPowerUp(width / 2, height / 2);
	/*rowDest += bgWidth;
	}
	rowDest = 80;
	heightDest += destCalc;
	}*/
}