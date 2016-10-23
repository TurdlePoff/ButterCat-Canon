// COMP710 GP 2D Framework

// This include:
#include "entity.h"

// Local includes:
#include "sprite.h"
#include "AnimatedSprite.h"
#include "backbuffer.h"

// Library includes:
#include <cassert>
#include <algorithm>
#include "Player.h"

Entity::Entity()
	: m_pSprite(0)
	, m_x(0.0f)
	, m_y(0.0f)
	, m_velocityX(0.0f)
	, m_velocityY(0.0f)
	, m_dead(false)
{

}

Entity::~Entity()
{
}

bool
Entity::Initialise(Sprite* sprite)
{
	assert(sprite);
	m_pSprite = sprite;

	return (true);
}

int Entity::GetSpriteWidth()
{
	return m_pSprite->GetWidth();
}


void
Entity::Process(float deltaTime)
{
	m_pSprite->SetX(static_cast<int>(m_x));
	m_pSprite->SetY(static_cast<int>(m_y));
	// W02.1: Generic position update, based upon velocity (and time).
	m_x += m_velocityX;
	m_y += m_velocityY;
	// W02.1: Boundary checking and position capping. 
	// W02.1: Boundary checking and position capping. 
	if (m_x > 720) {
		m_x = 720;
		m_velocityX = 0;
	}
	else if (m_x < 80) {
		m_x = 80;
		m_velocityX = 0;
	}

	if (m_y > 500) {
		m_y = 500;
		m_velocityY = 0;
	}
	else if (m_y < 80) {
		m_y = 80;
		m_velocityY = 0;
	}

}

void
Entity::Draw(BackBuffer& backBuffer)
{
	assert(m_pSprite);
	m_pSprite->SetX((int)m_x - (m_pSprite->GetWidth() / 2));
	m_pSprite->SetY((int)m_y - (m_pSprite->GetHeight() / 2));
	m_pSprite->Draw(backBuffer);

}

bool
Entity::IsCollidingWith(Entity& e)
{
	int r1 = m_pSprite->GetHeight() / 2;
	float x1 = GetPositionX();
	float y1 = GetPositionY();
	float x2 = e.GetPositionX();
	float y2 = e.GetPositionY();
	int r2 = e.m_pSprite->GetHeight() / 2;
	bool collide = false;
	int r1f = static_cast<int>(r1);
	int r2f = static_cast<int>(r2);
	float collision = (float)r1f + r2f;
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

void
Entity::SetDead(bool dead)
{
	m_dead = dead;
}

bool Entity::IsDead() const
{
	return m_dead;
}

float
Entity::GetPositionX()
{
	return (m_x);
}

float
Entity::GetPositionY()
{
	return (m_y);
}

void
Entity::SetPositionX(float x)
{
	m_x = x;
}

void
Entity::SetPositionY(float y)
{
	m_y = y;
}

float
Entity::GetHorizontalVelocity()
{
	return (m_velocityX);
}

float
Entity::GetVerticalVelocity()
{
	return (m_velocityY);
}

void
Entity::SetHorizontalVelocity(float x)
{
	m_velocityX = x;
}

void
Entity::SetVerticalVelocity(float y)
{
	m_velocityY = y;
}
