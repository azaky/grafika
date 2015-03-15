#include "../lib/framecanvas.h"
#include "../lib/mouse.h"
#include "../lib/cursor.h"
#include "../lib/sprite.h"
#include "../lib/layer.h"
#include "../lib/prism.h"
#include <time.h>
#define INDONESIAN_MAP_FILE "object/indonesia_10km"

FrameCanvas fc(1366, 786);

void initializeMap() {
	// set up frame canvas
	fc.setViewPos(Point(100, 100));
	fc.scaleView(0.6);

	system("clear");
	printf("Loading 3d Map ...\n");
	// set up theindonesia 3D polygon
	std::vector<Polygon> polygons;
	FILE *file = fopen(INDONESIAN_MAP_FILE, "r");
	if (file == NULL) return;
	int nPolygon, nPolyline;
	fscanf(file, "%d %d", &nPolygon, &nPolyline);
	while (nPolygon--) {
		int nPoint;
		fscanf(file, "%d", &nPoint);
		Polygon polygon(Color::EMPTY, Color::GREEN);
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

	layer.draw(fc.getBgFrame(), Point(0, 100));
	fc.resetBackground();
}

bool Game() {

	// Misc Objects
	// Ship
	Sprite ship(shipcolor, Color::RED);
	ship.add("object/ship_above");
	ship.normalize();
	ship.resize(0.4);
	ship.setV(Point(0, 30));
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
	bool isShipExplode = false;

	// Bullet
	Sprite shipBullet(Color::EMPTY, Color::RED);
	shipBullet.add("object/bullet");
	shipBullet.normalize();
	float shipBulletV = 200;
	float shipBulletlifetime;
	Sprite dummyShipBullet(shipBullet);
	bool shipBulletexist = false;
	float shipBulletT = 0;
	
	Sprite planeBullet(Color::EMPTY, Color::RED);
	planeBullet.add("object/bullet");
	planeBullet.normalize();
	float planeBulletV = 200;
	float planeBulletlifetime;
	Sprite dummyPlaneBullet(planeBullet);
	bool planeBulletexist = false;
	float planeBulletT = 0;
	
	// Plane
	Sprite plane(planecolor, Color::BLUE);
	plane.add("object/plane_above");
	plane.normalize();
	plane.resize(0.13);
	plane.setV(Point(0, 30));
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
	bool isPlaneExplode = false;

	bool focusToPlane = false;
	bool focusToShip = false;

	Point axis, prevaxis;
	int wheel = 0, prevwheel = 0;
	int left, right, middle;
	int prevleft = 0, prevright = 0;

	float last_render = clock();
	
	int iframe = 0;

	while ((not isShipExplode) and (not isPlaneExplode)) {
		// Draw objects
		fb.clear();
		fc.clear();
		if (iframe++ % framerate == 0) {
			system("clear");
			printf("Axis  : x = %f, y = %f, wheel = %d\n", axis.x, axis.y, wheel);
			printf("Button: left = %d, middle = %d, right = %d\n", left, middle, right);
			printf("Canvas: viewPos = (%f, %f), viewLen = (%f, %f), winfowPos = (%f, %f), windowLen = (%f, %f)\n", fc.getViewPos().x, fc.getViewPos().y, fc.getViewLen().x, fc.getViewLen().y, fc.getWindowPos().x, fc.getWindowPos().y, fc.getWindowLen().x, fc.getWindowLen().y);
			printf("Focus : ship: %d, plane: %d\n", focusToShip, focusToPlane);
		}

		// draw canvas
		ship.draw(&fc);
		ship.drawFill(&fc, &shippattern);
		plane.draw(&fc);
		plane.drawFill(&fc, &planepattern);
		if (shipBulletexist)
			shipBullet.draw(&fc);

		if (planeBulletexist)
			planeBullet.draw(&fc);

		// canvas.draw(&fb);
		fc.draw(&fb);

		// update objek
		// Mouse
		axis.x = Mouse::getXAxis();
		axis.y = Mouse::getYAxis();
		wheel = Mouse::getWheelAxis();
		Mouse::getButton(&left, &middle, &right);

		// drag window
		if (Mouse::getLeftButton()) {
			Cursor::DRAG.draw(&fb, axis);

			// focus to plane/ship if it was clicked
			if (prevleft == 0) {
				if (ship.collide(fc.viewToWindow(axis))) {
					focusToShip = true;
					focusToPlane = false;
				} else if (plane.collide(fc.viewToWindow(axis))) {
					focusToShip = false;
					focusToPlane = true;
				} else {
					focusToShip = false;
					focusToPlane = false;
					if (!isShipExplode){
						dummyShipBullet.setPos(ship.getPos());
						Point dummyShipBulletV = fc.viewToWindow(axis) - ship.getPos();
						float length = sqrt(dummyShipBulletV.x * dummyShipBulletV.x + dummyShipBulletV.y * dummyShipBulletV.y);
						dummyShipBulletV.x = dummyShipBulletV.x * shipBulletV / length;
						dummyShipBulletV.y = dummyShipBulletV.y * shipBulletV / length;
						dummyShipBullet.setV(dummyShipBulletV);
						shipBulletlifetime = 5.0;
						shipBulletexist = true;
						shipBulletT = 0;
						// canvas.addSprite(&shipBullet);
						// minimap.addSprite(&shipBullet);
					}
				}
			}
		}
		else if (Mouse::getRightButton()) {
			Cursor::DRAG.draw(&fb, axis);

			// convert movement in the view to window
			Point d = prevaxis - axis;
			d.x *= fc.getWindowLen().x;
			d.y *= fc.getWindowLen().y;
			d.x /= fc.getViewLen().x;
			d.y /= fc.getViewLen().y;
			fc.moveWindow(d);
		}
		else {
			Cursor::POINTER.draw(&fb, axis);
			float scale = 1;
			// decrease/increase by 48:27 per wheel
			scale = (float)(fc.getWindowLen().x + 48 * (prevwheel - wheel)) / (float)(fc.getWindowLen().x);
			if ((int)(fc.getWindowLen().x * scale) > 80) {
				fc.scaleWindow(scale, fc.viewToWindow(axis) - fc.getWindowPos());
			}
		}

		// zoom in/out
		if (wheel > prevwheel) {
			fc.scaleWindow(0.9, fc.viewToWindow(axis) - fc.getWindowPos());
		} else if (wheel < prevwheel) {
			fc.scaleWindow(1.1, fc.viewToWindow(axis) - fc.getWindowPos());
		}

		if (focusToShip) {
			fc.setWindowPos(fc.viewToWindow(fc.windowToView(ship.getPos()) - fc.getViewPos() - Point(fc.getViewLen().x / 2, fc.getViewLen().y / 2)));
		} else if (focusToPlane) {
			fc.setWindowPos(fc.viewToWindow(fc.windowToView(plane.getPos()) - fc.getViewPos() - Point(fc.getViewLen().x / 2, fc.getViewLen().y / 2)));
		}

		prevaxis = axis;
		prevwheel = wheel;
		prevleft = left;
		prevright = right;

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

		// bullet
		if(shipBulletexist){
			dummyShipBullet.update(period);
			shipBullet = dummyShipBullet;
			float shipBulletAng = atan2(shipBullet.getV().y, shipBullet.getV().x) * 180.f / acos(-1);
			shipBullet.rotate(shipBulletAng);
			shipBulletlifetime -= period;
			if(plane.collide(shipBullet.getPos())){
				isPlaneExplode = true;
				shipBulletexist = false;
			}
		}
		
		if(shipBulletexist && shipBulletlifetime<0.0){
			shipBulletexist = false;
			// canvas.removeSprite(shipBullet);
			// minimap.removeSprite(shipBullet);
		}

		if(!planeBulletexist && !isPlaneExplode){
			dummyPlaneBullet.setPos(plane.getPos());
			Point dummyPlaneBulletV = ship.getPos() - plane.getPos();
			float lengthPB = sqrt(dummyPlaneBulletV.x * dummyPlaneBulletV.x + dummyPlaneBulletV.y * dummyPlaneBulletV.y);
			dummyPlaneBulletV.x = dummyPlaneBulletV.x * planeBulletV / lengthPB;
			dummyPlaneBulletV.y = dummyPlaneBulletV.y * planeBulletV / lengthPB;
			dummyPlaneBullet.setV(dummyPlaneBulletV);
			planeBulletlifetime = 5.0;
			planeBulletexist = true;
			planeBulletT = 0;
		}

		if(planeBulletexist){
			dummyPlaneBullet.update(period);
			planeBullet = dummyPlaneBullet;
			float planeBulletAng = atan2(planeBullet.getV().y, planeBullet.getV().x) * 180.f / acos(-1);
			planeBullet.rotate(planeBulletAng);
			planeBulletlifetime -= period;
			if(ship.collide(planeBullet.getPos())){
				isShipExplode = true;
				planeBulletexist = false;
			}
		}

		if(planeBulletexist && planeBulletlifetime<0.0){
			planeBulletexist = false;
			// canvas.removeSprite(planeBullet);
			// minimap.removeSprite(planeBullet);
		}

		// delay sesuai framerate
		float now_render = clock();
		usleep(std::max(0., 1e6 / framerate - (now_render - last_render)));
		last_render = now_render;
	}

	return isPlaneExplode;
}
