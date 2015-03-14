#include "polygon.h"
#include "sprite.h"
#include <stdio.h>
#include <vector>
#include "framebuffer.h"

#define INSIDE 0 // 0000
#define LEFT 1   // 0001
#define RIGHT 2  // 0010
#define BOTTOM 4 // 0100
#define TOP 8    // 1000
 
class Canvas {
private:
	Point windowPos, windowLen; // window in canvas
	Point viewPos, viewLen;

	std::vector<Polygon> polygons; // the number of polygons in canvas
	std::vector<Sprite*> sprites; // list of sprite in the canvas
	Color defaultColor, defaultBorderColor;
	int defaultThickness;

	typedef int OutCode;
	
	// Compute the bit code for a point (x, y) using the clip rectangle
	// bounded diagonally by (xmin, ymin), and (xmax, ymax)
	 
	// ASSUME THAT xmax, xmin, ymax and ymin are global constants.
 
	OutCode ComputeOutCode(Point p) {
		OutCode code;
		code = INSIDE;          // initialised as being inside of clip window
		if (p.x < windowPos.x) {          // to the left of clip window
			code |= LEFT;
		}
		else if (p.x > windowPos.x+windowLen.x)      // to the right of clip window
			code |= RIGHT;
		if (p.y < windowPos.y) {           // below the clip window
			code |= BOTTOM;
		}
		else if (p.y > windowPos.y+windowLen.y)      // above the clip window
			code |= TOP;
		return code;
	}

// ABRL
	// 1001 1000 1010
	// 0001 0000 0010
	// 0101 0100 0110
	
public:
	Canvas(Point _windowLen, Point _viewLen, Color _color = Color::BLACK, Color _borderColor = Color::WHITE, int _thickness = 1) {
		defaultColor = _color;
		defaultBorderColor = _borderColor;
		defaultThickness = _thickness;

		windowLen = _windowLen;
		viewLen = _viewLen;
	}

	void addSprite(Sprite *sprite) {
		sprites.push_back(sprite);
	}

	void addPolygonFile(char* filePath) {
		FILE *file = fopen(filePath, "r");

		int nPolygon, nPolyline;
		fscanf(file, "%d %d", &nPolygon, &nPolyline);

		while (nPolygon--) {
			
			int nPoint;
			fscanf(file, "%d", &nPoint);
			Polygon polygon(defaultColor, Color::GREEN, 0);
			while (nPoint--) {
				Point point;
				fscanf(file, "%d %d", &point.x, &point.y);

				polygon.add(point);
			}
			polygons.push_back(polygon);
		}

		fclose(file);
	}

	void moveWindow(const Point& p) {
		windowPos += p;
	}

	Point getWindowPos() {
		return windowPos;
	}

	void setWindowPos(Point _windowPos) {
		windowPos = _windowPos;
	}

	void moveView(const Point&p) {
		viewPos += p;
	}

	Point getViewPos() {
		return viewPos;
	}

	void setViewPos(Point _viewPos) {
		viewPos = _viewPos;
	}

	void scaleView(float scale, Point offset = Point()) {
		viewLen.x *= scale;
		viewLen.y *= scale;

		viewPos.resize(scale, offset + viewPos);
	}

	void scaleWindow(float scale, Point offset = Point()) {
		windowLen.x *= scale;
		windowLen.y *= scale;

		windowPos.resize(scale, offset + windowPos);
	}

	void setWindowLen(Point _windowLen) {
		windowLen = _windowLen;
	}

	void setViewLen(Point _viewLen) {
		viewLen = _viewLen;
	}

	Point getViewLen() {
		return viewLen;
	}

	Point getWindowLen() {
		return windowLen;
	}

	Point windowToView(const Point &p) {
		Point ret = p;
		ret -= windowPos;
		ret.x *= viewLen.x;
		ret.y *= viewLen.y;
		ret.x /= windowLen.x;
		ret.y /= windowLen.y;
		ret += viewPos;
		return ret;
	}

	Point viewToWindow(const Point &p) {
		Point ret = p;
		ret -= viewPos;
		ret.x *= windowLen.x;
		ret.y *= windowLen.y;
		ret.x /= viewLen.x;
		ret.y /= viewLen.y;
		ret += windowPos;
		return ret;
	}

	void drawBoundary(Frame* fb, Color color = Color::WHITE, int thickness = 0) {
		Line(viewPos, Point(viewPos.x, viewPos.y + viewLen.y), color, thickness).draw(fb);
		Line(viewPos, Point(viewPos.x + viewLen.x, viewPos.y), color, thickness).draw(fb);
		Line(Point(viewPos.x, viewPos.y + viewLen.y), viewPos + viewLen, color, thickness).draw(fb);
		Line(Point(viewPos.x + viewLen.x, viewPos.y), viewPos + viewLen, color, thickness).draw(fb);
	}

