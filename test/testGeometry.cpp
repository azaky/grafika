#include "../lib/framebuffer.h"
#include "../lib/geometry.h"
#include "../lib/polygon.h"
#include "../lib/mouse.h"
#include "../lib/cursor.h"
#include "../lib/layer.h"
#include <time.h>

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
			(*prismSide)->borderColor = borderColor;
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

	Polygon p;
	p.add(Point(100, 100));
	p.add(Point(200, 100));
	p.add(Point(150, 200));
	p.add(Point(250, 150));
	p.add(Point(400, 250));
	p.add(Point(250, 300));
	p.add(Point(100, 250));
	Prism prism(p, 20);

	Polygon q(Color::EMPTY, Color::RED);
	q.add(Point(200, 435));
	q.add(Point(200, 505));
	q.add(Point(325, 505));
	q.add(Point(300, 470));
	q.add(Point(350, 435));
	Prism qrism(q, 200);

	std::vector<Polygon> polygons;

#define INDONESIAN_MAP_FILE "../object/plane"

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
		Prism prism(*polygon, 20);
		prism.addToLayer(layer);
	}

	bool isInside = false;

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
		if (iframe++ % 5 == 0) {
			system("clear");
			printf("Axis  : x = %f, y = %f, wheel = %d\n", axis.x, axis.y, wheel);
			printf("isInsidePolygon = %s\n", isInside ? "true" : "false");
		}

		// Draw Polygon
		// p.draw(&fb);
		// q.draw(&fb);
		layer.draw(&fb, Point(200, 200));
		isInside = Geometry::isPointInsidePolygon(axis, p);

		// Mouse
		axis.x = Mouse::getXAxis();
		axis.y = Mouse::getYAxis();
		wheel = Mouse::getWheelAxis();
		Mouse::getButton(&left, &middle, &right);
		Cursor::DEFAULT.draw(&fb, axis);

		// puter
		polygons[0].rotate(-1, (polygons[0].getTopLeft() + polygons[0].getBottomRight()) / 2.);
		layer = Layer();
		Prism prism(polygons[0], 80);
		prism.addToLayer(layer);

		// delay sesuai framerate
		float now_render = clock();
		usleep(std::max(0., 1e6 / framerate - (now_render - last_render)));
		last_render = now_render;
	}
	
	return 0;
}