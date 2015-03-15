#ifndef SPRITE_H
#define SPRITE_H

#include "polygon.h"
#include "polyline.h"
#include <stdio.h>
#include <set>

class Sprite : public Drawable {
public:
	Sprite(Color _color = Color::BLACK, Color _borderColor = Color::WHITE, int _thickness = 1) {
		defaultColor = _color;
		defaultBorderColor = _borderColor;
		defaultThickness = _thickness;

		topLeft.x = 1<<31-1, bottomRight.x = 1<<31;
		topLeft.y = 1<<31-1, bottomRight.y = 1<<31;
	}

	void add(const char *path) {
		std::set<Point> points;
		FILE *file = fopen(path, "r");

		if (file == NULL) return;

		int nPolygon, nPolyline;
		fscanf(file, "%d %d", &nPolygon, &nPolyline);

		while (nPolygon--) {
			
			int nPoint;
			fscanf(file, "%d", &nPoint);
			Polygon polygon(defaultColor, defaultBorderColor, defaultThickness);
			while (nPoint--) {
				Point point;
				fscanf(file, "%d %d", &point.x, &point.y);

				polygon.add(point);
			}
			polygons.push_back(polygon);
		}

		while (nPolyline--){
			int nPoint;
			fscanf(file, "%d", &nPoint);
			Polyline polyline;
			while (nPoint--) {
				Point point;
				fscanf(file, "%d %d", &point.x, &point.y);
				polyline.add(point);
			}
			polylines.push_back(polyline);
		}

		fclose(file);

		calcBoundingBox();
	}
	void normalize() {
		Point center = getCenter();
		for (std::vector<Polygon>::iterator polygon = polygons.begin(); polygon != polygons.end(); ++polygon) {
			for (int i = 0; i < polygon->size(); ++i) {
				(*polygon)[i] -= center;
			}
		}
		for (std::vector<Polyline>::iterator polyline = polylines.begin(); polyline != polylines.end(); ++polyline) {
			for (int i = 0; i < polyline->size(); ++i) {
				(*polyline)[i] -= center;
			}
		}
	}

	void draw(Frame* fb, Point offset = Point()) {
		for (std::vector<Polygon>::iterator polygon = polygons.begin(); polygon != polygons.end(); ++polygon) {
			polygon->draw(fb, pos+offset);
		}
		for (std::vector<Polyline>::iterator polyline = polylines.begin(); polyline != polylines.end(); ++polyline) {
			polyline->draw(fb, pos+offset);
		}
	}

	void drawFill(Frame *fb, Frame *fill, Point offset = Point()) {
		for (std::vector<Polygon>::iterator polygon = polygons.begin(); polygon != polygons.end(); ++polygon) {
			polygon->drawFill(fb, fill, offset);
		}
	}

	void resize(float size, Point center = Point()) {
		for (int i = 0; i < polygons.size(); ++i)
			polygons[i].resize(size, center);
		for (int i = 0; i < polylines.size(); ++i)
			polylines[i].resize(size, center);
		calcBoundingBox();
	}

	void rotate(const int& degree, const Point& offset = Point(0, 0)) {
		for (std::vector<Polygon>::iterator polygon = polygons.begin(); polygon != polygons.end(); ++polygon) {
			polygon->rotate(degree, offset);
		}
		for (std::vector<Polyline>::iterator polyline = polylines.begin(); polyline != polylines.end(); ++polyline) {
			polyline->rotate(degree, offset);
		}
		calcBoundingBox();
	}

	void calcBoundingBox() {
		topLeft.x = 1<<30, bottomRight.x = -(1<<30);
		topLeft.y = 1<<30, bottomRight.y = -(1<<30);
		for (std::vector<Polygon>::iterator polygon = polygons.begin(); polygon != polygons.end(); ++polygon) {
			for (int i = 0; i < polygon->size(); ++i) {
				Point &point = (*polygon)[i];
				topLeft.x = std::min(topLeft.x, point.x); topLeft.y = std::min(topLeft.y, point.y);
				bottomRight.x = std::max(bottomRight.x, point.x); bottomRight.y = std::max(bottomRight.y, point.y);
			}
		}
		for (std::vector<Polyline>::iterator polyline = polylines.begin(); polyline != polylines.end(); ++polyline) {
			for (int i = 0; i < polyline->size(); ++i) {
				Point &point = (*polyline)[i];
				topLeft.x = std::min(topLeft.x, point.x); topLeft.y = std::min(topLeft.y, point.y);
				bottomRight.x = std::max(bottomRight.x, point.x); bottomRight.y = std::max(bottomRight.y, point.y);
			}
		}
	}

