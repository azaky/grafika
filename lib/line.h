/**
 * http://members.chello.at/~easyfilter/bresenham.html
 */

#ifndef LINE_H
#define LINE_H

#include "drawable.h"
#include "circle.h"
#include <stdlib.h>
#include <math.h>

class Line : public Drawable {
public:
	Line(Point p0 = Point(0, 0), Point p1 = Point(0, 0),
			Color color = Color::WHITE, float thickness = 1, bool autoInc = false) {
		this->p0 = p0;
		this->p1 = p1;
		this->thickness = thickness;
		this->color = color;
		this->autoInc = autoInc;
	}
	Line(int x0 = 0, int y0 = 0, int x1 = 0, int y1 = 0,
			Color color = Color::WHITE, float thickness = 1, bool autoInc = false) {
		this->p0 = Point(x0, y0);
		this->p1 = Point(x1, y1);
		this->thickness = thickness;
		this->color = color;
		this->autoInc = autoInc;
	}
	void draw(Frame* fb, Point offset = Point()) {
		int x0 = p0.x, y0 = p0.y, x1 = p1.x, y1 = p1.y;
		int dx =  abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
		int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1; 
		int err = dx + dy, e2; /* error value e_xy */

		int treshold;
		if (this->thickness)
			treshold = (dx-dy)/this->thickness;

		int bold = this->thickness;
		if (autoInc) {
			bold = 1;
		}

		int it = 0;
		for(;;){
			Circle circle(x0, y0, bold, color);
			circle.drawSolid(fb, offset);
			++it;
			if (autoInc) {
				if (it >= treshold) {
					++bold;
					it = 0;
				}
				if (bold == thickness)
					autoInc = false;
			}

			if (x0 == x1 && y0 == y1) break;
			e2 = err + err;
			if (e2 >= dy) { err += dy; x0 += sx; /* ++it; */} /* e_xy+e_x > 0 */
			if (e2 <= dx) { err += dx; y0 += sy; /* ++it; */} /* e_xy+e_y < 0 */
		}
	}

	Point p0, p1;
	Color color;
	float thickness;
	bool autoInc;
};

#endif
