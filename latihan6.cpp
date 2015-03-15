#include "lib/framecanvas.h"
#include "lib/mouse.h"
#include "lib/cursor.h"
#include "lib/layer.h"
#include "lib/prism.h"
#include <ctime>
#define INDONESIAN_MAP_FILE "object/indonesia_10km"

int main() {
	FrameBuffer fb;

	// set up frame canvas
	FrameCanvas fc(1366, 786);
	fc.setViewPos(Point(100, 100));
	fc.scaleView(0.6);

	// set up theindonesia 3D polygon
	std::vector<Polygon> polygons;
	FILE *file = fopen(INDONESIAN_MAP_FILE, "r");
	if (file == NULL) return 0;
	int nPolygon, nPolyline;
	fscanf(file, "%d %d", &nPolygon, &nPolyline);
	while (nPolygon--) {
		int nPoint;
		fscanf(file, "%d", &nPoint);
		Polygon polygon(Color(127, 255, 127), Color::GREEN);
		while (nPoint--) {
			Point point;
			fscanf(file, "%f %f", &point.x, &point.y);

			polygon.add(point);
		}
		std::reverse(polygon.points.begin(), polygon.points.end());
		polygons.push_back(polygon);
	}
	fclose(file);
	Layer layer;
	// layer.add(&p);
	// layer.add(&q);
	// prism.addToLayer(layer);
	// qrism.addToLayer(layer);
	for (std::vector<Polygon>::iterator polygon = polygons.begin(); polygon != polygons.end(); ++polygon) {
		Prism prism(*polygon, 50);
		prism.addToLayer(layer);
	}

	// set the mouse
	Point axis, prevaxis;
	int wheel = 0, prevwheel = 0;
	int left, right, middle;
	int prevleft = 0;

	int framerate = 10;
	float period = 1. / framerate;
	float last_render = clock();

	int iframe = 0;

	Point offset = Point(200, 200);

	layer.draw(fc.getBgFrame(), Point(0, 100));
	fc.resetBackground();

	while (true) {
		// Draw objects
		fb.clear();
		fc.clear();

		if (iframe++ % 50 == 0) {
			system("clear");
			printf("Axis  : x = %f, y = %f, wheel = %d\n", axis.x, axis.y, wheel);
		}

		#ifdef DEBUG_MODE
		fc.drawBoundary(&fb, Color::CYAN, 3);
		#endif

		fc.draw(&fb);

		// Mouse
		axis.x = Mouse::getXAxis();
		axis.y = Mouse::getYAxis();
		wheel = Mouse::getWheelAxis();
		Mouse::getButton(&left, &middle, &right);

		// drag window
		if (Mouse::getLeftButton()) {
			Cursor::DRAG.draw(&fb, axis);
			// convert movement in the view to window
			Point d = prevaxis - axis;
			d.x *= fc.getWindowLen().x;
			d.y *= fc.getWindowLen().y;
			d.x /= fc.getViewLen().x;
			d.y /= fc.getViewLen().y;
			fc.moveWindow(d);
		} else {
			Cursor::DEFAULT.draw(&fb, axis);
		}

		if (wheel != prevwheel) {
			if (wheel > prevwheel) {
				fc.scaleWindow(0.9, fc.viewToWindow(axis) - fc.getWindowPos());
			} else {
				fc.scaleWindow(1.1, fc.viewToWindow(axis) - fc.getWindowPos());
			}
		}
		prevaxis = axis;
		prevwheel = wheel;

		// delay sesuai framerate
		float now_render = clock();
		// usleep(2e6);
		usleep(std::max(0., 1e6 / framerate - (now_render - last_render)));
		last_render = now_render;
	}

	return 0;
}