	void draw(Frame* fb, Point offset = Point()) {
		// do the clipping
		for (int i = 0; i < sprites.size(); ++i) {
			std::vector<Line> lines = sprites[i]->getLines();
			for (int j = 0; j < lines.size(); ++j) {
				Line newEndpoints = lines[j];

				#define isValid(e) (ComputeOutCode(e.p1)&ComputeOutCode(e.p2)) == INSIDE
				#define isFinish(e) ((ComputeOutCode(e.p1) == INSIDE) and (ComputeOutCode(e.p2) == INSIDE))
				
				while ((isValid(newEndpoints)) and (not isFinish(newEndpoints))) {
					OutCode outCode0 = ComputeOutCode(newEndpoints.p1), outCode1 = ComputeOutCode(newEndpoints.p2), outCode;
					Point p;
					if (outCode0 != INSIDE) {
						outCode = outCode0;
						p = newEndpoints.p1;
					}
					else {
						outCode = outCode1;
						p = newEndpoints.p2;
					}

					int dy = newEndpoints.p1.y-newEndpoints.p2.y;
					int dx = newEndpoints.p1.x-newEndpoints.p2.x;
					int c = p.x*dy-p.y*dx;
					if (outCode & TOP) {           // point is above the clip rectangle
						p.y = (windowPos.y+windowLen.y);
						p.x = (c+p.y*dx)/dy;
					} else if (outCode & BOTTOM) { // point is below the clip rectangle
						p.y = windowPos.y;
						p.x = (c+p.y*dx)/dy;
					} else if (outCode & RIGHT) {  // point is to the right of clip rectangle
						p.x = (windowPos.x+windowLen.x);
						p.y = (p.x*dy-c)/dx;
					} else if (outCode & LEFT) {   // point is to the left of clip rectangle
						p.x = windowPos.x;
						p.y = (p.x*dy-c)/dx;
					}

					if (outCode == outCode0) {
						newEndpoints.p1 = p;
					}
					else
						newEndpoints.p2 = p;
				}
				if (isValid(newEndpoints)) {
					// adjust the window to view ratio
					newEndpoints.p1 = windowToView(newEndpoints.p1);
					newEndpoints.p2 = windowToView(newEndpoints.p2);
					
					// draw to the view
					// newEndpoints.color = Color::GREEN;
					newEndpoints.draw(fb, offset);
				}
			}
		}
		// for (std::vector<Polygon>::iterator polygon = polygons.begin(); polygon != polygons.end(); ++polygon) {
		// 	std::vector<std::pair<Point, Point> > lines = polygon->getLines();
		// 	for (std::vector<std::pair<Point, Point> >::iterator endpoints = lines.begin(); endpoints != lines.end(); ++endpoints) {
		// 		std::pair<Point, Point> newEndpoints = *endpoints;
				
		// 		#define isValid(e) (ComputeOutCode(e.first)&ComputeOutCode(e.second)) == INSIDE
		// 		#define isFinish(e) ((ComputeOutCode(e.first) == INSIDE) and (ComputeOutCode(e.second) == INSIDE))
				
		// 		while ((isValid(newEndpoints)) and (not isFinish(newEndpoints))) {
		// 			OutCode outCode0 = ComputeOutCode(newEndpoints.first), outCode1 = ComputeOutCode(newEndpoints.second), outCode;
		// 			Point p;
		// 			if (outCode0 != INSIDE) {
		// 				outCode = outCode0;
		// 				p = newEndpoints.first;
		// 			}
		// 			else {
		// 				outCode = outCode1;
		// 				p = newEndpoints.second;
		// 			}

		// 			int dy = newEndpoints.first.y-newEndpoints.second.y;
		// 			int dx = newEndpoints.first.x-newEndpoints.second.x;
		// 			int c = p.x*dy-p.y*dx;
		// 			if (outCode & TOP) {           // point is above the clip rectangle
		// 				p.y = (windowPos.y+windowLen.y);
		// 				p.x = (c+p.y*dx)/dy;
		// 			} else if (outCode & BOTTOM) { // point is below the clip rectangle
		// 				p.y = windowPos.y;
		// 				p.x = (c+p.y*dx)/dy;
		// 			} else if (outCode & RIGHT) {  // point is to the right of clip rectangle
		// 				p.x = (windowPos.x+windowLen.x);
		// 				p.y = (p.x*dy-c)/dx;
		// 			} else if (outCode & LEFT) {   // point is to the left of clip rectangle
		// 				p.x = windowPos.x;
		// 				p.y = (p.x*dy-c)/dx;
		// 			}

		// 			if (outCode == outCode0) {
		// 				newEndpoints.first = p;
		// 			}
		// 			else
		// 				newEndpoints.second = p;
		// 		}
		// 		if (isValid(newEndpoints)) {
		// 			// adjust the window to view ratio
		// 			newEndpoints.first = windowToView(newEndpoints.first);
		// 			newEndpoints.second = windowToView(newEndpoints.second);
					
		// 			// draw to the view
		// 			Line(newEndpoints.first, newEndpoints.second, Color::GREEN).draw(fb, offset);
		// 		}
		// 	}
		// }


	}
};