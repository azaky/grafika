#include "lib/framebuffer.h"
#include "lib/sprite.h"

int main() {
	FrameBuffer fb;

	FrameMatrix birdPattern;

	Sprite bird(Color(0, 127, 255), Color::BLUE, 1);
	bird.add("object/bird");
	bird.draw(&birdPattern);
	bird.drawFill(&birdPattern, NULL);


	Sprite plane(Color::WHITE);
	plane.add("object/plane");
	plane.draw(&fb, Point(200, 200));
	plane.drawFill(&fb, &birdPattern, Point(200, 200));

	FrameMatrix fishPattern;

	Sprite fish(Color(0, 127, 255), Color::BLUE, 1);
	fish.add("object/fish");
	fish.draw(&fishPattern);
	fish.drawFill(&fishPattern, NULL);

	Sprite kapal(Color(255, 127, 127), Color::RED, 1);
	kapal.add("object/ship");	
	kapal.draw(&fb, Point(20, 20));
	kapal.drawFill(&fb, &fishPattern, Point(20, 20));

	// Sprite fish(Color(0, 127, 255), Color::BLUE, 1);
	// fish.add("object/fish");
	// fish.draw(&fb, Point(200, 200));
	// fish.drawFill(&fb, NULL, Point(200, 200));
	return 0;
}