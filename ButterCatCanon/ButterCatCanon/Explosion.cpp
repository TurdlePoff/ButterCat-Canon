#include "Explosion.h"
#include "AnimatedSprite.h"
#include "backbuffer.h"
#include <cassert>

Explosion::Explosion(AnimatedSprite* animatedSprite, int x, int y)
{
	this->animatedSprite = animatedSprite;
	animatedSprite->SetX(x);
	animatedSprite->SetY(y);
}

void
Explosion::Process(float deltaTime)
{
	animatedSprite->Process(deltaTime);
}

bool
Explosion::Initialise(AnimatedSprite* animatedSprite)
{
	assert(animatedSprite);
	this->animatedSprite = animatedSprite;
	return(true);
}

void 
Explosion::Draw(BackBuffer& backBuffer)
{
	animatedSprite->Draw(backBuffer);
}
