#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#ifndef NUM_THREAD
#	define NUM_THREAD 16
#endif

#include <unistd.h>
#include <fcntl.h>		/* for fcntl */
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>	/* for mmap */
#include <sys/ioctl.h>
#include <linux/fb.h>	/* for frame buffer properties */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <map>
#include <cmath>
#include <algorithm>

#include <pthread.h>

#include "point.h"

/**
 * Class that represents color (defined by red, green, blue, and alpha values)
 */
class Color {
public:
	Color() : red(0), green(0), blue(0), alpha(0) {}
	Color(char _red, char _green, char _blue, char _alpha = 0) :
		red(_red), green(_green), blue(_blue), alpha(_alpha) {}
	char red, green, blue, alpha;

	bool operator== (const Color &rhs) const {
		return ((red == rhs.red) and (green == rhs.green)
			and (blue == rhs.blue) and (alpha == rhs.alpha));
	}

	bool operator!= (const Color &rhs) const {
		return (!(*this == rhs));
	}
	/* static constants */
	static const Color RED;
	static const Color GREEN;
	static const Color BLUE;
	static const Color YELLOW;
	static const Color PURPLE;
	static const Color CYAN;
	static const Color BLACK;
	static const Color WHITE;
	static const Color EMPTY;
};

class Frame {
public:
	virtual void set(int x, int y, Color c) = 0;
	virtual void set(Point p, Color c) = 0;
	virtual void set(int x, int y, char red, char green, char blue, char alpha = 0) = 0;
	virtual Color get(Point p) = 0;
	virtual Color get(int x, int y) = 0;
	virtual void clear() = 0;
	virtual int getXSize() = 0;
	virtual int getYSize() = 0;
	bool isThreadSafe;
};

class FrameMatrix : public Frame {
public:
	FrameMatrix() {
		xres = yres = 0;
	}

	void set(int x, int y, Color c) {
		matrix[y][x] = c;
		xres = std::max(xres, x);
		yres = std::max(yres, y);

	};

	void set(Point p, Color c) {
		// Cast the coordinate to integers, since the Frame accepts only integer
		// coordinates
		int x = (int)p.x;
		int y = (int)p.y;

		matrix[y][x] = c;
		xres = std::max(xres, x);
		yres = std::max(yres, y);

	};
	void set(int x, int y, char red, char green, char blue, char alpha = 0) {
		matrix[y][x] = Color(red, green, blue, alpha);
		xres = std::max(xres, x);
		yres = std::max(yres, y);

	};
	
	Color get(Point p) {
		// Cast the coordinate to integers, since the Frame accepts only integer
		// coordinates
		int x = (int)p.x;
		int y = (int)p.y;

		if (y > yres)
			y %= yres+1;
		while (y < 0)
			y += yres+1;

		if (x > xres)
			x %= xres+1;
		while (x < 0)
			x += xres+1;

		if ((matrix.count(y)) and (matrix[y].count(x)))
			return matrix[y][x];
		return Color::EMPTY;
	};
	
	Color get(int x, int y) {
		if (y > yres)
			y %= yres+1;
		while (y < 0)
			y += yres+1;

		if (x > xres)
			x %= xres+1;
		while (x < 0)
			x += xres+1;

		if ((matrix.count(y)) and (matrix[y].count(x)))
			return matrix[y][x];
		return Color::EMPTY;
	};

	void clear() {
		matrix.clear();
	}

	int getXSize() {
		return xres;
	}

	int getYSize() {
		return yres;
	}
private:
	std::map<int, std::map<int, Color> > matrix;
	int xres, yres;
};

/**
 * Helper class to access linux framebuffer.
 * Source: http://xathrya.web.id/blog/2012/10/26/graphic-programming-using-frame-buffer-on-linux/
 * with refactoring and some modifications.
 */
class FrameBuffer : public Frame {
public:	
	FrameBuffer() {
		// initialize when needed
		if (fbp == NULL) {
			init();
		}
	}

