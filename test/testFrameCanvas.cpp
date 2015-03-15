#include "../lib/framecanvas.h"
#include "../lib/mouse.h"
#include "../lib/cursor.h"
#include "../lib/layer.h"
#include <ctime>

class Prism {
public:
	Prism(Polygon base, float height) {
		this->base = base;
		this->height = height;
		this->color = base.color;
		this->borderColor = base.borderColor;
	}

	void addToLayer(Layer& l) {
		std::vector<Polygon *> prismSides;
		for (int it = 0; it < base.size(); ++it) {
			Polygon* prismSide = new Polygon();
			prismSide->add(base[it]);
			prismSide->add(base[(it+1)%base.size()]);
			prismSide->add(base[(it+1)%base.size()]+Point(0, -height));
			prismSide->add(base[it]+Point(0, -height));
			prismSides.push_back(prismSide);
		}

		// topological sort
		// std::vector<int> s;
		// std::vector<int> result;
		// std::vector<int> backEdge;
		// std::vector<bool> visited;
		// visited.resize(prismSides.size());
		// for (int i = 0; i < prismSides.size(); ++i) {
		// 	int back = 0;
		// 	for (int j = 0; j < prismSides.size(); ++j) {
		// 		if (zIndexComparator(prismSides[j], prismSides[i])) {
		// 			back++;
		// 		}
		// 	}
		// 	backEdge.push_back(back);
		// 	if (back == 0) {
		// 		s.push_back(i);
		// 	}
		// }
		// while (result.size() < prismSides.size()) {
		// 	int now = s.back(); s.pop_back();
		// 	visited[now] = true;
		// 	result.push_back(now);
		// 	for (int i = 0; i < prismSides.size(); ++i) {
		// 		if (zIndexComparator(prismSides[now], prismSides[i])) {
		// 			backEdge[i]--;
		// 			if (backEdge[i] == 0 && !visited[i]) {
		// 				s.push_back(i);
		// 			}
		// 		}
		// 	}
		// }
		// for (int i = 0; i < result.size(); ++i) {
		// 	prismSides[result[i]]->color = color;
		// 	prismSides[result[i]]->borderColor = borderColor;
		// 	l.add(prismSides[result[i]]);
		// }

		// for (int i = 0; i < prismSides.size(); ++i) {
		// 	int chosen = i;
		// 	for (int j = i + 1; j < prismSides.size(); ++j) {
		// 		if (zIndexComparator(prismSides[j], prismSides[chosen])) {
		// 			chosen = j;
		// 		}
		// 	}
		// 	std::swap(prismSides[chosen], prismSides[i]);
		// }
		std::sort(prismSides.begin(), prismSides.end(), zIndexComparator);
		// FILE *f = fopen("temp.out", "w");
		// int z = 0;
		// for (std::vector<Polygon*>::iterator prismSide = prismSides.begin(); prismSide != prismSides.end(); ++prismSide, ++z) {
		// 	fprintf(f, "layer %d = (%f, %f) .. (%f, %f)\n", z, (*(*prismSide))[0].x, (*(*prismSide))[0].y, (*(*prismSide))[1].x, (*(*prismSide))[1].y);
		// }
		// fclose(f);

		for (std::vector<Polygon*>::iterator prismSide = prismSides.begin(); prismSide != prismSides.end(); ++prismSide) {
			(*prismSide)->color = color;
			(*prismSide)->borderColor = Color::CYAN;
			l.add(*prismSide);
		}

		Polygon* cover = new Polygon(base);
		for (std::vector<Point>::iterator point = cover->begin(); point != cover->end(); ++point) {
			(*point) += Point(0, -height);
		}
		l.add(cover);
	}

private:
	static bool zIndexComparator(const Polygon* lhs, const Polygon* rhs) {
		float p0 = (*lhs)[0].x, p1 = (*lhs)[1].x;
		float q0 = (*rhs)[0].x, q1 = (*rhs)[1].x;
		if (p1 < p0) {
			std::swap(p0, p1);
		}
		if (q1 < q0) {
			std::swap(q0, q1);
		}
		float l = std::max(p0, q0);
		float r = std::min(p1, q1);

		if (l > r) {
			// no intersection
			return false;
			// return std::max((*lhs)[1].y, (*lhs)[0].y) < std::max((*rhs)[1].y, (*rhs)[0].y);
		}
		else {
			float py, qy;
			if (p0 == p1) {
				py = (*lhs)[0].y;
			} else {
				py = ((*lhs)[1].y - (*lhs)[0].y) * (l - p0) / (p1 - p0) + (*lhs)[0].y;
			}
			if (q0 == q1) {
				qy = (*rhs)[0].y;
			} else {
				qy = ((*rhs)[1].y - (*rhs)[0].y) * (l - q0) / (q1 - q0) + (*rhs)[0].y;
			}
			return py < qy;
		}
	}

	Polygon base;
	float height;
	Color color;
	Color borderColor;
};


int main() {
	FrameBuffer fb;

	// set up frame canvas
	FrameCanvas fc(1366, 786);
	fc.setViewPos(Point(100, 100));
	fc.scaleView(0.6);

	// set up theindonesia 3D polygon
	std::vector<Polygon> polygons;
#define INDONESIAN_MAP_FILE "../object/indonesia_10km"
	FILE *file = fopen(INDONESIAN_MAP_FILE, "r");
	if (file == NULL) return 0;
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

	// Misc Objects
	// Ship
	Sprite ship(Color::EMPTY, Color::RED);
	ship.add("../object/ship_above");
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

	// Plane
	Sprite plane(Color::EMPTY, Color::BLUE);
	plane.add("../object/plane_above");
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

	bool focusToPlane = false;
	bool focusToShip = false;

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

		// Draw Polygon
		// p.draw(&fb);
		// q.draw(&fb);
		// puts("halo");
		ship.draw(&fc);
		plane.draw(&fc);

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

		// puter
		// polygons[0].rotate(1, (polygons[0].getTopLeft() + polygons[0].getBottomRight()) / 2.);
		// layer = Layer();
		// Prism prism(polygons[0], 80);
		// prism.addToLayer(layer);

		// delay sesuai framerate
		float now_render = clock();
		// usleep(2e6);
		usleep(std::max(0., 1e6 / framerate - (now_render - last_render)));
		last_render = now_render;
	}

	return 0;
}
