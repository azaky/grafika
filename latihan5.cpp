#include "lib/canvas.h"
#include "lib/mouse.h"
#include "lib/cursor.h"
#include "lib/sprite.h"
#include <time.h>

#define INDONESIAN_MAP_FILE "object/indonesia_1km"

int main() {
	FrameBuffer fb;

	Sprite mapOfIndonesia(Color::EMPTY, Color::GREEN);
	mapOfIndonesia.add(INDONESIAN_MAP_FILE);

	Canvas canvas(Point(1366, 768), Point(fb.getXSize(), fb.getYSize()));
	canvas.addSprite(&mapOfIndonesia);
	// canvas.addPolygonFile(INDONESIAN_MAP_FILE);

	Canvas minimap(Point(1366, 768), Point(228, 128));
	minimap.addSprite(&mapOfIndonesia);
	// minimap.addPolygonFile(INDONESIAN_MAP_FILE);
	minimap.setViewPos(Point(fb.getXSize(), fb.getYSize()) - minimap.getViewLen());

	// Misc Objects
	// Ship
	Sprite ship(Color::EMPTY, Color::RED);
	ship.add("object/ship_above");
	ship.normalize();
	ship.resize(0.4);
	ship.setV(Point(0, 30));
	canvas.addSprite(&ship);
	minimap.addSprite(&ship);
	Polyline shipPath;
	shipPath.add(Point(406, 391));
	shipPath.add(Point(575, 412));
	shipPath.add(Point(610, 411));
	shipPath.add(Point(634, 390));
	shipPath.add(Point(662, 341));
	shipPath.add(Point(671, 334));
	shipPath.add(Point(683, 337));
	shipPath.add(Point(766, 364));
	shipPath.add(Point(769, 373));
	shipPath.add(Point(776, 390));
	shipPath.add(Point(795, 437));
	shipPath.add(Point(837, 457));
	shipPath.add(Point(909, 450));
	shipPath.add(Point(1038, 425));
	shipPath.add(Point(1120, 376));
	shipPath.add(Point(1191, 342));
	float shipV = 30;
	float shipT = 0;
	Sprite dummyShip(ship);

	// Plane
	Sprite plane(Color::EMPTY, Color::BLUE);
	plane.add("object/plane_above");
	plane.normalize();
	plane.resize(0.13);
	plane.setV(Point(0, 30));
	canvas.addSprite(&plane);
	minimap.addSprite(&plane);
	Polyline planePath;
	planePath.add(Point(7, 13));
	planePath.add(Point(65, 52));
	planePath.add(Point(190, 181));
	planePath.add(Point(286, 270));
	planePath.add(Point(337, 367));
	planePath.add(Point(391, 409));
	planePath.add(Point(433, 428));
	planePath.add(Point(603, 468));
	planePath.add(Point(651, 480));
	planePath.add(Point(748, 489));
	planePath.add(Point(876, 487));
	planePath.add(Point(1000, 456));
	planePath.add(Point(1145, 391));
	planePath.add(Point(1340, 297));
	float planeV = 80;
	float planeT = 0;
	Sprite dummyPlane(plane);

	bool focusToPlane = false;
	bool focusToShip = false;

	Point axis, prevaxis;
	int wheel = 0, prevwheel = 0;
	int left, right, middle;
	int prevleft = 0;

	int framerate = 15;
	float period = 1. / framerate;
	float last_render = clock();
	
	int iframe = 0;

	while (true) {
		// Draw objects
		fb.clear();
		if (iframe++ % 20 == 0) {
			system("clear");
			printf("Axis  : x = %d, y = %d, wheel = %d\n", axis.x, axis.y, wheel);
			printf("Button: left = %d, middle = %d, right = %d\n", left, middle, right);
			printf("Canvas: viewPos = (%d, %d), viewLen = (%d, %d), windowPos = (%d, %d), windowLen = (%d, %d)\n", canvas.getViewPos().x, canvas.getViewPos().y, canvas.getViewLen().x, canvas.getViewLen().y, canvas.getWindowPos().x, canvas.getWindowPos().y, canvas.getWindowLen().x, canvas.getWindowLen().y);
			printf("Focus : ship: %d, plane: %d\n", focusToShip, focusToPlane);
		}

		// draw canvas
		canvas.draw(&fb);

		// draw minimap
		minimap.draw(&fb);
		minimap.drawBoundary(&fb, Color::BLUE, 1);

		// draw view position inside the minimap
		Point topLeftMinimap = minimap.windowToView(canvas.getWindowPos());
		Point bottomRightMinimap = minimap.windowToView(canvas.getWindowPos() + canvas.getWindowLen());
		if (topLeftMinimap.x < minimap.getViewPos().x) {
			topLeftMinimap.x = minimap.getViewPos().x;
		}
		if (topLeftMinimap.y < minimap.getViewPos().y) {
			topLeftMinimap.y = minimap.getViewPos().y;
		}
		if (bottomRightMinimap.x < minimap.getViewPos().x) {
			bottomRightMinimap.x = minimap.getViewPos().x;
		}
		if (bottomRightMinimap.y < minimap.getViewPos().y) {
			bottomRightMinimap.y = minimap.getViewPos().y;
		}
		if (bottomRightMinimap.x > minimap.getViewPos().x + minimap.getViewLen().x) {
			bottomRightMinimap.x = minimap.getViewPos().x + minimap.getViewLen().x;
		}
		if (bottomRightMinimap.y > minimap.getViewPos().y + minimap.getViewLen().y) {
			bottomRightMinimap.y = minimap.getViewPos().y + minimap.getViewLen().y;
		}
		Line(topLeftMinimap, Point(bottomRightMinimap.x, topLeftMinimap.y), Color::RED, 1).draw(&fb);
		Line(topLeftMinimap, Point(topLeftMinimap.x, bottomRightMinimap.y), Color::RED, 1).draw(&fb);
		Line(Point(bottomRightMinimap.x, topLeftMinimap.y), bottomRightMinimap, Color::RED, 1).draw(&fb);
		Line(Point(topLeftMinimap.x, bottomRightMinimap.y), bottomRightMinimap, Color::RED, 1).draw(&fb);

		// update objek
		// Mouse
		Mouse::getAxis(&axis.x, &axis.y, &wheel);
		Mouse::getButton(&left, &middle, &right);

		// drag window
		if (Mouse::getLeftButton()) {
			Cursor::DRAG.draw(&fb, axis);
			// convert movement in the view to window
			Point d = prevaxis - axis;
			d.x *= canvas.getWindowLen().x;
			d.y *= canvas.getWindowLen().y;
			d.x /= canvas.getViewLen().x;
			d.y /= canvas.getViewLen().y;
			canvas.moveWindow(d);

			// focus to plane/ship if it was clicked
			if (prevleft == 0) {
				if (ship.collide(canvas.viewToWindow(axis))) {
					focusToShip = true;
					focusToPlane = false;
				} else if (plane.collide(canvas.viewToWindow(axis))) {
					focusToShip = false;
					focusToPlane = true;
				} else {
					focusToShip = false;
					focusToPlane = false;
				}
			}
		}
		else if (Mouse::getRightButton()) {
			Cursor::DRAG.draw(&fb, axis);
			if (minimap.getViewPos().x <= axis.x && axis.x <= minimap.getViewPos().x + minimap.getViewLen().x
				&& minimap.getViewPos().y <= axis.y && axis.y <= minimap.getViewPos().y + minimap.getViewLen().y) {
				Point d = axis - prevaxis;
				minimap.moveView(d);
			}
		}
		else {
			Cursor::POINTER.draw(&fb, axis);
			float scale = 1;
			// decrease/increase by 48:27 per wheel
			scale = (float)(canvas.getWindowLen().x + 48 * (prevwheel - wheel)) / (float)(canvas.getWindowLen().x);
			if ((int)(canvas.getWindowLen().x * scale) > 80) {
				canvas.scaleWindow(scale, canvas.viewToWindow(axis) - canvas.getWindowPos());
			}
		}

		if (focusToShip) {
			canvas.setWindowPos(canvas.viewToWindow(canvas.windowToView(ship.getPos()) - canvas.getViewPos() - Point(canvas.getViewLen().x / 2, canvas.getViewLen().y / 2)));
		} else if (focusToPlane) {
			canvas.setWindowPos(canvas.viewToWindow(canvas.windowToView(plane.getPos()) - canvas.getViewPos() - Point(canvas.getViewLen().x / 2, canvas.getViewLen().y / 2)));
		}

		prevaxis = axis;
		prevwheel = wheel;
		prevleft = left;

		// ship
		// find the appropriate position
		shipT += period;
		float shipD = shipT * shipV;
		float shipTD = 0;
		float shipPR = 0;
		float shipAng = 0;
		int shipI = shipPath.size() - 2;
		for (int i = 0; i+1 < shipPath.size(); ++i) {
			float d = sqrt((shipPath[i].x - shipPath[i+1].x) * (shipPath[i].x - shipPath[i+1].x) + (shipPath[i].y - shipPath[i+1].y) * (shipPath[i].y - shipPath[i+1].y));
			if (shipTD + d >= shipD) {
				shipI = i;
				shipPR = (shipD - shipTD) / d;
				shipTD += d;
				shipAng = atan2(shipPath[i+1].y - shipPath[i].y, shipPath[i+1].x - shipPath[i].x) * 180.f / acos(-1.);
				break;
			}
			else {
				shipTD += d;
			}
		}
		ship = dummyShip;
		ship.setPos(Point(shipPath[shipI].x + (int)(shipPR * (shipPath[shipI+1].x - shipPath[shipI].x)),
			shipPath[shipI].y + (int)(shipPR * (shipPath[shipI+1].y - shipPath[shipI].y))));
		ship.rotate(shipAng - 90);
		// reverse if path already covered
		if (shipI == shipPath.size() - 2 && shipD + period * shipV >= shipTD){
			std::reverse(shipPath.points.begin(), shipPath.points.end());
			shipD = 0;
			shipT = 0;
		}

		// plane
		// find the appropriate position
		planeT += period;
		float planeD = planeT * planeV;
		float planeTD = 0;
		float planePR = 0;
		float planeAng = 0;
		int planeI = planePath.size() - 2;
		for (int i = 0; i+1 < planePath.size(); ++i) {
			float d = sqrt((planePath[i].x - planePath[i+1].x) * (planePath[i].x - planePath[i+1].x) + (planePath[i].y - planePath[i+1].y) * (planePath[i].y - planePath[i+1].y));
			if (planeTD + d >= planeD) {
				planeI = i;
				planePR = (planeD - planeTD) / d;
				planeTD += d;
				planeAng = atan2(planePath[i+1].y - planePath[i].y, planePath[i+1].x - planePath[i].x) * 180.f / acos(-1.);
				break;
			}
			else {
				planeTD += d;
			}
		}
		plane = dummyPlane;
		plane.setPos(Point(planePath[planeI].x + (int)(planePR * (planePath[planeI+1].x - planePath[planeI].x)),
			planePath[planeI].y + (int)(planePR * (planePath[planeI+1].y - planePath[planeI].y))));
		plane.rotate(planeAng -180);
		// reverse if path already covered
		if (planeI == planePath.size() - 2 && planeD + period * planeV >= planeTD){
			std::reverse(planePath.points.begin(), planePath.points.end());
			planeD = 0;
			planeT = 0;
		}

		// delay sesuai framerate
		float now_render = clock();
		usleep(std::max(0., 1e6 / framerate - (now_render - last_render)));
		last_render = now_render;
	}

	// for (int i = 0; i < 16; ++i) {
	// 	for (int j = 0; j < 16; ++j)
	// 	{
	// 		if (Cursor::pointer[i][j] != Color::BLACK) {
	// 			fb.set(i + 100, j + 100, Cursor::pointer[i][j]);
	// 		}
	// 	}
	// }

	return 0;
}
