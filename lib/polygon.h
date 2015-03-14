#ifndef POLYGON_H
#define POLYGON_H

#include "framebuffer.h"
#include "drawable.h"
#include "line.h"
#include <vector>
#include <algorithm>

/*Floodfill Zaky*/
// simple flood fill algorithm to color the polygon.
// REMOVE THIS PART TOGETHER WITH THE fill() METHOD SOON, because:
// 1. This won't work if the screen contains many images
// 2. This won't work for some cases of concave and degenerate polygon
// 3. This uses extremely high memory
// 4. This method is slow
// 5. The code is ugly
class FloodFill {
public:
	static void main(Frame *_fb, Frame *_pattern, std::vector<Point> points, Color _defaultColor, Color _borderColor, Point offset = Point()) {
		fb = _fb, pattern = _pattern, borderColor = _borderColor, defaultColor = _defaultColor;
		visited.clear();

		visited.resize(fb->getXSize());
		for (int i = 0; i < visited.size(); ++i) {
			visited[i].resize(fb->getYSize());
		}

		// start from the centroid
		Point centroid;
		for (int i = 0; i < points.size(); ++i) {
			centroid.x += points[i].x;
			centroid.y += points[i].y;
		}
		centroid.x /= points.size();
		centroid.y /= points.size();
		centroid.x += offset.x;
		centroid.y += offset.y;

		fill(centroid, Point(0, 0));
	}

	static void fill(Point p, Point start) {
		// out of bounds
		if (p.x < 0 || p.y < 0 || p.x >= fb->getXSize() || p.y >= fb->getYSize()) return;
		
		if (visited[p.x][p.y]) return;
		
		// if we reach edge of polygon, return
		Color c = fb->get(p);
		if (c == borderColor) return;

		visited[p.x][p.y] = true;

		// set color at p
		Color patternColor;
		if ((pattern != NULL) and ((patternColor = pattern->get(start)) != Color::EMPTY))
			fb->set(p, patternColor);
		else
			fb->set(p, defaultColor);

		// move to four directions
		fill(Point(p.x, p.y + 1), Point(start.x, start.y+1));
		fill(Point(p.x, p.y - 1), Point(start.x, start.y-1));
		fill(Point(p.x + 1, p.y), Point(start.x+1, start.y));
		fill(Point(p.x - 1, p.y), Point(start.x-1, start.y));
	}
	static Frame *fb, *pattern;
	static Color defaultColor, borderColor;
	// Algo Zaky
	static std::vector<std::vector<bool> > visited;
};
Frame *FloodFill::fb = NULL, *FloodFill::pattern = NULL;
Color FloodFill::defaultColor, FloodFill::borderColor;
std::vector<std::vector<bool> > FloodFill::visited;

class RasterScan {
public:
	static void main(Frame *_fb, Frame *_pattern, Point topLeft, Point bottomRight, Color _defaultColor, Color _borderColor, Point offset = Point()) {
		Point patternPos(0, 0);
		for (int i = topLeft.y+offset.y; i <= bottomRight.y+offset.y; ++i) {
			int j = topLeft.x+offset.x;
			patternPos.x = 0;
			while (_fb->get(j, i) != _borderColor) {
				++j;
				patternPos.x++;
			}
			bool mark = true;
			int last = 0;
			for (; j <= bottomRight.x+offset.x; ++j) {
				if (_fb->get(Point(j, i)) == _borderColor) {
					if (!mark) {
						for (int it = last; it < j; ++it) {
							Color patternColor;
							if ((_pattern != NULL) and ((patternColor = _pattern->get(Point(patternPos.x-(j-it), patternPos.y))) != Color::EMPTY))
								_fb->set(Point(it, i), patternColor);
							else
								_fb->set(Point(it, i), _defaultColor);
						}
						bool below = (_fb->get(j-1, i-1) == _borderColor) or (_fb->get(j, i-1) == _borderColor) or (_fb->get(j+1, i-1) == _borderColor);
						bool above = (_fb->get(j-1, i+1) == _borderColor) or (_fb->get(j, i+1) == _borderColor) or (_fb->get(j+1, i+1) == _borderColor);
						while (_fb->get(j+1, i) == _borderColor)
							++j;
						bool cross = (above and ((_fb->get(j-1, i-1) == _borderColor) or (_fb->get(j, i-1) == _borderColor) or (_fb->get(j+1, i-1) == _borderColor))) or (below and ((_fb->get(j-1, i+1) == _borderColor) or (_fb->get(j, i+1) == _borderColor) or (_fb->get(j+1, i+1) == _borderColor)));
						below = below and ((_fb->get(j-1, i-1) == _borderColor) or (_fb->get(j, i-1) == _borderColor) or (_fb->get(j+1, i-1) == _borderColor));
						above = above and ((_fb->get(j-1, i+1) == _borderColor) or (_fb->get(j, i+1) == _borderColor) or (_fb->get(j+1, i+1) == _borderColor));
						if (not above)
							mark = false;
						else
							mark = true;
					}
				}
				else {
					if (mark) {
						last = j;
						mark = false;
					}
				}
				patternPos.x++;
			}
			patternPos.y++;
		}
	}
};

