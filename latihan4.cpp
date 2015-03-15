#include "lib/framebuffer.h"
#include "lib/sprite.h"
#include "lib/bullet.h"
#include <ctime>
#include <algorithm>

int main() {
	FrameBuffer fb;

	Sprite plane(Color(127, 127, 255), Color::BLUE);
	plane.add("object/plane");
	plane.resize(0.4);
	plane.setPos(Point(200, 30));
	plane.setV(Point(-200, 0));
	Sprite planeExplosion[3] = {Sprite(Color::RED), Sprite(Color::GREEN), Sprite(Color::BLUE)};
	planeExplosion[0].add("object/explosion1");
	planeExplosion[1].add("object/explosion2");
	planeExplosion[2].add("object/explosion3");
	bool isPlaneExplode = false;
	int statePlaneExplosion = 0;
	float periodPlaneExplosion = 4.0;
	Point planeExplosionLocation;
	Sprite planeBullet(Color::RED);
	planeBullet.add("object/bullet");
	
	Sprite ship(Color(255, 127, 127), Color::RED);
	ship.add("object/ship");
	ship.setPos(Point(200, 400));
	ship.setV(Point(200, 0));
	Sprite shipExplosion[3] = {Sprite(Color::RED), Sprite(Color::GREEN), Sprite(Color::BLUE)};
	shipExplosion[0].add("object/explosion1");
	shipExplosion[1].add("object/explosion2");
	shipExplosion[2].add("object/explosion3");
	bool isShipExplode = false;
	int stateShipExplosion = 0;
	float periodShipExplosion = 4.0;
	Point shipExplosionLocation;
	Sprite shipBullet(Color::RED);
	shipBullet.add("object/bullet");
	
	Sprite blade(Color::WHITE, Color::BLUE);
	blade.add("object/blade");
	blade.resize(.5);
	blade.setPos(Point(210, 0));
	blade.setV(Point(-200, 0));
	int spin = 0;

	Bullet peluruPesawat;
	peluruPesawat.size = Point(0, 30);
	peluruPesawat.v = Point(0, 120);
	bool existPeluruPesawat = false;

	Bullet peluruKapal;
	peluruKapal.size = Point(0, -30);
	peluruKapal.v = Point(0, -120);
	bool existPeluruKapal = false;

	int framerate = 20;
	float period = 1. / framerate;
	float last_render = clock();
	float periodeKapal = 3.0, periodePesawat = 1.0;
	
	int iframe = 0;

	while (true) {
		// Draw objek
		fb.clear();
		printf("Frame %d\n", iframe++);

		// pesawat & kapal
		plane.draw(&fb);
		ship.draw(&fb);

		// baling-baling
		Sprite tempBlade = blade;
		tempBlade.rotate(17 * spin++, Point(5, 50));
		tempBlade.draw(&fb);

		// peluru
		if (existPeluruKapal)
			peluruKapal.draw(&fb);

		// delay sesuai framerate
		float now_render = clock();
		usleep(std::max(0.f, 1000 * 1000 / framerate - (now_render - last_render)));
		last_render = now_render;

		// update objek
		// pesawat
		plane.update(period);
		if (plane.getPos().x >= fb.getXSize()) {
			plane.getPos().x = 0;
		}
		else if (plane.getPos().x + plane.getBottomRight().x < 0) {
			plane.getPos().x = fb.getXSize() - 1;
		}

		// kapal
		ship.update(period);
		if (ship.getPos().x >= fb.getXSize()) {
			ship.getPos().x = 0;
		}
		else if (ship.getPos().x + ship.getBottomRight().x < 0) {
			ship.getPos().x = fb.getXSize() - 1;
		}

		// baling-baling
		blade.setPos(plane.getPos()+Point(190, -50));

		// peluruKapal kapal
		periodeKapal = periodeKapal - period;
		if (periodeKapal < 0) {
			existPeluruKapal = true;
			periodeKapal = 3.0;
			peluruKapal.p.x = ship.getPos().x + ship.getTopLeft().x;// + (ship.getBottomRight().x - ship.getTopLeft().x) / 2;
			peluruKapal.p.y = ship.getPos().y;
		}
		if (existPeluruKapal)
			peluruKapal.update(period);

		if (existPeluruKapal && (plane.collide(peluruKapal.p) || plane.collide(peluruKapal.p + peluruKapal.size))) {
			periodPlaneExplosion = 1.0;
			planeExplosionLocation = plane.getPos();
			break;
		}
	}

	// hambur-hamburkan semuanya!
	int gravity = 60;

	// plane pieces
	int nPieces = 3;
	std::vector<Sprite> pieces;
	for (int i = 1; i <= nPieces; ++i) {
		Sprite piece(Color(127, 127, 255), Color::BLUE);
		char temp[100];
		sprintf(temp, "object/piece%d", i);
		piece.add(temp);
		piece.setPos(planeExplosionLocation);
		piece.resize(0.4);
		pieces.push_back(piece);
	}

	// set directions
	int pieceSpin[3], pieceDSpin[3];
	pieces[0].setV(Point(-80, 0));
	pieces[1].setV(Point(0, 20));
	pieces[2].setV(Point(40, 0));
	for (int i = 0; i < nPieces; ++i) {
		pieces[i].setA(Point(0, gravity));
		pieceSpin[i] = 0;
		pieceDSpin[i] = (i + 1) * 10;
	}

	// baling-baling
	blade.setV(Point(-30, 0));
	blade.setA(Point(0, gravity));
	int bladeDSpin = 17;
	bool showBlade = true;

	// ban
	Sprite tire(Color::WHITE, Color::BLUE);
	tire.add("object/tire");
	tire.setPos(planeExplosionLocation + Point(170, 30));
	tire.resize(.15);
	tire.setV(Point(-80, 300));
	tire.setA(Point(0, gravity));

	// parasut
	Sprite parasut(Color::WHITE);
	parasut.add("object/parasut");
	parasut.setPos(planeExplosionLocation + Point(60, 0));
	parasut.setV(Point(0, 20));
	parasut.setA(Point(0, gravity / 2));
	parasut.resize(0.3);
	int parasutSpin = 0, parasutSpinV = 3;

	// bum bum
	Sprite boom(Color::WHITE, Color:: RED);
	boom.add("object/explosion1");
	boom.setPos(planeExplosionLocation);
	bool showBoom = true;

	// ground limit
	int groundY = 400 + ship.getSize().y;

	// do the thing!
	while (true) {
		// Draw objek
		fb.clear();
		printf("Frame %d\n", iframe++);

		// baling-baling
		if (showBlade) {
			Sprite tempBlade = blade;
			tempBlade.rotate(bladeDSpin * spin++, blade.getTopLeft() + Point(blade.getSize().x / 2, blade.getSize().y / 2));
			tempBlade.draw(&fb);
		}

		// ban
		tire.draw(&fb);

		// boom
		if (showBoom) {
			boom.draw(&fb);
		}

		// parasut
		Sprite tempParasut = parasut;
		tempParasut.rotate(parasutSpin, Point((parasut.getBottomRight().x + parasut.getTopLeft().x) / 2, parasut.getTopLeft().y));
		tempParasut.draw(&fb);

		// puing pesawat
		for (int i = 0; i < nPieces; ++i) {
			Sprite tempPiece = pieces[i];
			Point center = pieces[i].getTopLeft() + Point(pieces[i].getSize().x / 2, pieces[i].getSize().y / 2);
			// Circle(center.x, center.y, 3, Color::RED).drawSolid(&fb);
			tempPiece.rotate(pieceDSpin[i] * pieceSpin[i]++, center);
			tempPiece.draw(&fb);
		}

		// kapal
		ship.draw(&fb);

		// delay sesuai framerate
		float now_render = clock();
		usleep(std::max(0.f, 1000 * 1000 / framerate - (now_render - last_render)));
		last_render = now_render;

		// update objek
		// puing pesawatcount
		for (int i = 0; i < nPieces; ++i)
		{
			pieces[i].update(period);
			if (pieces[i].getBottomRight().y + pieces[i].getPos().y >= groundY) {
				pieces[i].setV(Point(0, 0));
				pieceDSpin[i] = 0;
			}
		}

		// kapal
		ship.update(period);
		if (ship.getPos().x >= fb.getXSize()) {
			ship.getPos().x = 0;
		}
		else if (ship.getPos().x + ship.getBottomRight().x < 0) {
			ship.getPos().x = fb.getXSize() - 1;
		}

		// baling-baling
		blade.update(period);
		// blade.resize((blade.getSize().x + 2.) / blade.getSize().x);
		if (blade.getTopLeft().y + blade.getPos().y >= groundY) {
			blade.setV(Point(0, 0));
			showBlade = false;
		}

		// ban
		tire.update(period);
		if (tire.getPos().y + tire.getBottomRight().y >= groundY && tire.getV().y > 0) {
			tire.setV(Point(tire.getV().x, -tire.getV().y * 0.3));
			if (tire.getV().y == 0) {
				tire.setV(Point(0, 0));
			}
		}

		// parasut
		parasut.update(period);
		if (parasutSpin < -30 || parasutSpin > 30) {
			parasutSpinV *= -1;
		}
		parasutSpin += parasutSpinV;
		if (parasut.getBottomRight().y + parasut.getPos().y >= groundY) {
			parasut.setV(Point(0, 0));
			parasutSpin = parasutSpinV = 0;
		}

		// boom
		boom.resize(1.1, boom.getTopLeft() + Point(boom.getSize().x / 2, boom.getSize().y / 2));
		if (boom.getSize().x >= 300) {
			showBoom = false;
		}
	}

	return 0;
}