	void print() {
		for(std::vector<Polygon>::iterator polygon = polygons.begin(); polygon != polygons.end(); ++polygon) {
			polygon->print();
		}
	}

	void disapear(){
		defaultColor = Color::EMPTY;
		defaultBorderColor = Color::EMPTY;
	}

	void setPos(Point _pos) {
		pos = _pos;
	}

	void update(float t) {
		// gerakin objek
		v.x += (int)(t * a.x);
		v.y += (int)(t * a.y);

		pos.x += (int)(t * v.x);
		pos.y += (int)(t * v.y);
	}

	void setV(Point _v) {
		v = _v;
	}

	Point getV() {
		return v;
	}

	void setA(Point _a) {
		a = _a;
	}

	Point getA() {
		return a;
	}

	Point& getPos() {
		return pos;
	}

	Point getTopLeft() {
		return topLeft;
	}

	Point getBottomRight() {
		return bottomRight;
	}

	Point getCenter() {
		return Point((topLeft.x + bottomRight.x) / 2, (topLeft.y + bottomRight.y) / 2);
	}

	Point getSize() {
		return bottomRight - topLeft;
	}

	bool collide(Point p) {
		return pos.x + topLeft.x <= p.x && p.x <= pos.x + bottomRight.x
			&& pos.y + topLeft.y <= p.y && p.y <= pos.y + bottomRight.y;
	}

	std::vector<Line> getLines() {
		std::vector<Line> lines;
		for(std::vector<Polygon>::iterator polygon = polygons.begin(); polygon != polygons.end(); ++polygon) {
			for (int i = 1; i < polygon->size(); ++i) {
				lines.push_back(Line((*polygon)[i-1], (*polygon)[i], polygon->borderColor, polygon->thickness));
			}
			if (polygon->size()) {
				lines.push_back(Line((*polygon)[polygon->size()-1], (*polygon)[0], polygon->borderColor, polygon->thickness));
			}
		}
		for(std::vector<Polyline>::iterator polyline = polylines.begin(); polyline != polylines.end(); ++polyline) {
			for (int i = 1; i < polyline->size(); ++i) {
				lines.push_back(Line((*polyline)[i-1], (*polyline)[i], polyline->color, polyline->thickness));
			}
		}
		for (int i = 0; i < lines.size(); ++i) {
			lines[i].p0 += pos;
			lines[i].p1 += pos;
		}
		return lines;
	}

	void drawBoundingBox(Frame* f, Color color = Color::WHITE) {
		topLeft += pos;
		bottomRight += pos;
		Line(topLeft, Point(topLeft.x, bottomRight.y), color).draw(f);
		Line(topLeft, Point(bottomRight.x, topLeft.y), color).draw(f);
		Line(Point(topLeft.x, bottomRight.y), bottomRight, color).draw(f);
		Line(Point(bottomRight.x, topLeft.y), bottomRight, color).draw(f);
		topLeft -= pos;
		bottomRight -= pos;
	}

	bool equal(Sprite sprite){
		return (this->pos.x == sprite.getPos().x &&
				this->pos.y == sprite.getPos().y &&
				this->v.x == sprite.getV().x &&
				this->v.y == sprite.getV().y &&
				this->topLeft.x == sprite.getTopLeft().x &&
				this->topLeft.y == sprite.getTopLeft().y &&
				this->bottomRight.x == sprite.getBottomRight().x &&
				this->bottomRight.y == sprite.getBottomRight().y);
	}

private:
	std::vector<Polygon> polygons;
	std::vector<Polyline> polylines;
	Color defaultColor;				// fill color of the polygon
	Color defaultBorderColor;			// line color of the polygon
	float defaultThickness;			// thicknes of the border line
	Point v;
	Point a;
	Point pos;

	Point topLeft, bottomRight;
};

#endif