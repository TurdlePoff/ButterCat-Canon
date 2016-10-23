#ifndef __EXPLOSION_H__
#define __EXPLOSION_H__
#include "entity.h"

class Explosion : public Entity
{
public:
	Explosion(AnimatedSprite* animatedSprite, int x, int y);
	void Process(float deltaTime);
	void Draw(BackBuffer& backBuffer);
	bool Initialise(AnimatedSprite* animatedSprite);
protected:
private:
public:
protected:
	AnimatedSprite* animatedSprite;
};

#endif //__EXPLOSION_H__