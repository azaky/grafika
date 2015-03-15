#include "../lib/framebuffer.h"
#include "../lib/geometry.h"
#include "../lib/polygon.h"
#include "../lib/mouse.h"
#include "../lib/cursor.h"
#include "../lib/layer.h"
#include <time.h>

int main() {
	FrameBuffer fb;

	Polygon p(Color(255, 127, 127), Color::RED);
	p.add(Point(100, 100));
	p.add(Point(200, 100));
	p.add(Point(150, 200));
	p.add(Point(250, 150));
	p.add(Point(400, 250));
	p.add(Point(250, 300));
	p.add(Point(100, 250));

	Polygon q(Color(127, 127, 255), Color::BLUE);
	q.add(Point(200, 435));
	q.add(Point(200, 505));
	q.add(Point(325, 505));
	q.add(Point(300, 470));
	q.add(Point(350, 435));

	bool isInside = false;

	Point axis, prevaxis;
	int wheel = 0, prevwheel = 0;
	int left, right, middle;
	int prevleft = 0;

	int framerate = 20;
	float period = 1. / framerate;
	float last_render = clock();

	int iframe = 0;

	Point offset = Point(200, 200);

	while (true) {
		// Draw objects
		fb.clear();
		if (iframe++ % 5 == 0) {
			system("clear");
			printf("Axis  : x = %f, y = %f, wheel = %d\n", axis.x, axis.y, wheel);
			printf("isInsidePolygon = %s\n", isInside ? "true" : "false");
		}

		// Draw Polygon
		p.draw(&fb);
		q.draw(&fb);

		isInside = Geometry::isPointInsidePolygon(axis, p, &fb);

		// Mouse
		axis.x = Mouse::getXAxis();
		axis.y = Mouse::getYAxis();
		wheel = Mouse::getWheelAxis();
		Mouse::getButton(&left, &middle, &right);
		Cursor::DEFAULT.draw(&fb, axis);

		// delay sesuai framerate
		float now_render = clock();
		usleep(std::max(0., 1e6 / framerate - (now_render - last_render)));
		last_render = now_render;
	}
	
	return 0;
}
