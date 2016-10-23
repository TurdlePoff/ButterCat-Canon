#include "AnimatedEntity.h"

#include "AnimatedSprite.h"
#include "backbuffer.h"

#include <cassert>


AnimatedEntity::AnimatedEntity()
	: m_pAnimSprite(0)
	, posYs(0)
	, posXs(0)
{

}

AnimatedEntity::~AnimatedEntity()
{
}

void
AnimatedEntity::Process(float deltaTime)
{
	m_pAnimSprite->SetX(static_cast<int>(m_x));
	m_pAnimSprite->SetY(static_cast<int>(m_y));
	m_pAnimSprite->Process(deltaTime);
	m_x += m_velocityX;
	m_y += m_velocityY;
	if (m_x > 800 - 132)
	{
		m_x = 800 - 132;
		m_velocityX = 0;
	}
	else if (m_x < 0 + 132) {
		m_x = 0 + 132;
		m_velocityX = 0;
	}

	if (m_y > 600 - 132) {
		m_y = 600 - 132;
		m_velocityY = 0;
	}
	else if (m_y < 0 + 132) {
		m_y = 0 + 132;
		m_velocityY = 0;
	}
}

void
AnimatedEntity::Draw(BackBuffer& backBuffer)
{
	m_pAnimSprite->SetX((int)m_x - (m_pAnimSprite->GetFrameWidth() / 2));
	m_pAnimSprite->SetY((int)m_y - (m_pAnimSprite->GetFrameHeight() / 2));
	m_pAnimSprite->Draw(backBuffer);
}

bool
AnimatedEntity::Initialise(AnimatedSprite* animatedSprite)
{
	assert(animatedSprite);
	this->m_pAnimSprite = animatedSprite;

	return (true);
}

bool
AnimatedEntity::IsCollidingWithAnim(AnimatedEntity& e)
{
	int r1 = m_pAnimSprite->GetFrameWidth() / 2;
	if (this->IsDead())
	{
		r1 = 0;
		return false;
	}
	float x1 = GetPositionX();;
	float y1 = GetPositionY();
	float x2 = e.GetPositionX();
	float y2 = e.GetPositionY();
	int r2 = e.m_pAnimSprite->GetFrameWidth() / 2;
	collide = false;
	auto collision = r1 + r2;
	// W02.3: Generic Entity Collision routine.
	float actualdistance = sqrt(pow((x2 - x1), 2) + pow((y2 - y1), 2));
	// W02.3: Does this object collide with the e object?
	if (actualdistance < collision)
	{
		collide = true;
	}
	if (collide == true)
	{
		return(collide);
	}

	return (false); // W02.4 Change return value!
}


bool
AnimatedEntity::IsCollidingWithEnt(Entity& e)
{
	int r1 = m_pAnimSprite->GetFrameWidth() / 4;
	float x1 = GetPositionX();
	float y1 = GetPositionY();
	float x2 = e.GetPositionX();
	float y2 = e.GetPositionY();
	auto r2 = e.GetSpriteWidth()/2;
	collide = false;
	auto collision = r1 + r2;
	// W02.3: Generic Entity Collision routine.
	float actualdistance = sqrt(pow((x2 - x1), 2) + pow((y2 - y1), 2));
	// W02.3: Does this object collide with the e object?
	if (actualdistance < collision)
	{
		collide = true;
		return collide;
	}
	return false;
}

int
AnimatedEntity::GetPosYs()
{
	return m_pAnimSprite->GetPosYs();
}

void
AnimatedEntity::SetPosYs(int y)
{
	m_pAnimSprite->SetPosYs(y);
}

int
AnimatedEntity::GetPosXs()
{
	return m_pAnimSprite->GetPosXs();
}

void
AnimatedEntity::SetPosXs(int x)
{
	m_pAnimSprite->SetPosXs(x);
}