#include "Vec2.h"
#include <math.h>

Vec2::Vec2()
{
}

Vec2::Vec2(float xin, float yin): x(xin), y(yin)
{
}

bool Vec2::operator == (const Vec2& rhs) const 
{
	return (x == rhs.x) && (y == rhs.y);
};


bool Vec2::operator != (const Vec2& rhs) const
{
	return (x != rhs.x) || (y != rhs.y);
};

Vec2 Vec2::operator + (const Vec2& rhs) const
{
	return Vec2(x + rhs.x, y + rhs.y);
}

Vec2 Vec2::operator - (const Vec2& rhs) const 
{
	return Vec2(x - rhs.x, y - rhs.y);
}

Vec2 Vec2::operator / (const float val) const
{
	return Vec2(x / val, y / val);
}

Vec2 Vec2::operator * (const float val) const 
{
	return Vec2( x*val, y*val );
};

void Vec2::operator += (const Vec2& rhs)
{
	x += rhs.x;
	y += rhs.y;
};

void Vec2::operator -= (const Vec2& rhs)
{
	x -= rhs.x;
	y -= rhs.y;
};

void Vec2::operator *= (const float val)
{
	x *= val;
	y *= val;
};

void Vec2::operator /= (const float val)
{
	x /= val;
	y /= val;
};

Vec2 Vec2::dist(const Vec2& rhs) const
{
	float dX = rhs.x - x;
	float dY = rhs.y - y;
	return Vec2(dX, dY);
	//return sqrt(dX * dX + dY * dY);//slow
};

Vec2 Vec2::normalize() const
{
	float L = sqrtf(x * x + y * y);
	return Vec2(x / L, y / L);//unit vector
};