#include "../lib/framebuffer.h"
#include "../lib/sprite.h"
#include "../lib/bullet.h"
#include <ctime>
#include <algorithm>

void ShipExplode() {

	Sprite ship(shipcolor, Color::RED);
	ship.add("object/boat");
	ship.resize(1.7);
	ship.setPos(Point(0, 200));
	ship.setV(Point(200, 0));
	bool isShipExplode = true;
	int stateShipExplosion = 0;
	float periodShipExplosion = 4.0;
	Point shipExplosionLocation;

	Sprite wave(Color::WHITE, Color::BLUE);
	wave.add("object/wave");
	wave.resize(1.7);
	wave.resize(0.65);
	wave.setPos(Point(0, 430));
	wave.setV(Point(0, 0));

	Sprite planeBullet(Color::BLUE);
	planeBullet.add("object/bullet");

	Bullet peluruPesawat;
	peluruPesawat.size = Point(0, 30);
	peluruPesawat.v = Point(0, 120);
	bool existPeluruPesawat = false;

	float last_render = clock();
	float periodeKapal = 1.5, periodePesawat = 1.0;
	
	int iframe = 0;

	system("clear");
	while (true) {
		// draw objek
		fb.clear();

		// ombak
		wave.draw(&fb);

		// kapal
		ship.draw(&fb);
		ship.drawFill(&fb, &shippattern);


		// peluru
		if (existPeluruPesawat)
			peluruPesawat.draw(&fb);

		// delay sesuai framerate
		float now_render = clock();
		usleep(std::max(0., 1e6 / framerate - (now_render - last_render)));
		last_render = now_render;

		// update objek
		// kapal
		ship.update(period);
		if (ship.getPos().x >= fb.getXSize()) {
			ship.getPos().x = 0;
		}
		else if (ship.getPos().x + ship.getBottomRight().x < 0) {
			ship.getPos().x = fb.getXSize() - 1;
		}

		// ombak
		wave.update(period);
		if (wave.getPos().x >= fb.getXSize()) {
			wave.getPos().x = 0;
		}
		else if (wave.getPos().x + wave.getBottomRight().x < 0) {
			wave.getPos().x = fb.getXSize() - 1;
		}

		// peluruKapal kapal
		periodeKapal = periodeKapal - period;
		if (periodeKapal < 0) {
			existPeluruPesawat = true;
			periodeKapal = 2;
			peluruPesawat.p.x = ship.getPos().x + ship.getTopLeft().x + 600;// + (ship.getBottomRight().x - ship.getTopLeft().x) / 2;
			peluruPesawat.p.y = ship.getPos().y;
		}
		if (existPeluruPesawat)
			peluruPesawat.update(period);

		if (existPeluruPesawat && (ship.collide(peluruPesawat.p) || ship.collide(peluruPesawat.p + peluruPesawat.size))) {
			periodShipExplosion = 1.0;
			shipExplosionLocation = ship.getPos();
			break;
		}
	}

	// kapal pecah
	int gravity = 1200;

	// boat shrink
	int nCracks = 3;
	std::vector<Sprite> cracks;
	for (int i = 1; i <= nCracks; ++i) {
		Sprite crack(shipcolor, Color::RED);
		char temp[100];
		sprintf(temp, "object/crack%d", i);
		crack.add(temp);
		crack.setPos(shipExplosionLocation);
		crack.resize(1.7);
		cracks.push_back(crack);
	}

	// set directions
	int crackSpin[3], crackDSpin[3];
	cracks[0].setV(Point(-100, 100));
	cracks[1].setV(Point(30, gravity));
	cracks[2].setV(Point(40, 0));
	for (int i = 0; i < nCracks; ++i) {
		cracks[i].setA(Point(0, gravity/2));
		crackSpin[i] = 0;
		crackDSpin[i] = (i + 1) * 10;
	}

	// bum bum
	Sprite boom(Color::WHITE, Color:: RED);
	boom.add("object/explosion1");
	boom.resize(1.7);
	boom.setPos(shipExplosionLocation);
	bool showBoom = true;

	// ground limit
	int groundY = 600;

	// do the thing!
	bool terminate = false;
	while (!terminate) {
		// Draw objek
		fb.clear();

		// ombak
		wave.draw(&fb);

		// boom
		if (showBoom) {
			boom.draw(&fb);
		}

		// puing kapal
		for (int i = 0; i < nCracks; ++i) {
			Sprite tempCrack = cracks[i];
			Point center = cracks[i].getTopLeft() + Point(cracks[i].getSize().x / 2, cracks[i].getSize().y / 4);
			// Circle(center.x, center.y, 3, Color::RED).drawSolid(&fb);
			tempCrack.rotate(crackDSpin[i] * crackSpin[i]++, center);
			tempCrack.draw(&fb);
			tempCrack.drawFill(&fb, &shippattern);
		}


		// delay sesuai framerate
		float now_render = clock();
		usleep(std::max(0.f, 1500 * 1000 / framerate - (now_render - last_render)));
		last_render = now_render;

		// update objek
		// puing kapalcount
		for (int i = 0; i < nCracks; ++i)
		{
			cracks[i].update(period);
			if (cracks[i].getBottomRight().y + cracks[i].getPos().y >= groundY) {
				cracks[i].setV(Point(0, 0));
				crackDSpin[i] = 0;
			}
			if (cracks[i].getBottomRight().y + cracks[i].getPos().y >= 750) {
				terminate = true;
			}
		}

		// boom
		boom.resize(1.1, boom.getTopLeft() + Point(boom.getSize().x / 2, boom.getSize().y / 2));
		if (boom.getSize().x >= 300) {
			showBoom = false;
		}
	}
}