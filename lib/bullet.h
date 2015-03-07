#include "drawable.h"
#include "polyline.h"
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>

class Bullet : public Drawable { 
public:
	Bullet() {
		
	}

	void draw(Frame *fb, Point offset = Point()) {
		Line(offset + p, offset + size + p, Color::RED, 4, true).draw(fb);
	}

	bool collide(int x, int y) {
		return p.x <= x && x <= p.x + size.x
			&& p.y <= y && y <= p.y + size.y;
	}

	void update(float t) {
		// gerakin objek
		p.x += (int)(t * v.x);
		p.y += (int)(t * v.y);
	}

	Point p;
	Point size;
	Point v;
};
