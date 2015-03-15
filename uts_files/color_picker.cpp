#include <termios.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../lib/conio.h"
#include "../lib/framebuffer.h"
#include "../lib/mouse.h"
#include "../lib/cursor.h"
#include "../lib/sprite.h"
#include "../lib/canvas.h"
using namespace std;

// Global Variables
FrameBuffer fb;
Color planecolor(1,1,1);
Color shipcolor(1,1,1);
FrameMatrix planepattern;
FrameMatrix shippattern;

int framerate = 10;
float period = 1. / framerate;

void rectangle(int x1, int y1, int x2, int y2, Color p, int width = 1) {
	for (int x = x1; x <= x2; ++x) {
		for (int y = -width; y <= width; ++y) {
			fb.set(x, y1 + y, p);
			fb.set(x, y2 + y, p);
		}
	}
	for (int y = y1; y <= y2; ++y) {
		for (int x = -width; x <= width; ++x) {
			fb.set(x + x1, y, p);
			fb.set(x + x2, y, p);
		}
	}
}

void ColorSelection() {
	float last_render = clock();

	// FrameMatrix for pattern
	FrameMatrix patternmatrix1;
	FrameMatrix patternmatrix2;
	FrameMatrix patternmatrix3;
	FrameMatrix patternmatrix4;	

	// Initial color
	Color colorpick(1,1,1);

	// screen size
	int width = fb.getXSize();
	int height = fb.getYSize();

	// mouse controller
	Point axis;
	int wheel = 0 , prevwheel = 0;
	int left,middle,right;

	int planeclicked = 0, shipclicked = 0;
	// color picker size
	int size = 1;
	
	// generate rgb table
	int _r[256 * 6], _g[256 * 6], _b[256 * 6];
	_r[0] = 255, _g[0] = 0, _b[0] = 0;
	// red to yellow
	for (int i = 1; i < 256; ++i) {
		_r[i] = _r[i-1];
		_g[i] = _g[i-1] + 1;
		_b[i] = _b[i-1];
	}
	_r[256 * 1] = _r[256 * 1 - 1];
	_g[256 * 1] = _g[256 * 1 - 1];
	_b[256 * 1] = _b[256 * 1 - 1];
	// yellow to green
	for (int i = 256 + 1; i < 2 * 256; ++i) {
		_r[i] = _r[i-1] - 1;
		_g[i] = _g[i-1];
		_b[i] = _b[i-1];
	}
	_r[256 * 2] = _r[256 * 2 - 1];
	_g[256 * 2] = _g[256 * 2 - 1];
	_b[256 * 2] = _b[256 * 2 - 1];
	// green to cyan
	for (int i = 2 * 256 + 1; i < 3 * 256; ++i) {
		_r[i] = _r[i-1];
		_g[i] = _g[i-1];
		_b[i] = _b[i-1] + 1;
	}
	_r[256 * 3] = _r[256 * 3 - 1];
	_g[256 * 3] = _g[256 * 3 - 1];
	_b[256 * 3] = _b[256 * 3 - 1];
	// cyan to blue
	for (int i = 3 * 256 + 1; i < 4 * 256; ++i) {
		_r[i] = _r[i-1];
		_g[i] = _g[i-1] - 1;
		_b[i] = _b[i-1];
	}
	_r[256 * 4] = _r[256 * 4 - 1];
	_g[256 * 4] = _g[256 * 4 - 1];
	_b[256 * 4] = _b[256 * 4 - 1];
	// blue to purple
	for (int i = 4 * 256 + 1; i < 5 * 256; ++i) {
		_r[i] = _r[i-1] + 1;
		_g[i] = _g[i-1];
		_b[i] = _b[i-1];
	}
	_r[256 * 5] = _r[256 * 5 - 1];
	_g[256 * 5] = _g[256 * 5 - 1];
	_b[256 * 5] = _b[256 * 5 - 1];
	// purple to red
	for (int i = 5 * 256 + 1; i < 6 * 256; ++i) {
		_r[i] = _r[i-1];
		_g[i] = _g[i-1];
		_b[i] = _b[i-1] - 1;
	}
	// position for color pallette
	int position = 0, px = 256 * size - 1, py = 0, mode = 0;

	// set up sprites
	Sprite plane(planecolor,Color::BLUE);
	plane.add("object/plane_above");

	Sprite ship(shipcolor,Color::RED);
	ship.add("object/ship_above");
	ship.resize(4);

	Sprite pattern1(Color(0, 127, 255), Color::BLUE, 1);
	pattern1.add("object/bird");
	pattern1.resize(0.5);

	Sprite pattern2(Color(0, 127, 255), Color::BLUE, 1);
	pattern2.add("object/fish");
	pattern2.resize(0.5);

	Sprite pattern3(Color::WHITE, Color::BLACK, 1);
	pattern3.add("object/hexagon");
	pattern3.resize(0.35);

	Sprite pattern4(Color::RED, Color::RED, 1);
	pattern4.add("object/diamond");
	pattern4.resize(0.1);

	Sprite square1(Color::WHITE, Color::WHITE);
	square1.add("object/square");

	Sprite square2(Color::WHITE, Color::WHITE);
	square2.add("object/square");

	Sprite square3(Color::WHITE, Color::WHITE);
	square3.add("object/square");

	Sprite square4(Color::WHITE, Color::WHITE);
	square4.add("object/square");

	Sprite square5(Color::EMPTY, Color::WHITE);
	square5.add("object/square");

	int cmd = ' ';
	int iframe = 0;
	while (true) {
		// show information every second
		if (iframe++ % framerate == 0) {
			system("clear");
			printf("Klik pada pesawat/kapal, kemudian klik warna/pattern untuk menggantinya.\n");
			printf("Klik kanan untuk memulai permainan.\n");
			if (planeclicked) {
				printf("Status: Pesawat aktif.\n");
			} else if (shipclicked) {
				printf("Status: Kapal aktif.\n");
			} else {
				printf("Status: Tidak ada yang dipilih.\n");
			}
		}
		// system("clear");
		fb.clear();
		// Gambar pesawat
		plane.draw(&fb,Point(160,150));
		plane.drawFill(&fb,&planepattern,Point(160,150));

		// Gambar kapal
		ship.draw(&fb,Point(900,200));
		ship.drawFill(&fb,&shippattern,Point(900,200));

		// tampilin color palette
		for (int x = 0; x < size * 256; ++x) {
			for (int y = 0; y < size * 256; ++y) {
				int r = _r[position], g = _g[position], b = _b[position];
				// ke bawah dulu
				r = r - (r * y) / (size * 256);
				g = g - (g * y) / (size * 256);
				b = b - (b * y) / (size * 256);
				// baru ke kiri
				int target = 255 - y / size;
				r = target + x * (r - target) / (size * 256);
				g = target + x * (g - target) / (size * 256);
				b = target + x * (b - target) / (size * 256);
				// set Color
				fb.set(width - 800 - size * 256 + x, 480 + y, r, g, b);
			}
		}

		// right sidebar
		for (int y = 0; y < size * 256; ++y) {
			int p = y * 6 / size;
			int r = _r[p], g = _g[p], b = _b[p];
			for (int x = 0; x < 20; ++x) {
				fb.set(width - 790 + x, 480 + y, r, g, b);
			}
		}
		// mark position
		int ypos = position * size / 6;
		rectangle(width - 790, 477 + ypos, width - 770, 483 + ypos, Color(255, 255, 255));
		if (mode == 1) {
			rectangle(width - 800 - size * 256 + px - 3, 480 + py - 3,
				width - 800 - size * 256 + px + 3, 480 + py + 3, Color(1, 1, 1));
		}
		
		// Create pattern menu
		pattern1.draw(&patternmatrix1);
		pattern1.drawFill(&patternmatrix1, NULL);
		
		pattern2.draw(&patternmatrix2);
		pattern2.drawFill(&patternmatrix2, NULL);
		
		pattern3.draw(&patternmatrix3);
		pattern3.drawFill(&patternmatrix3, NULL);
		
		pattern4.draw(&patternmatrix4);
		pattern4.drawFill(&patternmatrix4, NULL);

		square1.draw(&fb,Point(width - 750, 477));
		square1.drawFill(&fb,&patternmatrix1,Point(width-750,477));

		square2.draw(&fb,Point(width - 680, 477));
		square2.drawFill(&fb,&patternmatrix2,Point(width-680,477));

		square3.draw(&fb,Point(width - 750, 547));
		square3.drawFill(&fb,&patternmatrix3,Point(width-750,547));

		square4.draw(&fb,Point(width - 680, 547));
		square4.drawFill(&fb,&patternmatrix4,Point(width-680,547));

		square5.draw(&fb,Point(width - 610, 477));

		prevwheel = wheel;
		axis.x = Mouse::getXAxis();
		axis.y = Mouse::getYAxis();
		wheel = Mouse::getWheelAxis();
		Mouse::getButton(&left, &middle, &right);
		
		// Fill Warna (Klik pesawat / Klik kapal terlebih dahulu)
		if(Mouse::getLeftButton()){
			if(axis.x >= 160 && axis.x <= 483 && axis.y >= 150 
				&& axis.y <= 416){
				planeclicked = 1;
				shipclicked = 0;
			}
			else if(axis.x >= 900 && axis.x <= 1052 && axis.y >= 200
				&& axis.y <= 500){
				planeclicked = 0;
				shipclicked = 1;
			}
			else if(axis.x >= width - 750 && axis.x <= width - 700 && axis.y >= 477 && axis.y <= 527){
				if(planeclicked==1){
					planepattern = patternmatrix1;

				}
				else if(shipclicked==1){
					shippattern = patternmatrix1;
				}
			}
			else if(axis.x >= width - 680 && axis.x <= width - 630 && axis.y >= 477 && axis.y <= 527){
				if(planeclicked==1){
					planepattern = patternmatrix2;

				}
				else if(shipclicked==1){
					shippattern = patternmatrix2;
				}
			}
			else if(axis.x >= width - 750 && axis.x <= width - 700 && axis.y >= 547 && axis.y <= 597){
				if(planeclicked==1){
					planepattern = patternmatrix3;

				}
				else if(shipclicked==1){
					shippattern = patternmatrix3;
				}
			}
			else if(axis.x >= width - 680 && axis.x <= width - 630 && axis.y >= 547 && axis.y <= 597){
				if(planeclicked==1){
					planepattern = patternmatrix4;

				}
				else if(shipclicked==1){
					shippattern = patternmatrix4;
				}
			}
			else if(axis.x >= width - 610 && axis.x <= width - 560 && axis.y >= 477 && axis.y <= 527){
				if(planeclicked==1){
					planepattern.clear();
				}
				else if(shipclicked==1){
					shippattern.clear();
				}
			}
			// hue selector
			else if (width - 790 <= axis.x && axis.x <= width - 770 && 480 <= axis.y && axis.y <= 480 + size * 256) {
				position = 6 * (axis.y - 480);
			}
			// color selector
			else if (width - 800 - size * 256 <= axis.x && axis.x <= width - 800
					&& 480 <= axis.y && axis.y <= 480 + size * 256) {
				colorpick = fb.get(axis);
				if(planeclicked==1){
					planecolor = colorpick;
					plane.setColor(colorpick);
				}
				else if(shipclicked==1) {
					shipcolor = colorpick;
					ship.setColor(colorpick);
				}
			}
			//printf("warna = RGBA(%d, %d, %d, %d)\n", colorpick1.red & 255, colorpick1.green & 255, colorpick1.blue & 255, colorpick1.alpha & 255);
			Cursor::DRAG.draw(&fb,axis);

		}
		else{
			Cursor::POINTER.draw(&fb,axis);
		}

		// Collor pallete scroll color
		if(Mouse::getWheelAxis()){
			if (wheel < prevwheel) {
				position += 6 / size;
				if (position >= 6 * 256) position = 0;
			}
			else if (wheel > prevwheel) {
				position -= 6 / size;
				if (position < 0) position = 6 * 256 - 1;
			}
		}

		// Quit program
		if(Mouse::getRightButton()){
			break;
		}

		float now_render = clock();
		usleep(std::max(0., 1e6 / framerate - (now_render - last_render)));
		last_render = now_render;
	}
	return;
}
