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
	Sprite pesawat("pesawat.txt");
	Bullet peluruPesawat;
	Polyline ledakanPesawat[3] = {Polyline("ledakan1.txt"), Polyline("ledakan2.txt"), Polyline("ledakan3.txt")};
	bool isPesawatLedak = false;
	int kondisiPesawatLedak = 0;
	float periodePesawatLedak = 4.0;
	int xPesawatLedak, yPesawatLedak;

	Sprite kapal("kapal.txt");
	kapal.color = Color::GREEN;
	Bullet peluruKapal;
	Polyline ledakanKapal[3] = {Polyline("ledakan1.txt"), Polyline("ledakan2.txt"), Polyline("ledakan3.txt")};
	bool isKapalLedak = false;
	int kondisiKapalLedak = 0;
	float periodeKapalLedak = 4.0;
	int xKapalLedak, yKapalLedak;
	
	for (int it = 0; it < 3; ++it)
		ledakanPesawat[it].color = ledakanKapal[it].color = Color::RED;
	pesawat.vx = -300;
	pesawat.vy = 0;
	pesawat.x = fb.xres;
	pesawat.y = 0;
	pesawat.color = Color::BLUE;
	peluruPesawat.lengthx = 0, peluruPesawat.vx = 0;
	peluruPesawat.lengthy = 30, peluruPesawat.vy = 130;
	bool existPeluruPesawat = false;

	kapal.vx = 120;
	kapal.vy = 0;
	kapal.x = 0;
	kapal.y = fb.yres / 2;
	kapal.color = Color::WHITE;
	peluruKapal.lengthx = 0, peluruKapal.vx = 0;
	peluruKapal.lengthy = -30, peluruKapal.vy = -120;
	bool existPeluruKapal = false;

	// bagian yang mengatur framerate layar
	int framerate = 20;
	float last_render = clock();
	float periodeKapal = 3.0, periodePesawat = 1.0;
	
	while (true) {
		// gambar objek

		if (isPesawatLedak == true) {
			ledakanPesawat[kondisiPesawatLedak].draw(&fb, xPesawatLedak, yPesawatLedak);
			pesawat.x = fb.xres;
			pesawat.y = 0;
		}
		else
			pesawat.draw(&fb);

		if (isKapalLedak == true) {
			kapal.x = 0;
			kapal.y = fb.yres / 2;
			ledakanKapal[kondisiKapalLedak].draw(&fb, xKapalLedak, yKapalLedak);
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
		if (pesawat.x >= fb.xres) {
			pesawat.x = 0;
		}
		else if (pesawat.x + pesawat.sizex < 0) {
			pesawat.x = fb.xres - 1;
		}

		// kapal
		kapal.update(1. / framerate);
		if (kapal.x >= fb.xres) {
			kapal.x = 0;
		}
		else if (kapal.x + kapal.sizex < 0) {
			kapal.x = fb.xres - 1;
		}

		// peluruKapal pesawat
		periodePesawat = periodePesawat - (1. / framerate);
		if (periodePesawat < 0) {
			existPeluruPesawat = true;
			periodePesawat = 5.0;
			peluruPesawat.x = pesawat.x;
			peluruPesawat.y = pesawat.y+pesawat.sizey;
		}
		if (existPeluruPesawat)
			peluruPesawat.update(1. / framerate);
		if (peluruPesawat.y > kapal.y) {
			peluruPesawat.x = peluruPesawat.y = 0;
			existPeluruPesawat = false;
		}

		// peluruKapal kapal
		periodeKapal = periodeKapal - (1. / framerate);
		if (periodeKapal < 0) {
			existPeluruKapal = true;
			periodeKapal = 3.0;
			peluruKapal.x = kapal.x;
			peluruKapal.y = kapal.y;
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
			if (pesawat.collide(peluruKapal.x, peluruKapal.y)) {
				isPesawatLedak = true;
				periodePesawatLedak = 1.0;
				kondisiPesawatLedak = 0;
				xPesawatLedak = pesawat.x;
				yPesawatLedak = pesawat.y;
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
			if (kapal.collide(peluruPesawat.x, peluruPesawat.y+15)) {
				isKapalLedak = true;
				periodeKapalLedak = 1.0;
				kondisiKapalLedak = 0;
				xKapalLedak = kapal.x-50;
				yKapalLedak = kapal.y-40;
			}
		}

		// clear screen
		system("clear");
	}
	return 0;
}
