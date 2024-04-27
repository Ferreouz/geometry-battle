#pragma once
#include "Vec2.h"
#include <SFML/Graphics.hpp>

class CTransform
{
public:
	Vec2 pos = { 0.0, 0.0 };
	Vec2 velocity = { 0.0, 0.0 };
	float angle, rotationAngle;

	CTransform(const Vec2& p, const Vec2& v, float a)
		: pos(p), velocity(v), angle(a), rotationAngle(a) {}
};

class CShape
{
public:
	sf::CircleShape circle;

	CShape(float radius, int point, const sf::Color& fill, const sf::Color& outline, float thickness)
		: circle(radius, point)
	{
		circle.setFillColor(fill);
		circle.setOutlineColor(outline);
		circle.setOutlineThickness(thickness);
		circle.setOrigin(radius, radius); // makes x,y the middle of the circle
	}
};

class CCollision
{
public:
	float radius = 0;

	CCollision(float r)
		: radius(r){}
};

class CScore
{
public:
	int score = 0;

	CScore(int s)
		: score(s) {}
};

class CLifespan
{
public:
	int remaining = 0;
	int total = 0; //inital amount

	CLifespan(int _total)
		: total(_total), remaining(_total) {}
};

class CInput
{
public:
	bool up = false;
	bool left = false;
	bool right = false;
	bool down = false;
	bool shoot = false;

	CInput() {}
};