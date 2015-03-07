#ifndef DRAWABLE_H
#define DRAWABLE_H

#include "framebuffer.h"

class Drawable {
public:
	virtual void draw(Frame* fb, Point offset = Point()) = 0;
	// virtual void draw(FrameBuffer* fb, int offsetx = 0, int offsety = 0) = 0;
};

#endif