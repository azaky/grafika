#ifndef LAYER_H
#define LAYER_H

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

class Layer : public Drawable {
public:
    Layer() {}

    void add(Polygon* s) {
        polygons.push_back(s);
    }

    void draw(FrameBuffer *fb, Point offset = Point()) {
        // for each lines sorted by their z-index
        //     for each polygon whose z-index is > line's z-index
        //         cutAndDrawLine(l, z+1, fb, offset)
        int z = 0;
        for (std::vector<Polygon*>::iterator polygon = polygons.begin(); polygon != polygons.end(); polygon++, z++) {
            std::vector<Line> lines = (*polygon)->getLines();
            for (std::vector<Line>::iterator line = lines.begin(); line != lines.end(); line++) {
                cutAndDrawLine(*line, z + 1, fb, offset);
            }
        }
    }

private:
    void cutAndDrawLine(Line l, int z, FrameBuffer *fb, Point offset){
        if (z < polygons.size()) {
            // cut this line -> lines
            // foreach newLine in lines > cutAndDrawLine(newLine, z+1, fb, offset)

            // cut l with polygons[z]
            Polygon& p = *polygons[z];
            
        } else {
            l.draw(fb, offset);
        }
    }

    std::vector<Polygon*> polygons;
};

#endif
