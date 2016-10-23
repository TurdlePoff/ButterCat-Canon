#ifndef EXPLOSION_H
#define EXPLOSION_H
#include "entity.h"

class AnimatedEntity : public Entity
{
public:
	AnimatedEntity();
	~AnimatedEntity();
	void Process(float deltaTime);
	void Draw(BackBuffer& backBuffer);
	bool Initialise(AnimatedSprite* animatedSprite);
	bool IsCollidingWithAnim(AnimatedEntity& e);
	bool IsCollidingWithEnt(Entity& e);

	int GetPosYs();
	void SetPosYs(int posYs);
	int GetPosXs();
	void SetPosXs(int posXs);

protected:
private:
public:
protected:
	AnimatedSprite* m_pAnimSprite;
	int posYs;
	int posXs;
	bool collide;
	bool walkableWater, walkableNarrow, walkableLow;
};
#endif