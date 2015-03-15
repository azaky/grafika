#include "lib/framebuffer.h"
#include "lib/sprite.h"
#include "lib/bullet.h"
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <ctime>

int main(int argc, char** argv) {
	FrameBuffer fb;

	// objek-objek pada layar
	Sprite pesawat(Color(127, 127, 255), Color::BLUE);
	pesawat.add("object/plane");
	Bullet peluruPesawat;
	Sprite ledakanPesawat[3] = {Sprite(Color::RED), Sprite(Color::GREEN), Sprite(Color::BLUE)};
	ledakanPesawat[0].add("object/explosion1");
	ledakanPesawat[1].add("object/explosion2");
	ledakanPesawat[2].add("object/explosion3");
	bool isPesawatLedak = false;
	int kondisiPesawatLedak = 0;
	float periodePesawatLedak = 4.0;
	int xPesawatLedak, yPesawatLedak;

	Sprite kapal(Color(255, 127, 127), Color::RED);
	kapal.add("object/ship");
	Bullet peluruKapal;
	Sprite ledakanKapal[3] = {Sprite(Color::RED), Sprite(Color::GREEN), Sprite(Color::BLUE)};
	ledakanKapal[0].add("object/explosion1");
	ledakanKapal[1].add("object/explosion2");
	ledakanKapal[2].add("object/explosion3");
	bool isKapalLedak = false;
	int kondisiKapalLedak = 0;
	float periodeKapalLedak = 4.0;
	int xKapalLedak, yKapalLedak;
	
	pesawat.setV(Point(-300, 0));
	pesawat.setPos(Point(fb.getXSize(), 0));
	peluruPesawat.size = Point(0, 30);
	peluruPesawat.v = Point(0, 130);
	bool existPeluruPesawat = false;

	kapal.setV(Point(120, 0));
	kapal.setPos(Point(0, fb.getYSize() / 2));
	peluruKapal.size = Point(0, -30);
	peluruKapal.v = Point(0, -120);
	bool existPeluruKapal = false;

	// bagian yang mengatur framerate layar
	int framerate = 20;
	float last_render = clock();
	float periodeKapal = 3.0, periodePesawat = 1.0;
	
	while (true) {
		// gambar objek

		if (isPesawatLedak == true) {
			ledakanPesawat[kondisiPesawatLedak].draw(&fb, Point(xPesawatLedak, yPesawatLedak));
			pesawat.setPos(Point(fb.getXSize(), 0));
		}
		else
			pesawat.draw(&fb);

		if (isKapalLedak == true) {
			kapal.setPos(Point(0, fb.getYSize() / 2));
			ledakanKapal[kondisiKapalLedak].draw(&fb, Point(xKapalLedak, yKapalLedak));
		}
		else
			kapal.draw(&fb);

		if (existPeluruKapal)
			peluruKapal.draw(&fb);
		if (existPeluruPesawat)
			peluruPesawat.draw(&fb);

		// pesawat.drawBoundingBox(&fb);
		// kapal.drawBoundingBox(&fb);

		// delay sesuai framerate
		float now_render = clock();
		usleep(1000 * 1000 / framerate - (now_render - last_render));
		last_render = now_render;

		// update objek
		// pesawat
		pesawat.update(1. / framerate);
		if (pesawat.getPos().x >= fb.getXSize()) {
			pesawat.getPos().x = 0;
		}
		else if (pesawat.getPos().x + pesawat.getBottomRight().x < 0) {
			pesawat.getPos().x = fb.getXSize() - 1;
		}

		// kapal
		kapal.update(1. / framerate);
		if (kapal.getPos().x >= fb.getXSize()) {
			kapal.getPos().x = 0;
		}
		else if (kapal.getPos().x + kapal.getBottomRight().x < 0) {
			kapal.getPos().x = fb.getXSize() - 1;
		}

		// peluruKapal pesawat
		periodePesawat = periodePesawat - (1. / framerate);
		if (periodePesawat < 0) {
			existPeluruPesawat = true;
			periodePesawat = 5.0;
			peluruPesawat.p.x = pesawat.getPos().x;
			peluruPesawat.p.y = pesawat.getPos().y+pesawat.getSize().y;
		}
		if (existPeluruPesawat)
			peluruPesawat.update(1. / framerate);
		if (peluruPesawat.p.y > kapal.getPos().y) {
			peluruPesawat.p.x = peluruPesawat.p.y = 0;
			existPeluruPesawat = false;
		}

		// peluruKapal kapal
		periodeKapal = periodeKapal - (1. / framerate);
		if (periodeKapal < 0) {
			existPeluruKapal = true;
			periodeKapal = 3.0;
			peluruKapal.p.x = kapal.getPos().x;
			peluruKapal.p.y = kapal.getPos().y;
		}
		if (existPeluruKapal)
			peluruKapal.update(1. / framerate);
		
		if (isPesawatLedak) {
			periodePesawatLedak = periodePesawatLedak - (1. / framerate);
			if (periodePesawatLedak < 0) {
				++kondisiPesawatLedak;
				if (kondisiPesawatLedak == 3)
					isPesawatLedak = false;
				periodePesawatLedak = 1.0;
			}
		}
		if (!isPesawatLedak) {
			if (pesawat.collide(Point(peluruKapal.p.x, peluruKapal.p.y))) {
				isPesawatLedak = true;
				periodePesawatLedak = 1.0;
				kondisiPesawatLedak = 0;
				xPesawatLedak = pesawat.getPos().x;
				yPesawatLedak = pesawat.getPos().y;
			}
		}


		if (isKapalLedak) {
			periodeKapalLedak = periodeKapalLedak - (1. / framerate);
			if (periodeKapalLedak < 0) {
				++kondisiKapalLedak;
				if (kondisiKapalLedak == 3)
					isKapalLedak = false;
				periodeKapalLedak = 1.0;
			}
		}
		if (!isKapalLedak) {
			if (kapal.collide(Point(peluruPesawat.p.x, peluruPesawat.p.y+15))) {
				isKapalLedak = true;
				periodeKapalLedak = 1.0;
				kondisiKapalLedak = 0;
				xKapalLedak = kapal.getPos().x-50;
				yKapalLedak = kapal.getPos().y-40;
			}
		}

		// clear screen
		fb.clear();
	}
	return 0;
}
