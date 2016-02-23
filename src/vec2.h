#ifndef VEC2_H
#define VEC2_H

#include <cmath>

class Vec2
{
protected:
	double _x;
	double _y;

public:
	Vec2(double x, double y):_x(x), _y(y)
	{}

	double x() const {
		return _x;
	}

	double y() const {
		return _y;
	}

	double mag() const {
		return sqrt( pow(_x,2) + pow(_y,2) );
	}

	Vec2 squared() const {
		return Vec2(_x*_x, _y*_y);
	}
};

Vec2 operator+(const Vec2 a, const Vec2 b){
	return Vec2(a.x()+b.x(), a.y()+b.y());
}

Vec2 operator-(const Vec2 a, const Vec2 b){
	return Vec2(a.x()-b.x(), a.y()-b.y());
}

#endif // VEC2_H