	~FrameBuffer() {
		// close the framebuffer file
		close(fbfd);
	}

	/**
	 * Initialize frame buffer. Copy frame buffer ("/dev/fb0") into memory.
	 */
	void init() {
		/* open the file for reading and writing */
		fbfd = open("/dev/fb0",O_RDWR);
		if (!fbfd) {
			printf("Error: cannot open framebuffer device.\n");
			exit(1);
		}
		 
		/* get the fixed screen information */
		if (ioctl (fbfd, FBIOGET_FSCREENINFO, &finfo)) {
			printf("Error reading fixed information.\n");
			exit(2);
		}
		 
		/* get variable screen information */
		if (ioctl (fbfd, FBIOGET_VSCREENINFO, &vinfo)) {
			printf("Error reading variable information.\n");
			exit(3);
		}

		/* store important information */
		xres = vinfo.xres;
		yres = vinfo.yres;
		bits_per_pixel = vinfo.bits_per_pixel;

		/* map the device to memory */
		fbp = (char*)mmap(0, finfo.smem_len, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);

		/* initialize visited vector */
		visited.clear();
		visited.resize(xres);
		for (int i = 0; i < xres; ++i) {
			visited[i].resize(yres);
		}
		lastSet.clear();

		isThreadSafe = true;
	}

	/**
	 * Set pixel at (x, y) using color c.
	 */
	void set(int x, int y, Color c) {
		set(x, y, c.red, c.green, c.blue, c.alpha);
	}

	/**
	 * Set pixel at p using color c.
	 */
	void set(Point p, Color c) {
		// Cast the coordinate to integers, since the Frame accepts only integer
		// coordinates
		int x = (int)p.x;
		int y = (int)p.y;

		set(x, y, c.red, c.green, c.blue, c.alpha);
	}

	/**
	 * Set pixel at (x, y) using RGBA red, green, blue, and alpha.
	 * The value of alpha is unnecessary, the default is 0;
	 */
	void set(int x, int y, char red, char green, char blue, char alpha = 0) {
		/* Validate coordinate */
		if (x < 0 || x >= xres || y < 0 || y >= yres) {
			return;
		}
		int location = getLocation(x, y);
		if (bits_per_pixel == 32) {
			*(fbp + location + 0) = blue;
			*(fbp + location + 1) = green;
			*(fbp + location + 2) = red;
			*(fbp + location + 3) = alpha;
		}
		else if (bits_per_pixel == 16) {
			unsigned short int t = (red >> 3) << 11 | (green >> 2) << 5 | (blue >> 3);
			*((unsigned short int*)(fbp + location)) = t;			
		}
		else {
			printf("Unknown bpp format: %d bpp\n", vinfo.bits_per_pixel);
		}
		/* adds to lastSet */
		if (!clearMode && isThreadSafe) {
			if (!visited[x][y]) {
				visited[x][y] = 1;
				lastSet.push_back(std::make_pair(x, y));
			}
		}
	}

	/**
	 * Get color at p.
	 */
	Color get(Point p) {
		// Cast the coordinate to integers, since the Frame accepts only integer
		// coordinates
		int x = (int)p.x;
		int y = (int)p.y;

		return get(x, y);
	}

	/**
	 * Get color at (x, y).
	 */
	Color get(int x, int y) {
		/* Validate coordinate */
		if (x < 0 || x >= xres || y < 0 || y >= yres) {
			return Color();
		}
		int location = getLocation(x, y);
		char red, green, blue, alpha;
		if (bits_per_pixel == 32) {
			blue  = *(fbp + location + 0);
			green = *(fbp + location + 1);
			red   = *(fbp + location + 2);
			alpha = *(fbp + location + 3);
		}
		else if (bits_per_pixel == 16) {
			unsigned short int t = *((unsigned short int*)(fbp + location));
			red   = (char)((t >> 11) << 3);
			green = (char)((t >> 5) << 2);
			blue  = (char)(t << 3);
			alpha = 0;
		}
		else {
			printf("Unknown bpp format: %d bpp\n", vinfo.bits_per_pixel);
			red = green = blue = alpha = 0;
		}
		return Color(red, green, blue, alpha);
	}

