/**
 * http://members.chello.at/~easyfilter/bresenham.html
 */

#ifndef CIRCLE_H
#define CIRCLEs_H

#include "drawable.h"
#include <stdlib.h>
#include <math.h>

class Circle : public Drawable {
public:
	Circle(int x0 = 0, int y0 = 0, int radius = 1, Color color = Color::WHITE) {
		this->x0 = x0;
		this->y0 = y0;
		this->radius = radius;
		this->color = color;
	}
	void draw(Frame* fb, Point offset = Point()) {
		int x = radius, y = 0, radiusError = 1-x;
		while (x >= y) {
			fb->set(-x+x0+offset.x, y+y0+offset.y, color);
			fb->set(x+x0+offset.x, y+y0+offset.y, color);
			fb->set(-y+x0+offset.x, x+y0+offset.y, color);
			fb->set(y+x0+offset.x, x+y0+offset.y, color);
			fb->set(-x+x0+offset.x, -y+y0+offset.y, color);
			fb->set(x+x0+offset.x, -y+y0+offset.y, color);
			fb->set(-y+x0+offset.x, -x+y0+offset.y, color);
			fb->set(y+x0+offset.x, -x+y0+offset.y, color);
			y++;
			if (radiusError<0)
				radiusError += 2*y+1;
			else {
				x--;
				radiusError += 2*(y - x)+1;
			}
		}
	}
	void drawSolid(Frame* fb, Point offset = Point()) {
		int x = radius, y = 0, radiusError = 1-x;
		while (x >= y) {
			for (int it = -x; it <= x; ++it)
				fb->set(it+x0+offset.x, y+y0+offset.y, color);
			for (int it = -y; it <= y; ++it)
				fb->set(it+x0+offset.x, x+y0+offset.y, color);
			for (int it = -x; it <= x; ++it)
				fb->set(it+x0+offset.x, -y+y0+offset.y, color);
			for (int it = -y; it <= y; ++it)
				fb->set(it+x0+offset.x, -x+y0+offset.y, color);

			y++;
			if (radiusError<0)
				radiusError += 2*y+1;
			else {
				x--;
				radiusError += 2*(y - x)+1;
			}
		}
	}
	int x0, y0;
	int radius;
	Color color;
};

#endif

