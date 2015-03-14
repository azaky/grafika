#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "polygon.h"

#define sqr(i) (i*i)
const double PI = acos(-1);
const double EPS = 1e-9;
const double INF = 1e300;

class Geometry {
public:
	static int cross(const Point &p1, const Point &p2) {
		/* returns z-component of cross product of two points (vectors) */
		return p1.x * p2.y - p1.y * p2.x;
	}

	static int dot(const Point &p1, const Point &p2) {
		/* returns dot product of two points (vectors) */
		return p1.x * p2.x + p1.y * p2.y;
	}

	static int dist(const Point &p){
		/* returns distance of p from the origin */
		return sqrt(sqr(p.x) + sqr(p.y));
	}

	static int dist(const Point &p1, const Point &p2) {
		/* returns distance between two points */
		return sqrt(sqr(p1.x - p2.x) + sqr(p1.y - p2.y));
	}

	static int length(const Line &s) {
		/* returns length of a line */
		return sqrt(sqr(s.p1.x - s.p2.x) + sqr(s.p1.y - s.p2.y));
	}

	static double getAngle(const Point &p1, const Point &p2) {
		/* returns angle formed by two vectors. WARNING: undirected angle */
		return fabs(acos(dot(p1,p2) / dist(p1,Point(0,0)) / dist(p2,Point(0,0))));
	}

	static double getAngle(const Point &p1, const Point &center, const Point &p2) {
		/* returns angle formed by three points. WARNING: undirected angle */
		return getAngle(p1 - center, p2 - center);
	}

	static double distToSegment(const Point &p, const Line &s) {
		/* returns distance of a Point to a Line */
		if (getAngle(s.p2, s.p1, p) > PI/2 + EPS || getAngle(s.p1, s.p2, p) > PI/2 + EPS) return std::min(dist(p,s.p1), dist(p,s.p2));
		return fabs(cross(s.p1 - p, s.p2 - p)) / dist(s.p1, s.p2);
	}

	static double distToLine(const Point &p, const Line &s){
		/* returns distance of a Point to a line (its orthogonal projection) */
		return fabs(cross(s.p1 - p, s.p2 - p)) / dist(s.p1, s.p2);
	}

	static bool isRightTurn(const Point &p1, const Point &p2, const Point &p3){
		return cross(p2 - p1, p3 - p2) <= 0;
		/* straight returns true */
	}

	static bool isOnSameSide(const Point &p1, const Point &p2, const Line &s){
		double z1 = cross(s.p2 - s.p1, p1 - s.p1);
		double z2 = cross(s.p2 - s.p1, p2 - s.p1);
		return (z1 + EPS < 0 && z2 + EPS < 0) || (0 < z1 - EPS && 0 < z2 - EPS) || fabs(z1) < EPS || fabs(z2) < EPS;
		/* on Line returns true */
	}

	// NANYA ZAKY TTG INI
	static bool isOnLine(const Point &p, const Line &l){
		return fabs((l.p1.y - p.y) * (l.p2.x - p.x) - (l.p2.y - p.y) * (l.p1.x - p.x)) < EPS;
	}

	static bool isOnSegment(const Point &p, const Line &s){
		return fabs(dist(p, s.p1) + dist(p, s.p2) - dist(s.p1, s.p2)) < EPS;
	}

	static bool isIntersecting(const Line &s1, const Line &s2){
		return !(isOnSameSide(s1.p1,s1.p2,s2) || isOnSameSide(s2.p1,s2.p2,s1)) || isOnLine(s1.p1,s2) || isOnLine(s1.p2,s2) || isOnLine(s2.p1,s1) || isOnLine(s2.p2,s1);
	}

	static bool isParallel(const Line &s1, const Line &s2) {
		return fabs((s1.p1.y-s1.p2.y)*(s2.p1.x-s2.p2.x)-(s2.p1.y-s2.p2.y)*(s1.p1.x-s1.p2.x)) < EPS;
	}

	static Point intersection(const Line &s1, const Line &s2){
		/* assumes !isParallel(s1,s2) */
		double x1 = s1.p1.x - s1.p2.x;
		double x2 = s2.p1.x - s2.p2.x;
		double y1 = s1.p1.y - s1.p2.y;
		double y2 = s2.p1.y - s2.p2.y;
		double cross1 = cross(s1.p1, s1.p2);
		double cross2 = cross(s2.p1, s2.p2);
		return Point ((cross1 * x2 - cross2 * x1) / (x1 * y2 - x2 * y1), (cross1 * y2 - cross2 * y1) / (x1 * y2 - x2 * y1));
	}
	static Point projection(const Point &p, const Line &s){
		/* projects p onto line s */
		Point newPoint = s.p2-s.p1;
		newPoint.resize(dot(p - s.p1, s.p2 - s.p1) / sqr(length(s)));
		return newPoint + s.p1;
	}
	static Point reflection(const Point &p, const Point &center){
		/* reflect p with respect to center */
		Point newPoint = center;
		newPoint.resize(2.);

		return newPoint-p;
	}
	static Point reflection(const Point &p, const Line &s){
		/* reflect p with respect to line s */
		return reflection(p, projection(p, s));
	}
	static Line lineBisector(const Point &p1, const Point &p2){
		Point center = (p1 + p2);
		center.resize(2.);

		Point newP1 = center;
		newP1.rotate(180);

		Point newP2 = center;
		newP2.rotate(180);
		return Line(newP1, newP2);
	}

	static Line lineBisector(const Line &s){
		return lineBisector(s.p1, s.p2);
	}

	/** Check position of a point with respect to a polygon  **/
	/* complexity : O(N) */
	static bool isPointInsidePolygon(Point p, Polygon poly){
		/* ray casting to the right */
		Line ray (p,p+Point(1,0));
		int n = (int)poly.size();
		/* counts the number of intersections */
		int nIntersection = 0;
		for (int i = 0; i < n; ++i){
			Line side(poly[i],poly[(i+1)%n]);
			if (isOnSegment(p,side)) return false;
			if (isParallel(ray,side)) continue;
			Point x = intersection(ray,side);
			if (isOnSegment(x,side) && dot(x-p,ray.p2-p) > 0){
				/* special case: x is one of vertices of sides */
				if (x == side.p1){
					if (isRightTurn(p,x,side.p2)) nIntersection ++;
				}
				else if (x == side.p2){
					if (isRightTurn(p,x,side.p1)) nIntersection ++;
				}
				else nIntersection ++;
			}
		}
		return nIntersection % 2 == 1;
	}

	/** Convex Hull | monotone chain algorithm **/
	/* complexity : O(N log N) */
	static Polygon convexHull(Polygon p){
		int m = 0, n = p.size();
		Polygon hull;
		sort(p.begin(),p.end());
		for (int i = 0; i < n; ++i){
			while (m >= 2 && isRightTurn(hull[m-2],hull[m-1],p[i])) --m;
			if (m > hull.size())
				hull.add(p[i]);
			else
				hull[m++] = p[i];
		}
		for (int i = n-1, t = m+1; i >= 0; --i){
			while (m >= t && isRightTurn(hull[m-2],hull[m-1],p[i])) --m;
			if (m > hull.size())
				hull.add(p[i]);
			else
				hull[m++] = p[i];
		}
		hull.resize(m);
		return hull;
	}
};

#endif