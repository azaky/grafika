#include "uts_files/color_picker.cpp"
#include "uts_files/mainscene.cpp"
#include "uts_files/plane_explode.cpp"
#include "uts_files/ship_explode.cpp"

int main(int argc, char** argv) {
	// set up framerate
	int framerate = 10;
	float period = 1. / framerate;

	ColorSelection();
	initializeMap();
	while (true) {
		if (Game()) { // if plane explode
			PlaneExplode();
		}
		else {
			ShipExplode();
		}
	}
	return 0;
}
