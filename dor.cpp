#include "lib/sprite.h"
#include <time.h>

int main(){
	FrameBuffer fb;

	Sprite plane(Color::EMPTY, Color::BLUE);
	plane.add("object/plane_above");
	plane.normalize();
	plane.resize(0.13);
	plane.setV(Point(0, 30));
	// plane.setPos(Point(100,100));
	
	
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
	bool isPlaneBulletExist = false;
	Line planeBullet(Point(0,0), Point(0,0),Color::WHITE);

	Sprite ship(Color::EMPTY, Color::RED);
	ship.add("object/ship_above");
	ship.normalize();
	ship.resize(0.4);
	ship.setV(Point(0, 30));
	// ship.setPos(Point(200,200));
	
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
	bool isShipBulletExist = false;
	Line shipBullet(Point(0,0), Point(0,0),Color::WHITE);

	int framerate = 15;
	float period = 1. / framerate;
	float last_render = clock();
	
	int iframe = 0;

	while(true){
		fb.clear();
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

		
		
		

		if (planeI == planePath.size() - 2 && planeD + period * planeV >= planeTD){
			std::reverse(planePath.points.begin(), planePath.points.end());
			planeD = 0;
			planeT = 0;
		}

		plane.draw(&fb);
		ship.draw(&fb);
		
		if(!isShipBulletExist){
			Point shipBullet0 = ship.getPos();
			Point shipBullet1 = Point(shipPath[shipI+1].x,shipPath[shipI+1].y);
			Point shipBullet2 = shipBullet.segmentation(shipBullet0, shipBullet1,15);
			shipBullet.p1 = shipBullet0;
			shipBullet.p2 = shipBullet2;
			shipBullet.draw(&fb);
			isShipBulletExist = true;	
		} else {
			Point temp = shipBullet.p2;
			int del_x = shipBullet.p2.x - shipBullet.p1.x;
			int del_y = shipBullet.p2.y - shipBullet.p1.y;
			int new_x = shipBullet.p2.x + del_x;
			int new_y = shipBullet.p2.y + del_y;
			shipBullet.p1 = shipBullet.p2;
			shipBullet.p2 = Point(new_x, new_y);
			shipBullet.draw(&fb);
		}
		
		if(!isPlaneBulletExist){
			Point planeBullet0 = plane.getPos();
			Point planeBullet1 = Point(planePath[planeI+1].x,planePath[planeI+1].y);
			Point planeBullet2 = planeBullet.segmentation(planeBullet0, planeBullet1,15);
			planeBullet.p1 = planeBullet0;
			planeBullet.p2 = planeBullet2;
			planeBullet.draw(&fb);
			isPlaneBulletExist = true;	
		} else {
			Point temp = planeBullet.p2;
			int del_x = planeBullet.p2.x - planeBullet.p1.x;
			int del_y = planeBullet.p2.y - planeBullet.p1.y;
			int new_x = planeBullet.p2.x + del_x;
			int new_y = planeBullet.p2.y + del_y;
			planeBullet.p1 = planeBullet.p2;
			planeBullet.p2 = Point(new_x, new_y);
			// Line sb2(shipBullet.p2, Point(new_x,new_y), Color::WHITE);
			// shipBullet.p1 = temp;
			// shipBullet.p2 = Point(new_x, new_y);
			planeBullet.draw(&fb);
		}
		

		float now_render = clock();
		usleep(std::max(0., 1e6 / framerate - (now_render - last_render)));
		last_render = now_render;
	}

	// plane.draw(&fb);
	// ship.draw(&fb);
	// planeBullet.draw(&fb);
	// shipBullet.draw(&fb);
	return 0;
}