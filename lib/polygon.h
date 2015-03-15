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

struct GlobalEdge {
	float miny, maxy;
	float minx;
	float inverseSlope;
	bool operator< (const GlobalEdge &edge) {
		if (this->miny != edge.miny) {
			return this->miny < edge.miny;
		} else {
			return this->minx < edge.minx;
		}
	}
};
bool operator<(const GlobalEdge &a, const GlobalEdge &b) {
	if (a.miny != b.miny) {
		return a.miny < b.miny;
	} else {
		return a.minx < b.minx;
	}
}
struct ActiveEdge {
	float maxy;
	float xval;
	float inverseSlope;
	bool operator< (const ActiveEdge &edge) {
		return this->xval < edge.xval;
	}
};
bool operator<(const ActiveEdge &a, const ActiveEdge &b) {
	return a.xval < b.xval;
}

/**
 * Source: http://www.cs.rit.edu/~icss571/filling/how_to.html
 */
class ScanLine {
public:
	static void main(Frame *_fb, Frame *_pattern, const std::vector<Point> &points, Color _defaultColor, Color _borderColor, Point offset = Point()) {
		
		if (!points.size()) {
			return;
		}

		int n = points.size();

		// initialize the global edge table
		std::vector<GlobalEdge> globalEdge;

		for (int i = 0; i < n; ++i) {
			Point a = points[i];
			Point b = points[(i+1)%n];

			// discard edges with slope of zero
			if (a.y == b.y) {
				continue;
			}

			if (b.y < a.y) {
				std::swap(a, b);
			}

			GlobalEdge edge;
			edge.miny = a.y;
			edge.maxy = b.y;
			edge.minx = a.x;
			edge.inverseSlope = (a.x - b.x) / (a.y - b.y);
			globalEdge.push_back(edge);
		}
		std::sort(globalEdge.begin(), globalEdge.end());
		std::reverse(globalEdge.begin(), globalEdge.end());

		// initialize the active edge table
		std::vector<ActiveEdge> activeEdge;
		int yval = (int)globalEdge.back().miny - 1;

		Point start(0.0, 0.0);
		int it = 0;
		do {
			// draw all pixels
			for (int i = 0; i < activeEdge.size(); i += 2) {
				int xstart = (int)activeEdge[i].xval;
				int xfinish = (int)activeEdge[i+1].xval;
				start.x = xstart;
				for (int x = xstart; x <= xfinish; ++x) {
					Color patternColor;
					if ((_pattern != NULL) and ((patternColor = _pattern->get(start)) != Color::EMPTY))
						_fb->set(offset + Point(x, yval), patternColor);
					else{
						_fb->set(offset + Point(x, yval), _defaultColor);
					}
					start.x += 1.0;
				}
			}

			// increase the scan-line by one
			yval++;
			start.y += 1.0;

			// remove any edges from the active edge table for which the maximum y value is equal to the scan line
			std::vector<ActiveEdge> newActiveEdge;
			for (int i = 0; i < activeEdge.size(); ++i) {
				if ((int)activeEdge[i].maxy > yval) {
					newActiveEdge.push_back(activeEdge[i]);
				}
			}
			activeEdge = newActiveEdge;

			// update the x-value for each active edges
			for (int i = 0; i < activeEdge.size(); ++i) {
				activeEdge[i].xval += activeEdge[i].inverseSlope;
			}

			// Remove any edges from the global edge table for which the minimum y value is equal to the scan-line and place them in the active edge table.
			while (!globalEdge.empty() && (int)globalEdge.back().miny == yval) {
				ActiveEdge edge;
				edge.maxy = globalEdge.back().maxy;
				edge.xval = globalEdge.back().minx;
				edge.inverseSlope = globalEdge.back().inverseSlope;
				activeEdge.push_back(edge);
				globalEdge.pop_back();
			}

			// Reorder the edges in the active edge table according to increasing x value
			std::sort(activeEdge.begin(), activeEdge.end());

		} while (!activeEdge.empty());
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
	int size() const {
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
		// // draw the filling
		// drawFill(fb, pattern, offset);

		// draw the border line
		for (int i = 1; i < points.size(); ++i)
			Line(points[i-1], points[i], borderColor, thickness).draw(fb, offset);

		// draw the last line. this makes Polygon different from Polyline
		Line(points[0], points[points.size() - 1], borderColor, thickness).draw(fb, offset);
		
		// // draw the filling
		// drawFill(fb, pattern, offset);
	}

	void drawFill(Frame *fb, Frame *pattern = NULL, Point offset = Point()) {
		// FloodFill::main(fb, pattern, points, color, borderColor, offset);
		// RasterScan::main(fb, pattern, topLeft, bottomRight, color, borderColor, offset);
		ScanLine::main(fb, pattern, points, color, borderColor, offset);
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
			printf("%d %d\n", (int)point->x, (int)point->y);
		}
	}
	std::vector<Line> getLines() {
		std::vector<Line> lines;
		for (int i = 0; i < size(); ++i) {
			lines.push_back(Line(points[i], points[(i + 1) % size()], borderColor, thickness));
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