class Polygon : public Drawable {
public:
	Polygon(Color _color = Color::BLACK, Color _borderColor = Color::WHITE, int _thickness = 1, Frame *_pattern = NULL) {
		color = _color;
		borderColor = _borderColor;
		thickness = _thickness;
		pattern = _pattern;

		topLeft.x = 1<<31-1, bottomRight.x = 1<<31;
		topLeft.y = 1<<31-1, bottomRight.y = 1<<31;
	}

	void add(Point p) {
		topLeft.x = std::min(topLeft.x, p.x); topLeft.y = std::min(topLeft.y, p.y);
		bottomRight.x = std::max(bottomRight.x, p.x); bottomRight.y = std::max(bottomRight.y, p.y);

		points.push_back(p);
	}

	// methods for accessing points
	Point operator[](int i) const {
		return points[i];
	}
	Point& operator[](int i) {
		return points[i];
	}
	int size() {
		return points.size();
	}
	
	std::vector<Point>::iterator begin() {
		return points.begin();
	}

	std::vector<Point>::iterator end() {
		return points.end();
	}

	void rotate(const int& degree, const Point offset = Point(0, 0)) {
		for (std::vector<Point>::iterator point = points.begin(); point != points.end(); ++point) {
			point->rotate(degree, offset);
		} 
	}

	void draw(Frame* fb, Point offset = Point()) {
		// draw the border line
		for (int i = 1; i < points.size(); ++i)
			Line(points[i-1], points[i], borderColor, thickness).draw(fb, offset);

		// draw the last line. this makes Polygon different from Polyline
		Line(points[0], points[points.size() - 1], borderColor, thickness).draw(fb, offset);

		// draw the filling
		// drawFill(fb, pattern, offset);
	}

	void drawFill(Frame *fb, Frame *pattern, Point offset = Point()) {
		FloodFill::main(fb, pattern, points, color, borderColor, offset);
		// RasterScan::main(fb, pattern, topLeft, bottomRight, color, borderColor, offset);
	}

	void resize(float size, Point center = Point()) {
		for (int i = 0; i < points.size(); ++i)
			points[i].resize(size, center);
	}

	Point getTopLeft() {
		return topLeft;
	}

	Point getBottomRight() {
		return bottomRight;
	}

	void print() {
		for (std::vector<Point>::iterator point = points.begin(); point != points.end(); ++point) {
			printf("%d %d\n", point->x, point->y);
		}
	}
	std::vector<std::pair<Point, Point> > getLines() {
		std::vector<std::pair<Point, Point> > lines;
		for (int i = 0; i < size(); ++i) {
			lines.push_back(std::make_pair(points[i], points[(i + 1) % size()]));
		}
		return lines;
	}

	std::vector<Point> points;	// vector containing vertices of the polygon
	Color color;				// fill color of the polygon
	Color borderColor;			// line color of the polygon
	float thickness;			// thicknes of the border line
	Frame *pattern;

	Point topLeft, bottomRight;
};

#endif