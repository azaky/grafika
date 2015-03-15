#ifndef FRAME_CANVAS_H
#define FRAME_CANVAS_H

#include "framebuffer.h"
#include "drawable.h"
#include "line.h"
#include <vector>
#include <queue>
#include <omp.h>
#include <pthread.h>
#include <utility>

#ifndef NUM_THREAD
#	define NUM_THREAD 16
#endif

class FrameCanvas : public Frame, public Drawable {
public:
	FrameCanvas(int xres, int yres) {
		this->xres = xres;
		this->yres = yres;

		// resize the World
		world.clear();
		world.resize(xres);
		for (int i = 0; i < xres; ++i) {
			world[i].resize(yres);
		}
		isDirty.resize(xres);
		for (int i = 0; i < xres; ++i) {
			isDirty[i].resize(yres, false);
		}

		// set default values for window and view
		windowPos = Point(0, 0);
		windowLen = Point(xres, yres);
		viewPos = Point(0, 0);
		viewLen = Point(xres, yres);
	}

	void set(int x, int y, Color c) {
		// validate coordinate
		if (x < 0 || x >= xres || y < 0 || y >= yres) {
			return;
		}

		world[x][y] = c;
		if (!isDirty[x][y]) {
			changed.push_back(std::make_pair(x, y));
			isDirty[x][y] = true;
		}
	}

	void set(Point p, Color c) {
		set((int)p.x, (int)p.y, c);
	}

	void set(int x, int y, char red, char green, char blue, char alpha = 0) {
		set(x, y, Color(red, green, blue, alpha));
	}

	Color get(Point p) {
		return get((int)p.x, (int)p.y);
	}

	Color get(int x, int y) {
		// calidate coordinate
		if (x < 0 || x >= xres || y < 0 || y >= yres) {
			return Color();
		}

		return world[x][y];
	}

	void clear() {
		while (!changed.empty()) {
			std::pair<int, int> curr = changed.back();
			changed.pop_back();
			Color color = background.get(curr.first, curr.second);
			if (color != Color::EMPTY)
				world[curr.first][curr.second] = color;
			else
				world[curr.first][curr.second] = Color::BLACK;
			isDirty[curr.first][curr.second] = false;
		}
	}

	struct DrawData {
		FrameCanvas* context;
		Frame* frame;
		Point* offset;
		int idxThread;
	};

	static void* parallelDraw(void* param) {
		DrawData *data = (DrawData*) param;
		FrameCanvas *fc = data->context;
		Frame *frame = data->frame;
		Point *offset = data->offset;
		int rank = data->idxThread;

		// printf("draw from thread %d\n", rank);

		int topLeftX = fc->viewPos.x;
		int topLeftY = fc->viewPos.y;
		int bottomRightX = fc->viewPos.x + fc->viewLen.x;
		int bottomRightY = fc->viewPos.y + fc->viewLen.y;

		int step = (bottomRightX - topLeftX + 1) / NUM_THREAD;
		int startX = (topLeftX + step * rank);
		int finishX = (topLeftX + step * (rank + 1));
		if (finishX > bottomRightX) {
			finishX = bottomRightX + 1;
		}

		for (int x = startX; x < finishX; ++x) {
			for (int y = topLeftY; y <= bottomRightY; ++y) {
				Point c = fc->viewToWindow(Point(x, y));
				frame->set(x, y, fc->get(c));
			}
		}
	}

	void draw(Frame* fb, Point offset = Point()) {
		fb->isThreadSafe = false;
		pthread_t thread_pool[NUM_THREAD];
		DrawData data[NUM_THREAD];
		for (int i = 0; i < NUM_THREAD; ++i) {
			data[i].context = this;
			data[i].frame = fb;
			data[i].offset = &offset;
			data[i].idxThread = i;
			pthread_create(&thread_pool[i], NULL, FrameCanvas::parallelDraw, (void*)&data[i]);
		}
		for (int i = 0; i < NUM_THREAD; ++i) {
			pthread_join(thread_pool[i], NULL);
		}
		fb->isThreadSafe = true;
	}

	void resetBackground() {
		for (int x = 0; x < xres; ++x) {
			for (int y = 0; y < yres; ++y) {
				Color color;
				if ((color = background.get(x, y)) != Color::EMPTY)
					world[x][y] = color;
			}
		}
	}

	int getXSize() {
		return xres;
	}

	int getYSize() {
		return yres;
	}

	void moveWindow(const Point& p) {
		windowPos += p;

		if (!(p == Point(0, 0))) {
			windowChanged = true;
		}
	}

	Point getWindowPos() {
		return windowPos;
	}

	void setWindowPos(Point _windowPos) {
		if (!(windowPos == _windowPos)) {
			windowPos = _windowPos;
			windowChanged = true;
		}
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

		if (scale != 1) {
			windowChanged = true;
		}
	}

	void setWindowLen(Point _windowLen) {
		windowLen = _windowLen;

		windowChanged = true;
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

	Frame* getBgFrame() {
		return &background;
	}

	Frame* tempFrame;
private:
	int xres, yres;								// resolution of the World
	std::vector<std::vector<Color> > world;		// color matrix for the World
	Point windowLen, windowPos;
	Point viewLen, viewPos;
	std::vector<std::pair<int, int> > changed;
	std::vector<std::vector<bool> > isDirty;
	FrameMatrix background;

	bool windowChanged;
};

#endif