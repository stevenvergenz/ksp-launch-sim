#include "vec2.h"

Vec2::Vec2() : _x(0.0), _y(0.0)
{}

Vec2::Vec2(double x, double y) : _x(x), _y(y)
{}

double Vec2::x() const {
	return _x;
}

double Vec2::y() const {
	return _y;
}

double Vec2::mag() const {
	return sqrt( pow(_x,2) + pow(_y,2) );
}

Vec2 Vec2::squared() const {
	return Vec2(_x*_x, _y*_y);
}

Vec2 Vec2::normalized() const {
	if(_x == 0.0 && _y == 0.0)
		return Vec2();
	else
		return Vec2(_x/this->mag(), _y/this->mag());
}



Vec2 operator+(const Vec2& a, const Vec2& b){
	return Vec2(a.x()+b.x(), a.y()+b.y());
}

Vec2 operator-(const Vec2& a, const Vec2& b){
	return Vec2(a.x()-b.x(), a.y()-b.y());
}

Vec2 operator-(const Vec2& a) {
	return Vec2(-a.x(), -a.y());
}

Vec2 operator*(const double a, const Vec2& b){
	return Vec2(a*b.x(), a*b.y());
}

Vec2 operator*(const Vec2& a, const double b){
	return b * a;
}
