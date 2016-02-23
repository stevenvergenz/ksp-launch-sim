#ifndef VEC2_H
#define VEC2_H

#include <cmath>

class Vec2
{
protected:
	double _x;
	double _y;

public:
	Vec2();
	Vec2(double x, double y);

	double x() const;
	double y() const;

	double mag() const;
	Vec2 squared() const;
	Vec2 normalized() const;

};


Vec2 operator+(const Vec2& a, const Vec2& b);
Vec2 operator-(const Vec2& a, const Vec2& b);
Vec2 operator-(const Vec2& a);
Vec2 operator*(const double a, const Vec2& b);
Vec2 operator*(const Vec2& a, const double b);

#endif // VEC2_H
