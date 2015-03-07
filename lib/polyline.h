#ifndef POLYLINE_H
#define POLYLINE_H

#include "drawable.h"
#include "line.h"
#include <vector>

class Polyline : public Drawable {
public:
	Polyline() {
		color = Color::WHITE;
		thickness = 1;
	}
	
	void add(Point p) {
		points.push_back(p);
	}
	int size() {
		return points.size();
	}

	// methods for accessing points
	Point operator[](int i) const {
		return points[i];
	}
	Point& operator[](int i) {
		return points[i];
	}

	void draw(Frame* fb, Point offset = Point()) {
		for (int i = 1; i < size(); ++i) {
			Line line(points[i-1], points[i], color, thickness);
			line.draw(fb, offset);
		}
	}

	void rotate(const int& degree, const Point offset = Point(0, 0)) {
		for (std::vector<Point>::iterator point = points.begin(); point != points.end(); ++point) {
			point->rotate(degree, offset);
		} 
	}

	void resize(float size, Point center = Point()) {
		for (int i = 0; i < points.size(); ++i)
			points[i].resize(size, Point());
	}

	std::vector<Point> points;
	Color color;
	float thickness;
};

#endif