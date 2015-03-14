#ifndef POINT_H
#define POINT_H

#include <math.h>

/**
 * A Class that represents 2D Point
 */
class Point {
public:
	Point() : x(0), y(0) {}
	Point(float _x, float _y) : x(_x), y(_y) {}
	
	/**
	 * Basic operations of Point
	 */
	Point operator+(const Point& rhs) {
		return Point(x+rhs.x, y+rhs.y);
	}

	Point operator-(const Point& rhs) {
		return Point(x-rhs.x, y-rhs.y);
	}

	Point operator+=(const Point& rhs) {
		x += rhs.x;
		y += rhs.y;
		return *this;
	}

	Point operator-=(const Point& rhs) {
		x -= rhs.x;
		y -= rhs.y;
		return *this;
	}

	bool operator==(const Point& rhs) {
		return fabs(x - rhs.x) < 1e-3 && fabs(y - rhs.y) < 1e-3;
	}

	bool operator< (const Point &rhs) {
		if (this->x != rhs.x) return this->x < rhs.x;
		return this->y < rhs.y;
	}

	/**
	 * Dilate this point with a factor of size and with respect to Point center
	 */
	void resize(float size, const Point& center = Point()) {
		*this -= center;
		x *= size;
		y *= size;
		*this += center;
	}

	/**
	 * Rotate this point with respect to Point center.
	 * The orientation depends on what coordinate system is being used. For a 
	 * Cartesian Coordinate System, the orientation will be counter-clockwise.
	 */
	void rotate(const float degree, const Point& center = Point()) {
		*this -= center;
		Point temp = *this;
		float rad = degree*3.14159265/180.0;
		x = temp.x*cos(rad)-temp.y*sin(rad);
		y = temp.x*sin(rad)+temp.y*cos(rad);
		*this += center;
	}

	float x, y;
};

/** basic operators and functions of point **/
Point operator-(const Point &p1, const Point &p2) {
	return Point(p1.x - p2.x, p1.y - p2.y);
}
Point operator+(const Point &p1, const Point &p2) {
	return Point(p1.x + p2.x, p1.y + p2.y);
}
Point operator*(const Point &p, const double &s) {
	return Point(p.x * s, p.y * s);
}
Point operator/(const Point &p, const double &s) {
	return p * (1./s);
}
bool operator<(const Point &p1, const Point &p2) {
	if (p1.x != p2.x) return p1.x < p2.x;
	return p1.y < p2.y;
}
bool operator==(const Point &p1, const Point &p2) {
	return fabs(p1.x - p2.x) < 1e-3 && fabs(p1.y - p2.y) < 1e-3;
}

#endif
