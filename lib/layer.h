#ifndef LAYER_H
#define LAYER_H

#include "polygon.h"
#include "sprite.h"
#include <stdio.h>
#include <vector>
#include "framebuffer.h"
#include "geometry.h"
using namespace Geometry;

#define INSIDE  0 // 0000
#define LEFT    1 // 0001
#define RIGHT   2 // 0010
#define BOTTOM  4 // 0100
#define TOP     8 // 1000

class Layer : public Drawable {
public:
    Layer() {}
    ~Layer() {
        for (int i = 0; i < polygons.size(); ++i) {
            delete polygons[i];
        }
    }

    void add(Polygon* s) {
        polygons.push_back(s);
    }

    void draw(Frame *fb, Point offset = Point()) {
        // for each lines sorted by their z-index
        //     for each polygon whose z-index is > line's z-index
        //         cutAndDrawLine(l, z+1, fb, offset)
        int z = 0;
        for (std::vector<Polygon*>::iterator polygon = polygons.begin(); polygon != polygons.end(); polygon++, z++) {
            (*polygon)->drawFill(fb, NULL, offset);
            (*polygon)->draw(fb, offset);
            // std::vector<Line> lines = (*polygon)->getLines();
            // for (std::vector<Line>::iterator line = lines.begin(); line != lines.end(); line++) {
            //     zz = z;
            //     // cutAndDrawLine(*line, polygons.size() + 1, fb, offset);
            //     cutAndDrawLine(*line, z + 1, fb, offset);
            // }
        }
    }

private:
    int zz;
    void cutAndDrawLine(Line l, int z, Frame *fb, Point offset){
        if (z < polygons.size()) {
            // cut this line -> lines
            // foreach newLine in lines > cutAndDrawLine(newLine, z+1, fb, offset)

            // cut l with polygons[z]
            Polygon& p = *polygons[z];

            // find the intersection of current segment with each side
            std::vector<Point> intersections;
            intersections.push_back(l.p1);
            intersections.push_back(l.p2);
            for (int i = 0; i < p.size(); ++i) {
                Line side(p[i], p[(i + 1) % p.size()]);
                if (isParallel(side, l)) {
                    continue;
                }
                Point temp = intersection(side, l);
                if (isIntersecting(side, l)) {
                // if (isOnSegment(temp, side) && isOnSegment(temp, l)) {
                    intersections.push_back(temp);
                    // if (z == polygons.size() - 1) {
                    //     Circle(temp.x, temp.y, 3, Color::RED).drawSolid(fb, Point(200, 200));
                    // }
                }
            }

            // sort them and remove duplicates
            std::sort(intersections.begin(), intersections.end());
            // std::reverse(intersections.begin(), intersections.end());
            intersections.resize(std::unique(intersections.begin(), intersections.end()) - intersections.begin());

            // recursively cut the new calculated line(s)
            int isInside = -1;// = isPointInsidePolygon(*intersections.begin(), p);
            // for (int i = 0; i < p.size() && !isInside; ++i) {
            //     Line side(p[i], p[(i + 1) % p.size()]);
            //     if (isOnSegment(*intersections.begin(), side)) {
            //         isInside = true;
            //     }
            // }
            for (int i = 1; i < intersections.size(); ++i) {
                // initialize isInside
                // if (fabs(intersections[i].x - intersections[i-1].x) < EPS &&
                //         fabs(intersections[i].y - intersections[i-1].y) < EPS) {
                //     continue;
                // }
                if (intersections[i] == intersections[i-1]) {
                    // printf("sama coy\n");
                    continue;
                }
                if (isInside == -1) {
                    Point center = (intersections[0] + intersections[1]) / 2.f;
                    isInside = isPointInsidePolygon(center, p);
                    // if (z == polygons.size() - 1 && isInside) {
                    //     Circle(center.x, center.y, 3, Color(127, 255, 255)).drawSolid(fb, Point(200, 200));
                    // }
                }
                if (!isInside) {
                    cutAndDrawLine(Line(intersections[i-1], intersections[i], l.color, l.thickness, l.autoInc), z + 1, fb, offset);
                }
                isInside = !isInside;
            }
        } else {
            l.draw(fb, offset);
        }
    }

    std::vector<Polygon*> polygons;
};

#endif