	struct ClearData {
		FrameBuffer* context;
		int idxThread;
	};

	static void* parallelClear(void* param) {
		ClearData *data = (ClearData*) param;
		FrameBuffer *fb = data->context;
		int rank = data->idxThread;

		int step = fb->lastSet.size() / NUM_THREAD;
		int startX = step * rank;
		int finishX = step * (rank + 1);
		if (finishX >= fb->lastSet.size()) {
			finishX = fb->lastSet.size() - 1;
		}

		for (int x = startX; x < finishX; ++x) {
			std::pair<int, int> &p = lastSet[x];
			fb->set(p.first, p.second, Color::BLACK);
			visited[p.first][p.second] = 0;
		}
	}

	/**
	 * Clear the screen. Color them with the default color (BLACK)
	 */
	void clear() {
#if 0
		clearMode = true;
		pthread_t thread_pool[NUM_THREAD];
		ClearData data[NUM_THREAD];
		for (int i = 0; i < NUM_THREAD; ++i) {
			data[i].context = this;
			data[i].idxThread = i;
			pthread_create(&thread_pool[i], NULL, FrameBuffer::parallelClear, (void*)&data[i]);
		}
		for (int i = 0; i < NUM_THREAD; ++i) {
			pthread_join(thread_pool[i], NULL);
		}
		lastSet.clear();
		clearMode = false;
#endif
#if 1
		clearMode = true;
		// memset(fbp + getLocation(0, 0), 0, finfo.line_length * yres);
		while (lastSet.size()) {
			// printf("huba: size = %d\n", lastSet.size());
			std::pair<int, int> &p = lastSet.back();
			set(p.first, p.second, Color::BLACK);
			visited[p.first][p.second] = 0;
			lastSet.pop_back();
		}
		clearMode = false;
#endif
	}

	/*
	 * Getter for informations regarding the frame buffer
	 */
	int getBitsPerPixel() {
		return bits_per_pixel;
	}

	int getXSize() {
		return xres;
	}

	int getYSize() {
		return yres;
	}

private:
	/**
	 * Get memory offset for pixel at (x, y).
	 */
	int getLocation(int x, int y) {
		return (x + vinfo.xoffset) * (vinfo.bits_per_pixel / 8)
				+ (y + vinfo.yoffset) * finfo.line_length;
	}

	int xres, yres;			/* screen resolution */
	int bits_per_pixel;		/* guess what */
	struct fb_var_screeninfo vinfo;
	struct fb_fix_screeninfo finfo;
	static bool clearMode;	/* true when clear() is called */
	static int fbfd;		/* frame buffer file descriptor */
	static char* fbp;		/* pointer to framebuffer */
	static std::vector<std::vector<int> > visited;
	static std::vector<std::pair<int, int> > lastSet;
};

/* set default values for static variables */
/* Color constants */
const Color Color::RED 		= Color(255,   0,   0);
const Color Color::GREEN 	= Color(  0, 255,   0);
const Color Color::BLUE 	= Color(  0,   0, 255);
const Color Color::YELLOW 	= Color(255, 255,   0);
const Color Color::PURPLE 	= Color(255,   0, 255);
const Color Color::CYAN		= Color(  0, 255, 255);
const Color Color::BLACK	= Color(  0,   0,   0);
const Color Color::WHITE	= Color(255, 255, 255);
const Color Color::EMPTY	= Color(-1, -1, -1, -1);

/* FrameBuffer variables */
int FrameBuffer::fbfd = 0;
char* FrameBuffer::fbp = NULL;
bool FrameBuffer::clearMode = false;
std::vector<std::vector<int> > FrameBuffer::visited;
std::vector<std::pair<int, int> > FrameBuffer::lastSet;

#endif
